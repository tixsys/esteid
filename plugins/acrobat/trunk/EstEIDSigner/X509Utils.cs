﻿using System;
using System.Net;
using System.IO;

using Org.BouncyCastle.X509;
using Org.BouncyCastle.Tsp;
using Org.BouncyCastle.Math;
using Org.BouncyCastle.Asn1.X509;
using Org.BouncyCastle.Asn1.Cmp;


namespace EstEIDSigner
{
    class X509Utils
    {
        public const String ID_TIME_STAMP_TOKEN = "1.2.840.113549.1.9.16.2.14"; // RFC 3161 id-aa-timeStampToken 

        static public byte[] GetTimestampToken(String tsaURL, string tsaUserName, string tsaPassword, byte[] imprint, ref string error)
        {
            TimeStampRequestGenerator tsqGenerator = new TimeStampRequestGenerator();

            tsqGenerator.SetCertReq(true);

            tsqGenerator.SetReqPolicy("1.3.6.1.4.1.601.10.3.1");

            BigInteger nonce = BigInteger.ValueOf(DateTime.Now.Ticks);

            TimeStampRequest request = tsqGenerator.Generate(X509ObjectIdentifiers.IdSha1.Id, imprint, nonce);

            byte[] requestBytes = request.GetEncoded();

            byte[] responseBytes = GetTSAResponse(tsaURL, tsaUserName, tsaPassword, requestBytes);

            TimeStampResponse response = new TimeStampResponse(responseBytes);

            response.Validate(request);


            PkiFailureInfo failure = response.GetFailInfo();

            int value = (failure == null) ? 0 : failure.IntValue;

            if (value != 0)
            {
                error = "TSA '" + tsaURL + "' veakood " + value;                
                return (null);
            }

            TimeStampToken tsToken = response.TimeStampToken;

            if (tsToken == null)
            {
                error = "TSA '" + tsaURL + "' vastuse lugemine ebaõnnestus";                
                return (null);
            }
          
            return tsToken.GetEncoded();
        }
        
        static protected byte[] GetTSAResponse(String tsaURL, string tsaUserName, string tsaPassword, byte[] requestBytes)
        {
            Uri uri = new Uri(tsaURL);
            HttpWebRequest request = (HttpWebRequest)WebRequest.Create(uri);
            request.ContentType = "application/timestamp-query";
            request.ContentLength = requestBytes.Length;

            request.Method = "POST";

            if ((tsaUserName != null) && (tsaUserName != ""))
            {

                String userPassword = tsaUserName + ":" + tsaPassword;
                CredentialCache credCache = new CredentialCache();

                credCache.Add(new Uri(tsaURL), "Basic",
                          new NetworkCredential(tsaUserName, tsaPassword, tsaURL));
                request.Credentials = credCache;
            }

            Stream requestStream = request.GetRequestStream();

            requestStream.Write(requestBytes, 0, requestBytes.Length);

            requestStream.Close();

            WebResponse response = request.GetResponse();

            Stream resStream = response.GetResponseStream();

            MemoryStream ms = new MemoryStream();

            byte[] responseBytes;
            byte[] buffer = new byte[4096];

            using (MemoryStream memoryStream = new MemoryStream())
            {
                int count = 0;
                do
                {
                    count = resStream.Read(buffer, 0, buffer.Length);
                    memoryStream.Write(buffer, 0, count);

                } while (count != 0);

                responseBytes = memoryStream.ToArray();

            }

            response.Close();

            return responseBytes;
        }
    }
}