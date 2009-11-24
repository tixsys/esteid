/*
 * Copyright (C) 2009  Manuel Matonin <manuel@smartlink.ee>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */ 

using System;
using System.IO;
using System.Collections;
using System.Net;

using iTextSharp.text.pdf;

using Org.BouncyCastle.X509;
using Org.BouncyCastle.Math;
using Org.BouncyCastle.Ocsp;
using Org.BouncyCastle.Asn1;
using Org.BouncyCastle.Asn1.X509;
using Org.BouncyCastle.Asn1.Ocsp;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Security;
using Org.BouncyCastle.Utilities;
using Org.BouncyCastle.Utilities.Date;

using EstEIDSigner.Properties;

namespace EstEIDSigner
{
    public class OCSPClientEstEID
        : ErrorContainer, IOcspClient 
    {
        private X509Certificate signerCert;
        private X509Certificate issuerCert;
        private X509Certificate checkerCert;
        private X509Certificate responderCert;
        private String url;
        private AsymmetricKeyParameter checkerKey;
        private OcspReq ocspRequest = null;
        private OcspResp ocspResponse = null;

        public enum CertStatus : int
        {
            UNKNOWN = -2,
            REVOKED = -1,
            GOOD = 0,
            ERROR = 1
        }

        /// <summary>Class that carries OCSP request and handles response.</summary>        
        public OCSPClientEstEID(X509Certificate signerCert,
            X509Certificate checkerCert,
            X509Certificate issuerCert, 
            String url, 
            AsymmetricKeyParameter checkerKey,
            X509Certificate responderCert)
        {
            this.signerCert = signerCert;
            this.checkerCert = checkerCert;
            this.issuerCert = issuerCert;
            this.responderCert = responderCert;
            this.url = url;
            this.checkerKey = checkerKey;
        }

        public OcspReq OcspRequest
        {
            get { return ocspRequest; }
        }

        public BasicOcspResp OcspResponse
        {
            get { return (BasicOcspResp)ocspResponse.GetResponseObject(); }
        }

        private static OcspReq GenerateOCSPRequest(X509Certificate signerCert,
            X509Certificate checkerCert,
            X509Certificate issuerCert,
            AsymmetricKeyParameter checkerKey) 
        {
            // Generate the id for the certificate we are looking for
            CertificateID id = new CertificateID(CertificateID.HashSha1, issuerCert, signerCert.SerialNumber);            
            
            // basic request generation with nonce
            OcspReqGenerator gen = new OcspReqGenerator();
            
            gen.AddRequest(id);
            
            // create details for nonce extension
            ArrayList oids = new ArrayList();
            ArrayList values = new ArrayList();
            
            oids.Add(OcspObjectIdentifiers.PkixOcspNonce);
            values.Add(new X509Extension(false, new DerOctetString(new DerOctetString(PdfEncryption.CreateDocumentId()).GetEncoded())));
            
            gen.SetRequestExtensions(new X509Extensions(oids, values));
            
            X509Certificate [] chain = new X509Certificate[2];
            chain[0] = checkerCert;
            chain[1] = signerCert;

            gen.SetRequestorName(checkerCert.SubjectDN);

            return gen.Generate(checkerCert.SigAlgOid, checkerKey, chain);
        }

        private bool CheckTimeValidity(DateTime thisUpate, DateTimeObject nextUpdate, int skew, int maxage)
        {
            DateTime tmp;
            long t1, t2;
            DateTime now = DateTime.UtcNow;
            t1 = now.Ticks;

            tmp = thisUpate.AddMilliseconds(skew);
            t2 = tmp.Ticks;

            if (t2 < t1)
                return false;

            tmp = thisUpate.AddMilliseconds(maxage);
            t2 = tmp.Ticks;
            if (t2 < t1)
                return false;

            if (nextUpdate != null)
            {
                DateTime next = nextUpdate.Value;

                tmp = next.AddMilliseconds(-skew);
                t2 = tmp.Ticks;

                if (t2 < t1)
                    return false;

                // nextUpdate precedes thisUpdate ?
                if (thisUpate.CompareTo(next) > 0)
                    return false;
            }

            return true;
        }

        private int VerifyOCSPResponse()
        {
            if (ocspResponse.Status != 0)
            {
                this.lastError = Resources.INVALID_OCSP_STATUS + ocspResponse.Status;
                return (int)CertStatus.ERROR;
            }

            BasicOcspResp basicResponse = (BasicOcspResp)ocspResponse.GetResponseObject();

            if (basicResponse != null)
            {
                if (responderCert != null)
                {
                    bool verifyResult = basicResponse.Verify(responderCert.GetPublicKey());

                    if (verifyResult != true)
                    {
                        this.lastError = Resources.OCSP_VERIFY_FAILED;
                        return (int)CertStatus.ERROR;
                    }
                }

                SingleResp[] responses = basicResponse.Responses;

                byte[] reqNonce = ocspRequest.GetExtensionValue(OcspObjectIdentifiers.PkixOcspNonce).GetEncoded();
                byte[] respNonce = basicResponse.GetExtensionValue(OcspObjectIdentifiers.PkixOcspNonce).GetEncoded();

                if (reqNonce == null || Arrays.AreEqual(reqNonce, respNonce))
                {
                    // will handle single response only
                    if (responses.Length != 1)
                    {
                        this.lastError = Resources.INVALID_OCSP_RESPONSE_COUNT + responses.Length;
                        return (int)CertStatus.ERROR;
                    }

                    SingleResp resp = responses[0];

                    // Check that response creation time is in valid time slot.
                    DateTime thisUpdate = resp.ThisUpdate;
                    DateTimeObject nextUpdate = resp.NextUpdate;
                    DateTime now = DateTime.UtcNow;

                    // Check whether response creation ramained in valid slot
                    bool valid = CheckTimeValidity(thisUpdate, nextUpdate, 5000, 100000);
                    if (valid == false)
                    {
                        this.lastError = Resources.INVALID_OCSP_TIMESLOT;
                        return (int)CertStatus.ERROR;
                    }

                    Object status = resp.GetCertStatus();
                    
                    if (status == CertificateStatus.Good)
                    {
                        // Ok
                        return (int)CertStatus.GOOD;
                    }                    
                    else if (status is RevokedStatus)
                    {                        
                        this.lastError = string.Format(Resources.OCSP_ERROR, 
                            resp.GetCertID().SerialNumber,
                            "revoked");
                        return (int)CertStatus.REVOKED;
                    }
                    else
                    {
                        this.lastError = string.Format(Resources.OCSP_ERROR,
                            resp.GetCertID().SerialNumber,
                            "unknown");
                        return (int)CertStatus.UNKNOWN;
                    }
                }                
            }
            else
                this.lastError = Resources.INVALID_OCSP_RESPONSE;

            // failed
            return (int)CertStatus.ERROR;
        }

        public byte[] GetEncoded() 
        {
            ocspRequest = GenerateOCSPRequest(signerCert, checkerCert, issuerCert, checkerKey);
            byte[] array = ocspRequest.GetEncoded();
            HttpWebRequest con = (HttpWebRequest)WebRequest.Create(url);
            con.ContentLength = array.Length;
            con.ContentType = "application/ocsp-request";
            con.Accept = "application/ocsp-response";
            con.Method = "POST";
            Stream outp = con.GetRequestStream();
            outp.Write(array, 0, array.Length);
            outp.Close();
            HttpWebResponse response = (HttpWebResponse)con.GetResponse();
            if (response.StatusCode != HttpStatusCode.OK)
            {
                this.lastError = "Invalid HTTP response: " + (int)response.StatusCode;
                return null;
            }

            Stream inp = response.GetResponseStream();
            ocspResponse = new OcspResp(inp);
            inp.Close();
            response.Close();

            int verify = VerifyOCSPResponse();
            if (verify != (int)CertStatus.GOOD)
                return null;

            return ((BasicOcspResp)ocspResponse.GetResponseObject()).GetEncoded();
        }
    }
}