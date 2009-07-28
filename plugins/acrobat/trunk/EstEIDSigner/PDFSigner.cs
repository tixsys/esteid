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
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;

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
using Org.BouncyCastle.Utilities;

using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography.Pkcs;
using System.Security.Cryptography;
using System.Windows.Forms;
using System.Resources;
using System.Reflection;

using EstEIDNative;

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


    /// <summary>
    /// This is a holder class for PDF signature appearance
    /// </summary>
    class Appearance
    {
        private string reason;
        private string location;
        private string contact;
        private bool visible;

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
    /// base class for resource string    
    /// </summary>
    class ResourceBase
    {
        protected ResourceManager resManager = null;

        public ResourceBase()
        {
            resManager = new ResourceManager("EstEIDSigner.Properties.Resources",
                Assembly.GetExecutingAssembly());
        }
    }

    /// <summary>
    /// this is the most important class
    /// it uses iTextSharp library to sign PDF document
    /// </summary>
    class PDFSigner : ResourceBase
    {
        public const uint SIGNATURE_LENGTH = 128;
        private string inputPDF = string.Empty;
        private string outputPDF = string.Empty;
        private Timestamp stamp = null;
        private MetaData metadata = null;
        private Appearance appearance = null;        

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
        public PDFSigner(string input, string output, Timestamp tmstamp, MetaData md, Appearance app)
        {
            this.inputPDF = input;
            this.outputPDF = output;
            this.stamp = tmstamp;
            this.metadata = md;
            this.appearance = app;
        }

        public void Verify()
        {
        }        

        private string ReadPin(string label, int maxpin)
        {
            FormPin form = new FormPin(label, maxpin);
            DialogResult dr = form.ShowDialog();
            if (dr == DialogResult.OK)
            {
                return form.Pin;
            }

            return (string.Empty);
        }

        private byte[] EstEIDCardSign(EstEIDReader estEidReader, PKCS11Signer signer, Byte[] digest)
        {
            uint rc, slot;
            const uint rsa_length = SIGNATURE_LENGTH;
            uint digest_length = rsa_length;
            string pin = string.Empty;
            TokenInfo info = null;

            HGlobalSafeHandle raw = new HGlobalSafeHandle((int)rsa_length);
            IntPtr rsaBytes = raw;
            if (rsaBytes == IntPtr.Zero)
                throw new OutOfMemoryException(resManager.GetString("OUT_OF_MEMORY"));

            info = signer.Info;
            slot = signer.Slot;

            if (info.LoginRequired && info.PinIsSet)
            {
                string label = info.Label;
                pin = ReadPin(label, (int)info.MinPin);

                // no pin supplied ?
                if (pin == string.Empty)
                    throw new Exception(resManager.GetString("NO_PIN_SUPPLIED"));
            }

            rc = estEidReader.Sign(slot, pin, digest, (uint)digest.Length, ref rsaBytes, ref digest_length);
            // failure ?
            if (rc != EstEIDReader.ESTEID_OK)
                throw new PKCS11Exception(rc);                

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
                throw new Exception(resManager.GetString("CARD_HASH_INIT"));

            Stream s = sap.RangeStream;
            MemoryStream ss = new MemoryStream();
            int read = 0;
            byte[] buff = new byte[8192];
            while ((read = s.Read(buff, 0, 8192)) > 0)
            {
                ss.Write(buff, 0, read);
                rc = hash.UpdateDigest(buff, (uint)read);
                if (rc != EstEIDReader.ESTEID_OK)
                    throw new Exception(resManager.GetString("CARD_HASH_UPDATE"));
            }

            HGlobalSafeHandle raw = new HGlobalSafeHandle((int)hash_length);
            IntPtr hashBytes = raw;
            if (hashBytes == IntPtr.Zero)
                throw new OutOfMemoryException(resManager.GetString("OUT_OF_MEMORY"));

            rc = hash.FinalizeDigest(ref hashBytes, ref digest_length);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(resManager.GetString("CARD_HASH_FINALIZE"));            

            return raw.ToByteArray();
        }

        private byte[] ComputeHash(EstEIDReader estEidReader, byte[] data)
        {
            uint rc, digest_length = 0;
            const uint hash_length = Digest.SHA1_LENGTH;

            Digest hash = new Digest(estEidReader);

            rc = hash.InitDigest(Digest.HashAlgorithm.SHA1);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(resManager.GetString("CARD_HASH_INIT"));

            rc = hash.UpdateDigest(data, (uint)data.Length);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(resManager.GetString("CARD_HASH_UPDATE"));

            HGlobalSafeHandle raw = new HGlobalSafeHandle((int)hash_length);
            IntPtr hashBytes = raw;
            if (hashBytes == IntPtr.Zero)
                throw new OutOfMemoryException(resManager.GetString("OUT_OF_MEMORY"));

            rc = hash.FinalizeDigest(ref hashBytes, ref digest_length);
            if (rc != EstEIDReader.ESTEID_OK)
                throw new Exception(resManager.GetString("CARD_HASH_FINALIZE"));

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
                    throw new Exception(resManager.GetString("TSA_ERROR") + error);

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
            uint flags;

            slots = estEidReader.GetSlotCount(1);
            if (slots == 0)
                throw new Exception(resManager.GetString("CARD_MISSING"));

            col = new X509Certificate2Collection();
            mech = new Mechanism(Mechanism.CKF_SIGN|Mechanism.CKF_HW);
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
                
                TokenInfo info = new TokenInfo(estEidReader);
                rc = info.ReadInfo(i);
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

                signer = new PKCS11Signer(i, cert, info);
                signers.Add(signer);
            }

            // no valid certs found ?
            if (col.Count == 0)
                return null;
   
            X509Certificate2Collection sel = X509CertificateUI.SelectFromCollection(col,
                "Sertifikaadid", "Vali sertifikaat digitaalallkirja lisamiseks");

            if (sel == null)
                throw new Exception(resManager.GetString("CERT_MISSING"));                     

            if (sel.Count > 0)
            {
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
            }

            return null;
        }

        public void SignUsingEstEIDCard(string filename, string outfile)
        {
            // open EstEID
            EstEIDReader estEidReader = new EstEIDReader();
            bool b = estEidReader.Open("opensc-pkcs11.dll");
            if (b == false)
                throw new Exception(resManager.GetString("PKCS11_OPEN"));

            PKCS11Signer signer = LocateSigner(estEidReader);
            if (signer == null)
                throw new Exception(resManager.GetString("CERT_MISSING"));

            X509Certificate2 card = signer.Cert;
            Oid oid = card.SignatureAlgorithm;

            if (oid.Value != PkcsObjectIdentifiers.Sha1WithRsaEncryption.Id)
                throw new Exception(resManager.GetString("INVALID_CERT"));

            Org.BouncyCastle.X509.X509CertificateParser cp = new Org.BouncyCastle.X509.X509CertificateParser();
            Org.BouncyCastle.X509.X509Certificate[] chain = new Org.BouncyCastle.X509.X509Certificate[] { cp.ReadCertificate(card.RawData) };

            PdfReader reader = new PdfReader(filename);
            PdfStamper stp = PdfStamper.CreateSignature(reader, new FileStream(outfile, FileMode.Create), '\0');
            if (metadata != null)
                stp.XmpMetadata = metadata.getStreamedMetaData();
            PdfSignatureAppearance sap = stp.SignatureAppearance;
            if (appearance.Visible)
                sap.SetVisibleSignature(new Rectangle(100, 100, 300, 200), 1, null);
            sap.SignDate = DateTime.Now;
            sap.SetCrypto(null, chain, null, null);
            sap.Reason = appearance.Reason;
            sap.Location = appearance.Location;
            sap.Contact = appearance.Contact;
            sap.Acro6Layers = true;
            sap.Render = PdfSignatureAppearance.SignatureRender.NameAndDescription;
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

            // sign hash
            byte[] rsadata = EstEIDCardSign(estEidReader, signer, digest);
            if (rsadata == null)
                throw new Exception(resManager.GetString("CARD_INTERNAL_ERROR"));

            // create PKCS#7 object
            PdfPKCS7 pk7 = new PdfPKCS7(null, chain, null, "SHA1", true);
            pk7.SetExternalDigest(rsadata, digest, "RSA");
            byte[] pk = pk7.GetEncodedPKCS7();

            // user wants to add TSA response ?
            if (stamp != null && pk != null)
                pk = TimestampAuthorityResponse(estEidReader, pk);

            // PKCS#7 bytes too large ?
            if (pk.Length >= csize)
                throw new Exception(resManager.GetString("MEMORY_ERROR"));

            byte[] outc = new byte[csize];

            PdfDictionary dic2 = new PdfDictionary();

            Array.Copy(pk, 0, outc, 0, pk.Length);

            dic2.Put(PdfName.CONTENTS, new PdfString(outc).SetHexWriting(true));
            sap.Close(dic2);
        }

        PdfDictionary ParseSigantureBytes(string filename)
        {
            PdfReader reader = new PdfReader(filename);
            ArrayList ar = reader.AcroFields.GetSignatureNames();
            PdfDictionary dic = reader.AcroFields.GetSignatureDictionary(ar[0].ToString());
            AcroFields af = reader.AcroFields;
            foreach (string strSigName in af.GetSignatureNames())
            {
                PdfPKCS7 pk = af.VerifySignature(strSigName);
            }

            return dic;
        }           

        public void Sign()
        {
            //PdfDictionary dic1 = ParseSigantureBytes("E:\\test_notenabled_ms.pdf");
            //PdfDictionary dic2 = ParseSigantureBytes("E:\\test_notenabled_mm.pdf");
            SignUsingEstEIDCard(this.inputPDF, this.outputPDF);
        }
    }
}




