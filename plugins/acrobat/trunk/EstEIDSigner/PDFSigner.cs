﻿/*
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
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using Microsoft.Win32;

using iTextSharp;
using iTextSharp.text;
using iTextSharp.text.pdf;
using iTextSharp.text.xml.xmp;

using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.X509;
using Org.BouncyCastle.Pkcs;
using Org.BouncyCastle.Cms;
using Org.BouncyCastle.Asn1;
using Org.BouncyCastle.Asn1.Pkcs;
using Org.BouncyCastle.Ocsp;
using Org.BouncyCastle.Utilities;
using Org.BouncyCastle.Security;

using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography.Pkcs;
using System.Security.Cryptography;
using System.Windows.Forms;
using System.Resources;
using System.Reflection;
using System.Configuration;

using EstEIDNative;
using EstEIDSigner.Properties;

///
/// <summary>
/// This Library allows you to sign a PDF document using iTextSharp and SmartCard
/// </summary>
/// <author>Manuel Matonin</author>
///
///

namespace EstEIDSigner
{
    /// <summary>
    /// This class hold the Timestamp url and password for e-signature 
    /// </summary>
    class Timestamp
    {
        private string url;
        private string username;
        private string password;

        public Timestamp()
        {
        }
        public Timestamp(string url)
        {
            this.url = url;
        }
        public Timestamp(string url, string cusername, string cpassword)
        {
            this.url = url;
            this.username = cusername;
            this.password = cpassword;
        }
        public string Url
        {
            set { url = value; }
            get { return url; }
        }
        public string User
        {
            set { username = value; }
            get { return username; }
        }
        public string Password
        {
            set { password = value; }
            get { return password; }
        }
    }

    class SignatureLocation
    {
        private Rectangle rectangle;

        public SignatureLocation(EstEIDSettings config)
        {
            rectangle = new Rectangle(
                config.ToUInt("signature_x", EstEIDSettings.SignatureX),
                config.ToUInt("signature_y", EstEIDSettings.SignatureY),
                config.ToUInt("signature_w", EstEIDSettings.SignatureW),
                config.ToUInt("signature_h", EstEIDSettings.SignatureH));
        }

        public static implicit operator Rectangle(SignatureLocation location)
        {
            return (location.rectangle);
        }
    }


    /// <summary>
    /// This is a holder class for PDF signature appearance
    /// </summary>
    class Appearance
    {
        private string reason;
        private string location;
        private string contact;
        private bool visible;
        private Rectangle rectangle;
        private uint page;
        private PdfSignatureAppearance.SignatureRender render;

        public Appearance()
        {
        }
        public Appearance(string SigReason, string SigLocation, string SigContact, bool SigVisible)
        {
            reason = SigReason;
            location = SigLocation;
            contact = SigContact;
            visible = SigVisible;
        }
        public string Reason
        {
            set { reason = value; }
            get { return reason; }
        }
        public string Location
        {
            set { location = value; }
            get { return location; }
        }
        public string Contact
        {
            set { contact = value; }
            get { return contact; }
        }
        public bool Visible
        {
            set { visible = value; }
            get { return visible; }
        }
        public Rectangle Rectangle
        {
            set { rectangle = value; }
            get { return rectangle; }
        }
        public uint Page
        {
            set { page = value; }
            get { return page; }
        }
        public PdfSignatureAppearance.SignatureRender SignatureRender
        {
            set { render = value; }
            get { return render; }
        }
    }

    /// <summary>
    /// This is a holder class for PDF metadata
    /// </summary>
    class MetaData
    {
        private Hashtable info = new Hashtable();

        public Hashtable Info
        {
            get { return info; }
            set { info = value; }
        }

        public string Author
        {
            get { return (string)info["Author"]; }
            set { info.Add("Author", value); }
        }
        public string Title
        {
            get { return (string)info["Title"]; }
            set { info.Add("Title", value); }
        }
        public string Subject
        {
            get { return (string)info["Subject"]; }
            set { info.Add("Subject", value); }
        }
        public string Keywords
        {
            get { return (string)info["Keywords"]; }
            set { info.Add("Keywords", value); }
        }
        public string Producer
        {
            get { return (string)info["Producer"]; }
            set { info.Add("Producer", value); }
        }

        public string Creator
        {
            get { return (string)info["Creator"]; }
            set { info.Add("Creator", value); }
        }

        public Hashtable getMetaData()
        {
            return this.info;
        }
        public byte[] getStreamedMetaData()
        {
            MemoryStream os = new System.IO.MemoryStream();
            XmpWriter xmp = new XmpWriter(os, this.info);
            xmp.Close();
            return os.ToArray();
        }
    }

    /// <summary>
    /// base class for exception where user action is canceled
    /// </summary>
    public class CancelException
        : Exception
    {
        public CancelException(string message)
            : base(message)
        {
        }
    }

    /// <summary>
    /// base class for exception where cert status is not good
    /// </summary>
    public class RevocationException
        : Exception
    {
        public RevocationException(string message)
            : base(message)
        {
        }
    }

    /// <summary>
    /// base class for exception where cert status is not good
    /// </summary>
    public class DocVerifyException
        : Exception
    {
        public DocVerifyException(string message)
            : base(message)
        {
        }
    }

    /// <summary>
    /// this is the most important class
    /// it uses iTextSharp library to sign PDF document
    /// </summary>
    class PDFSigner
        : ErrorContainer
    {
        public const uint SIGNATURE_LENGTH = 128;
        private string inputPDF = string.Empty;
        private string outputPDF = string.Empty;
        private Timestamp stamp = null;
        private MetaData metadata = null;
        private Appearance appearance = null;
        private DirectoryX509CertStore store = null;
        private PdfReader reader = null;
        public delegate void OnStatus(string s, bool error);
        private OnStatus statusHandler;
        private EstEIDSettings config = null;
        private PKCS12Settings credentials = null;

        public PDFSigner(string input, string output)
        {
            this.inputPDF = input;
            this.outputPDF = output;
        }
        public PDFSigner(string input, string output, Timestamp tmstamp)
        {
            this.inputPDF = input;
            this.outputPDF = output;
            this.stamp = tmstamp;
        }
        public PDFSigner(string input, string output, MetaData md)
        {
            this.inputPDF = input;
            this.outputPDF = output;
            this.metadata = md;
        }
        public PDFSigner(string input, string output, Appearance app)
        {
            this.inputPDF = input;
            this.outputPDF = output;
            this.appearance = app;
        }
        public PDFSigner(PdfReader reader, string input, string output, Timestamp tmstamp, MetaData md, Appearance app, DirectoryX509CertStore store)
        {
            this.inputPDF = input;
            this.outputPDF = output;
            this.stamp = tmstamp;
            this.metadata = md;
            this.appearance = app;
            this.store = store;
            this.reader = reader;
        }
        public OnStatus StatusHandler
        {
            set { statusHandler = value; }
        }
        public EstEIDSettings Settings
        {
            set { config = value; }
        }
        public PKCS12Settings Credentials
        {
            set { credentials = value; }
        }

        public void Verify()
        {
        }

        private string ReadPin(TokenInfo token)
        {
            FormPin form = new FormPin(token.Label, (int)token.MinPin, (int)token.MaxPin);
            DialogResult dr = form.ShowDialog();
            if (dr == DialogResult.OK)
                return form.Pin;
            else if (dr == DialogResult.Cancel)
                return null;

            return (string.Empty);
        }

        private byte[] EstEIDCardSign(EstEIDReader estEidReader, PKCS11Signer signer, Byte[] digest)
        {
            uint rc, slot;
            const uint rsa_length = SIGNATURE_LENGTH;
            uint digest_length = rsa_length;
            string pin = string.Empty;
            TokenInfo token = null;

            HGlobalSafeHandle raw = new HGlobalSafeHandle((int)rsa_length);
            IntPtr rsaBytes = raw;
            if (rsaBytes == IntPtr.Zero)
                throw new OutOfMemoryException(Resources.OUT_OF_MEMORY);

            token = signer.Token;
            slot = signer.Slot;

            if (token.LoginRequired && token.PinIsSet)
            {
                if (!token.PinPadPresent)
                {
                    pin = ReadPin(token);

                    // redraw window: single-threaded UI
                    Application.DoEvents();

                    // user requested Cancel ?
                    if (pin == null)
                        throw new CancelException(Resources.ACTION_CANCELED);
                    // no pin supplied ?
                    else if (pin == string.Empty)
                        throw new Exception(Resources.NO_PIN_SUPPLIED);
                }
                else
                {
                    // we have a PINPAD present
                    pin = null;
                    statusHandler(string.Format(Resources.UI_ENTER_PIN_ONTHE_PINPAD, token.Label), false);
                }
            }

            rc = estEidReader.Sign(slot, pin, digest, (uint)digest.Length, ref rsaBytes, ref digest_length);
            // failure ?
            if (rc != EstEIDReader.ESTEID_OK)
            {
                // signing cancelled or timed out
                if (rc == PKCS11Error.CKR_FUNCTION_CANCELED)
                    throw new CancelException(Resources.ACTION_CANCELED);

                throw new PKCS11Exception(rc);
            }

            return raw.ToByteArray();
        }

        private byte[] ComputeHash(EstEIDReader estEidReader, PdfSignatureAppearance sap)
        {
            Digest hash = new Digest(estEidReader);
            const uint hash_length = Digest.SHA1_LENGTH;
            uint digest_length = 0;
            uint rc;

            rc = hash.InitDigest(Digest.HashAlgorithm.SHA1);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(Resources.CARD_HASH_INIT);

            Stream s = sap.RangeStream;
            MemoryStream ss = new MemoryStream();
            int read = 0;
            byte[] buff = new byte[8192];
            while ((read = s.Read(buff, 0, 8192)) > 0)
            {
                ss.Write(buff, 0, read);
                rc = hash.UpdateDigest(buff, (uint)read);
                if (rc != EstEIDReader.ESTEID_OK)
                    throw new Exception(Resources.CARD_HASH_UPDATE);
            }

            HGlobalSafeHandle raw = new HGlobalSafeHandle((int)hash_length);
            IntPtr hashBytes = raw;
            if (hashBytes == IntPtr.Zero)
                throw new OutOfMemoryException(Resources.OUT_OF_MEMORY);

            rc = hash.FinalizeDigest(ref hashBytes, ref digest_length);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(Resources.CARD_HASH_FINALIZE);

            return raw.ToByteArray();
        }

        private byte[] ComputeHash(EstEIDReader estEidReader, byte[] data)
        {
            uint rc, digest_length = 0;
            const uint hash_length = Digest.SHA1_LENGTH;

            Digest hash = new Digest(estEidReader);

            rc = hash.InitDigest(Digest.HashAlgorithm.SHA1);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(Resources.CARD_HASH_INIT);

            rc = hash.UpdateDigest(data, (uint)data.Length);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(Resources.CARD_HASH_UPDATE);

            HGlobalSafeHandle raw = new HGlobalSafeHandle((int)hash_length);
            IntPtr hashBytes = raw;
            if (hashBytes == IntPtr.Zero)
                throw new OutOfMemoryException(Resources.OUT_OF_MEMORY);

            rc = hash.FinalizeDigest(ref hashBytes, ref digest_length);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(Resources.CARD_HASH_FINALIZE);

            return raw.ToByteArray();
        }

        private byte[] TimestampAuthorityResponse(EstEIDReader estEidReader, byte[] signedPkcs)
        {
            ArrayList newSigners = new ArrayList();

            CmsSignedData sd = new CmsSignedData(signedPkcs);

            foreach (SignerInformation si in sd.GetSignerInfos().GetSigners())
            {
                // possible TSA URLs
                //string TsaServerUrl = "http://www.edelweb.fr/cgi-bin/service-tsp"; 
                //string TsaServerUrl = "http://dse200.ncipher.com/TSS/HttpTspServer";

                byte[] signedDigest = si.GetSignature();
                byte[] timeStampHash = ComputeHash(estEidReader, signedDigest);

                string TsaServerUrl = stamp.Url;
                string TsaUser = stamp.User;
                string TsaPassword = stamp.Password;
                string error = string.Empty;

                byte[] timeStampToken = X509Utils.GetTimestampToken(TsaServerUrl,
                    TsaUser,
                    TsaPassword,
                    timeStampHash,
                    ref error);

                if (timeStampToken == null)
                    throw new Exception(Resources.TSA_ERROR + error);

                Hashtable ht = new Hashtable();
                Asn1Object derObj = new Asn1InputStream(timeStampToken).ReadObject();
                DerSet derSet = new DerSet(derObj);

                Org.BouncyCastle.Asn1.Cms.Attribute unsignAtt = new Org.BouncyCastle.Asn1.Cms.Attribute(
                    new DerObjectIdentifier(X509Utils.ID_TIME_STAMP_TOKEN), derSet);

                ht.Add(X509Utils.ID_TIME_STAMP_TOKEN, unsignAtt);

                Org.BouncyCastle.Asn1.Cms.AttributeTable unsignedAtts = new Org.BouncyCastle.Asn1.Cms.AttributeTable(ht);

                newSigners.Add(SignerInformation.ReplaceUnsignedAttributes(si, unsignedAtts));
            }

            SignerInformationStore newSignerInformationStore = new SignerInformationStore(newSigners);

            CmsSignedData newSd = CmsSignedData.ReplaceSigners(sd, newSignerInformationStore);

            // Encode the CMS/PKCS #7 message
            return newSd.GetEncoded();
        }

        private PKCS11Signer LocateSigner(EstEIDReader estEidReader)
        {
            uint slots;
            uint i, rc;
            X509Certificate2Collection col = null;
            Mechanism mech = null;
            PKCS11Signer signer = null;
            X509Certificate2 cert = null;
            ArrayList signers = null;

            slots = estEidReader.GetSlotCount(1);
            if (slots == 0)
                throw new Exception(Resources.CARD_MISSING);

            col = new X509Certificate2Collection();
            mech = new Mechanism(Mechanism.CKF_SIGN | Mechanism.CKF_HW);
            signers = new ArrayList((int)slots);

            for (i = 0; i < slots; i++)
            {
                rc = estEidReader.IsMechanismSupported(i, mech);
                // can't use this slot mechanism for signing
                if (rc != EstEIDReader.ESTEID_OK)
                {
                    // Mechanism not supported ?
                    if (rc == EstEIDReader.ESTEID_ERR)
                        continue;
                    throw new PKCS11Exception(rc);
                }

                TokenInfo token = new TokenInfo(estEidReader);
                rc = token.ReadInfo(i);
                if (rc != EstEIDReader.ESTEID_OK)
                    throw new PKCS11Exception(rc);

                PKCS11Cert pkcs11Cert = new PKCS11Cert();
                rc = estEidReader.LocateCertificate(i, pkcs11Cert);
                if (rc != EstEIDReader.ESTEID_OK)
                {
                    // did we find a certificate ?
                    if (rc == EstEIDReader.ESTEID_ERR)
                        continue;
                    throw new PKCS11Exception(rc);
                }

                byte[] raw = pkcs11Cert.CertificateToByteArray();

                // can we use this cert for signing ?
                string s = X509Utils.GetSubjectFields(raw, "OU");
                if (!s.Equals("digital signature"))
                    continue;

                cert = new X509Certificate2(raw);
                col.Add(cert);

                signer = new PKCS11Signer(i, cert, token);
                signers.Add(signer);
            }

            // no valid certs found ?
            if (col.Count == 0)
                throw new Exception(Resources.CERTS_MISSING);

            X509Certificate2Collection sel = X509CertificateUI.SelectFromCollection(col,
                Resources.UI_CERTIFICATES, Resources.UI_PICK_CERTIFICATE);

            // user requested Cancel or there are no certs ?
            if (sel == null || sel.Count == 0)
                throw new CancelException(Resources.ACTION_CANCELED);

            X509Certificate2Enumerator en = sel.GetEnumerator();
            en.MoveNext();
            cert = en.Current;
            byte[] s1 = cert.PublicKey.EncodedKeyValue.RawData;

            IEnumerator enumerator = signers.GetEnumerator();

            while (enumerator.MoveNext())
            {
                signer = (PKCS11Signer)enumerator.Current;

                byte[] s2 = signer.Cert.PublicKey.EncodedKeyValue.RawData;
                if (Arrays.AreEqual(s1, s2))
                    return signer;
            }

            // didn't find any match ?
            // data is being altered in memory ?
            throw new Exception(Resources.CERT_DONT_MATCH);
        }

        private OCSPClientEstEID OCSPClient(PKCS12Settings credentials, Org.BouncyCastle.X509.X509Certificate signer)
        {
            string certPath = credentials.Filename;
            string certPassword = credentials.Password;

            X509CertStoreEntry storeEntry = null;
            Org.BouncyCastle.X509.X509Certificate checkerCert, issuerCert, responderCert;

            // open PKCS12 store
            DirectoryPKCS12CertStore pkcs12 = new DirectoryPKCS12CertStore(certPath, certPassword);
            if (pkcs12.Open() == false)
                throw new Exception(pkcs12.lastError);
            // public key
            checkerCert = pkcs12.Chain[0];
            // private key
            AsymmetricKeyParameter privKey = (AsymmetricKeyParameter)pkcs12.Key;

            // signing cert issuer name
            string signerKey = X509Utils.GetIssuerFields(signer, "CN");
            if (signerKey.Length == 0)
            {
                this.lastError = Resources.CERT_ISSUER_MISSING;
                return null;
            }

            // load issuer cert
            storeEntry = this.store[signerKey, true];
            if (storeEntry == null)
            {
                this.lastError = Resources.ISSUER_CERT_MISSING + signerKey;
                return null;
            }
            issuerCert = storeEntry.Certificate;

            // load responder cert, will be used for verify
            storeEntry = this.store[signerKey, false];
            if (storeEntry == null)
            {
                this.lastError = Resources.RESPONDER_CET_MISSING + signerKey;
                return null;
            }
            responderCert = storeEntry.Certificate;

            string ocspUrl = config.ToString("ocsp_url");
            if (ocspUrl == null || ocspUrl.Length == 0)
            {
                this.lastError = Resources.OCSP_URL_MISSING;
                return null;
            }

            return new OCSPClientEstEID(signer, checkerCert, issuerCert, ocspUrl, privKey, responderCert);
        }

        private void SignUsingEstEIDCard2(PKCS12Settings credentials, string filename, string outfile)
        {
            statusHandler(Resources.VERIFYING_DOCUMENT, false);

            AcroFields af = this.reader.AcroFields;
            ArrayList names = af.GetSignatureNames();
            bool nextRevision = ((names != null) && (names.Count > 0));

            // already signed ?
            if (nextRevision)
            {
                // pick always first signature 
                string name = (string)names[0];
                PdfPKCS7 pkc7 = af.VerifySignature(name);
                bool verify = pkc7.Verify();
                if (!verify)
                {
                    string who = PdfPKCS7.GetSubjectFields(pkc7.SigningCertificate).GetField("CN");
                    throw new DocVerifyException(Resources.DOC_VERIFY_FAILED + who);
                }
            }

            statusHandler(Resources.CONNECTING_SMARTCARD, false);

            // open EstEID
            EstEIDReader estEidReader = new EstEIDReader();
            string pkcs11_lib = config.ToString("pkcs11_library");
            bool b = estEidReader.Open(pkcs11_lib);
            if (b == false)
                throw new Exception(Resources.PKCS11_OPEN);

            statusHandler(Resources.READ_CERTS, false);
            PKCS11Signer signer = LocateSigner(estEidReader);
            Org.BouncyCastle.X509.X509Certificate[] chain = X509Utils.LoadCertificate(signer.Cert.RawData);

            statusHandler(Resources.VERIFYING_OCSP, false);
            OCSPClientEstEID ocspClient = OCSPClient(credentials, chain[0]);
            if (ocspClient == null)
                throw new Exception(this.lastError);

            byte[] ocsp = ocspClient.GetEncoded();
            if (ocsp == null)
                throw new RevocationException(ocspClient.lastError);

            X509Certificate2 card = signer.Cert;
            Oid oid = card.SignatureAlgorithm;

            if (oid.Value != PkcsObjectIdentifiers.Sha1WithRsaEncryption.Id)
                throw new Exception(Resources.INVALID_CERT);

            PdfReader reader = new PdfReader(filename);
            PdfStamper stp = PdfStamper.CreateSignature(reader, new FileStream(outfile, FileMode.Create), '\0', null, nextRevision);
            if (metadata != null)
                stp.XmpMetadata = metadata.getStreamedMetaData();
            PdfSignatureAppearance sap = stp.SignatureAppearance;
            if (appearance.Visible)
                sap.SetVisibleSignature(appearance.Rectangle, (int)appearance.Page, null);
            sap.SignDate = DateTime.Now;
            sap.SetCrypto(null, chain, null, null);
            sap.Reason = (appearance.Reason.Length > 0) ? appearance.Reason : null;
            sap.Location = (appearance.Location.Length > 0) ? appearance.Location : null;
            sap.Contact = (appearance.Contact.Length > 0) ? appearance.Contact : null;
            sap.Acro6Layers = true;
            sap.Render = appearance.SignatureRender;
            PdfSignature dic = new PdfSignature(PdfName.ADOBE_PPKLITE, PdfName.ADBE_PKCS7_SHA1);
            dic.Date = new PdfDate(sap.SignDate);
            dic.Name = PdfPKCS7.GetSubjectFields(chain[0]).GetField("CN");
            if (sap.Reason != null)
                dic.Reason = sap.Reason;
            if (sap.Location != null)
                dic.Location = sap.Location;
            if (sap.Contact != null)
                dic.Contact = sap.Contact;
            sap.CryptoDictionary = dic;
            sap.SetExternalDigest(new byte[SIGNATURE_LENGTH], new byte[Digest.SHA1_LENGTH], "RSA");

            // expect 6K to be enough if TSA response, else 2K ?
            int csize = (stamp != null) ? 1024 * 6 : 1024 * 2;
            Hashtable exc = new Hashtable();
            exc[PdfName.CONTENTS] = csize * 2 + 2;
            sap.PreClose(exc);

            // compute hash based on PDF bytes
            byte[] digest = ComputeHash(estEidReader, sap);

            statusHandler(Resources.ADD_SIGNATURE, false);
            // sign hash
            byte[] rsadata = EstEIDCardSign(estEidReader, signer, digest);
            // if null, user requested Cancel
            if (rsadata == null)
                throw new Exception(Resources.CARD_INTERNAL_ERROR);

            // create PKCS#7 envelope
            PdfPKCS7 pk7 = new PdfPKCS7(null, chain, null, "SHA1", true);
            pk7.SetExternalDigest(rsadata, digest, "RSA");

            byte[] pk = pk7.GetEncodedPKCS7();

            // user wants to add TSA response ?
            if (stamp != null && pk != null)
            {
                statusHandler(Resources.TSA_REQUEST, false);
                pk = TimestampAuthorityResponse(estEidReader, pk);
            }

            // PKCS#7 bytes too large ?
            if (pk.Length >= csize)
                throw new Exception(Resources.MEMORY_ERROR);

            byte[] outc = new byte[csize];

            PdfDictionary dic2 = new PdfDictionary();

            Array.Copy(pk, 0, outc, 0, pk.Length);

            dic2.Put(PdfName.CONTENTS, new PdfString(outc).SetHexWriting(true));
            sap.Close(dic2);
        }

        public void Sign()
        {
            SignUsingEstEIDCard2(this.credentials, this.inputPDF, this.outputPDF);
        }
    }
}




