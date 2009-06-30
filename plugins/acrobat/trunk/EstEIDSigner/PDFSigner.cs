using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

using iTextSharp;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.X509;
using System.Collections;
using Org.BouncyCastle.Pkcs;
using iTextSharp.text.pdf;
using System.IO;
using iTextSharp.text.xml.xmp;

using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography.Pkcs;
using System.Security.Cryptography;
using iTextSharp.text;

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
        private string password;

        public Timestamp()
        {
        }
        public Timestamp(string url)
        {
            this.url = url;
        }
        public Timestamp(string url, string cpassword)
        {
            this.url = url;
            this.password = cpassword;
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
    /// this is the most important class
    /// it uses iTextSharp library to sign a PDF document
    /// </summary>
    class PDFSigner
    {
        private string inputPDF = "";
        private string outputPDF = "";
        private Timestamp stamp;
        private MetaData metadata;
        private Appearance appearance;
        private static byte[] digest;
        private static byte[] rsadata;

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

        public static X509Certificate2 GetCertificate()
        {
            X509Store st = new X509Store(StoreName.My, StoreLocation.CurrentUser);
            st.Open(OpenFlags.ReadOnly);
            X509Certificate2Collection col = st.Certificates;
            X509Certificate2 card = null;
            X509Certificate2Collection sel = X509Certificate2UI.SelectFromCollection(col, "Certificates", "Select one to sign", X509SelectionFlag.SingleSelection);
            if (sel.Count > 0)
            {
                X509Certificate2Enumerator en = sel.GetEnumerator();
                en.MoveNext();
                card = en.Current;
            }
            st.Close();
            return card;
        }

        static public byte[] SignMsg(Byte[] msg, X509Certificate2 signerCert, bool detached)
        {
            //  Place message in a ContentInfo object.
            //  This is required to build a SignedCms object.
            ContentInfo contentInfo = new ContentInfo(msg);

            //  Instantiate SignedCms object with the ContentInfo above.
            //  Has default SubjectIdentifierType IssuerAndSerialNumber.
            SignedCms signedCms = new SignedCms(contentInfo, detached);

            //  Formulate a CmsSigner object for the signer.
            CmsSigner cmsSigner = new CmsSigner(signerCert);

            // Include the following line if the top certificate in the
            // smartcard is not in the trusted list.
            cmsSigner.IncludeOption = X509IncludeOption.EndCertOnly;

            //  Sign the CMS/PKCS #7 message. The second argument is
            //  needed to ask for the pin.
            signedCms.ComputeSignature(cmsSigner, false);

            //  Encode the CMS/PKCS #7 message.
            return signedCms.Encode();
        }

        public void SignHashed(string filename, string outfile)
        {
            X509Certificate2 card = GetCertificate();
            if (card == null)
                throw new ArgumentNullException("Missing certificate");
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
            PdfSignature dic = new PdfSignature(PdfName.ADOBE_PPKMS, PdfName.ADBE_PKCS7_SHA1);
            dic.Date = new PdfDate(sap.SignDate);
            dic.Name = PdfPKCS7.GetSubjectFields(chain[0]).GetField("CN");
            if (sap.Reason != null)
                dic.Reason = sap.Reason;
            if (sap.Location != null)
                dic.Location = sap.Location;
            if (sap.Contact != null)
                dic.Contact = sap.Contact;
            sap.CryptoDictionary = dic;
            int csize = 1536;
            Hashtable exc = new Hashtable();
            exc[PdfName.CONTENTS] = csize * 2 + 2;
            sap.PreClose(exc);

            HashAlgorithm sha = new SHA1CryptoServiceProvider();

            Stream s = sap.RangeStream;
            int read = 0;
            byte[] buff = new byte[8192];
            while ((read = s.Read(buff, 0, 8192)) > 0)
            {
                sha.TransformBlock(buff, 0, read, buff, 0);
            }
            sha.TransformFinalBlock(buff, 0, 0);
            byte[] pk = SignMsg(sha.Hash, card, false);

            byte[] outc = new byte[csize];

            PdfDictionary dic2 = new PdfDictionary();

            Array.Copy(pk, 0, outc, 0, pk.Length);

            dic2.Put(PdfName.CONTENTS, new PdfString(outc).SetHexWriting(true));
            sap.Close(dic2);
        }

        public void SignDetached(string filename, string outfile)
        {
            X509Certificate2 card = GetCertificate();
            if (card == null)
                throw new ArgumentNullException("Missing certificate");
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
            PdfSignature dic = new PdfSignature(PdfName.ADOBE_PPKLITE, PdfName.ADBE_PKCS7_DETACHED);
            dic.Date = new PdfDate(sap.SignDate);
            dic.Name = PdfPKCS7.GetSubjectFields(chain[0]).GetField("CN");
            if (sap.Reason != null)
                dic.Reason = sap.Reason;
            if (sap.Location != null)
                dic.Location = sap.Location;
            if (sap.Contact != null)
                dic.Contact = sap.Contact;
            sap.CryptoDictionary = dic;
            int csize = 1536;
            Hashtable exc = new Hashtable();
            exc[PdfName.CONTENTS] = csize * 2 + 2;
            sap.PreClose(exc);

            Stream s = sap.RangeStream;
            MemoryStream ss = new MemoryStream();
            int read = 0;
            byte[] buff = new byte[8192];
            while ((read = s.Read(buff, 0, 8192)) > 0)
            {
                ss.Write(buff, 0, read);
            }
            byte[] pk = SignMsg(ss.ToArray(), card, true);

            byte[] outc = new byte[csize];

            PdfDictionary dic2 = new PdfDictionary();

            Array.Copy(pk, 0, outc, 0, pk.Length);

            dic2.Put(PdfName.CONTENTS, new PdfString(outc).SetHexWriting(true));
            sap.Close(dic2);
        }

        private string ReadPin()
        {
            return "";
        }               

        public bool EstEIDCardSign(Byte[] msg)
        {
            EstEIDReader reader = new EstEIDReader();
            bool b = reader.Open("opensc-pkcs11.dll");

            if (b != false)
            {
                uint slots = reader.GetSlotCount(1);
                uint i;

                for (i = 0; i < slots; i++)
                {
                    // use first slot
                    b = reader.IsMechanismSupported(i, Mechanisms.CKF_SIGN);
                    if (b)
                    {       
                        uint hashLength = 20;
                        uint rsaLength = 128;
                        IntPtr hashBytes = Marshal.AllocHGlobal((int)hashLength);
                        IntPtr rsaBytes = Marshal.AllocHGlobal((int)rsaLength);
                        reader.Sign(i, ReadPin(), msg, (uint)msg.Length,
                            ref hashBytes, ref hashLength,
                            ref rsaBytes, ref rsaLength);
                        reader.Close();

                        digest = new byte[hashLength];
                        rsadata = new byte[rsaLength];
                        Marshal.Copy(hashBytes, digest, 0, digest.Length);
                        Marshal.Copy(rsaBytes, rsadata, 0, rsadata.Length);
                        Marshal.FreeHGlobal(hashBytes);
                        Marshal.FreeHGlobal(rsaBytes);                        
                        return true;
                    }
                }
                reader.Close();
                return (digest != null) && (rsadata != null);
            }

            return (false);
        }               

        public void SignUsingEstEIDCard(string filename, string outfile)
        {
            X509Certificate2 card = GetCertificate();
            if (card == null)
                throw new ArgumentNullException("Missing certificate");

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
            //PdfSignature dic = new PdfSignature(PdfName.ADOBE_PPKLITE, PdfName.ADBE_PKCS7_DETACHED);           
            dic.Date = new PdfDate(sap.SignDate);
            dic.Name = PdfPKCS7.GetSubjectFields(chain[0]).GetField("CN");
            if (sap.Reason != null)
                dic.Reason = sap.Reason;
            if (sap.Location != null)
                dic.Location = sap.Location;
            if (sap.Contact != null)
                dic.Contact = sap.Contact;
            sap.CryptoDictionary = dic;            
            //sap.SetExternalDigest(new byte[20], new byte[128], "RSA");
            int csize = 1536;
            Hashtable exc = new Hashtable();
            exc[PdfName.CONTENTS] = csize * 2 + 2;
            sap.PreClose(exc);

            Stream s = sap.RangeStream;
            MemoryStream ss = new MemoryStream();
            int read = 0;
            byte[] buff = new byte[8192];
            while ((read = s.Read(buff, 0, 8192)) > 0)
            {
                ss.Write(buff, 0, read);
            }

            bool success = EstEIDCardSign(ss.ToArray());
            if (success == false)
                throw new ArgumentNullException("Failed to sign using SmartCard");

            PdfPKCS7 pk7 = new PdfPKCS7(null, chain, null, "SHA1", true);
            pk7.SetExternalDigest(digest, rsadata, "RSA");
            byte[] pk = pk7.GetEncodedPKCS7();            

            byte[] outc = new byte[csize];

            PdfDictionary dic2 = new PdfDictionary();

            Array.Copy(pk, 0, outc, 0, pk.Length);

            dic2.Put(PdfName.CONTENTS, new PdfString(outc).SetHexWriting(true));
            sap.Close(dic2);
        }

        public void Sign()
        {            
            //SignHashed(this.inputPDF, this.outputPDF);
            //SignDetached(this.inputPDF, this.outputPDF);            
            SignUsingEstEIDCard(this.inputPDF, this.outputPDF);
        }
    }
}




