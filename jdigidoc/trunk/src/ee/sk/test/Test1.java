/*
 * Test1.java
 * PROJECT: JDigiDoc
 * DESCRIPTION: Test programm for JDigiDoc library 
 * AUTHOR:  Veiko Sinivee, S|E|B IT Partner Estonia
 *==================================================
 * Copyright (C) AS Sertifitseerimiskeskus
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * GNU Lesser General Public Licence is available at
 * http://www.gnu.org/copyleft/lesser.html
 *==================================================
 */

package ee.sk.test;
import ee.sk.digidoc.*;
import java.io.*;
import java.security.cert.X509Certificate;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.*;
import java.util.*;

/**
 * Test creating a SignedDoc, adding some data files
 * and storing to a file
 * @author  Veiko Sinivee
 * @version 1.0
 */
public class Test1 
{
    //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    public static String file1 = "\\work\\MyWorkspace\\JDigiDoc_Integreeritud\\doc\\SKXMLPlugin.Readme.txt";
    public static String mime1 = "text/text";
    public static String file2 = "\\work\\MyWorkspace\\JDigiDoc_Integreeritud\\doc\\JDigiDoc-2.0-eng.pdf";
    public static String mime2 = "application/pdf";
    /*public static String file3 = "C:\\veiko\\work\\sk\\JDigiDoc\\input.txt";
    public static String mime3 = "text/txt";*/
//A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    public static String ddoc_file = "\\work\\MyWorkspace\\JDigiDoc_Integreeritud\\data\\test1.ddoc";
    public static String zip_file = "\\work\\MyWorkspace\\JDigiDoc_Integreeritud\\data\\test1.bdoc";
    public static String zip_file2 = "\\work\\MyWorkspace\\JDigiDoc_Integreeritud\\data\\testb2.bdoc";
    /*
    public static String file1 = "input3.xml";
    public static String mime1 = "text/xml";
    public static String file2 = "j-strutstiles.pdf";
    public static String mime2 = "application/pdf";
    public static String file3 = "input.txt";
    public static String mime3 = "text/txt";
    public static String ddoc_file = "test4.ddoc";
    */
    //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    public static String pin = "51440";
    //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
    public static String myBody = "Eesti Vabariigi p�hiseadus P�hilehek�lg | M��rangud | Otsing Eesti Vabariigi p�hiseadus rahvah��letuse seadus nr 1";
    
    public static void main(String[] args) 
    {
        SignedDoc sdoc = null;
        try {
            //ConfigManager.init("jar://JDigiDoc.cfg");
            //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            //ConfigManager.init("/home/veiko/workspace/JDigiDoc/jdigidoc.cfg");
            ConfigManager.init("\\work\\MyWorkspace\\JDigiDoc_Integreeritud\\JDigiDoc-sample.cfg");
            // create a new SignedDoc 
            sdoc = new SignedDoc(SignedDoc.FORMAT_BDOC, SignedDoc.BDOC_VERSION_1_0);
            DataFile df = sdoc.addDataFile(new File(file1), mime1, DataFile.CONTENT_EMBEDDED);
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            //df.setInitialCodepage("ISO-8859-1");
            // now set this Datafiles body.
            // It will not read it later if you do this per hand!!!
            
            //byte[] u8b = ConvertUtils.str2data(myBody);
            
            //System.out.println("UTF8 body: " + new String(u8b));
            
            //df.setBody(u8b, "ISO-8859-1");
            
            //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            DataFile df2 = sdoc.addDataFile(new File(file2), mime2, DataFile.CONTENT_EMBEDDED);
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            //df.setBody(myBody.getBytes("ISO-8859-1"), "ISO-8859-1");
            // add a detatched file
            //df = sdoc.addDataFile(new File(file2), mime2, DataFile.CONTENT_DETATCHED);
            // add an embedded base64 file
            //df = sdoc.addDataFile(new File(file3), mime3, DataFile.CONTENT_EMBEDDED_BASE64);
            
            // Do card login and get certificate
            SignatureFactory sigFac = ConfigManager.
                instance().getSignatureFactory();
            System.out.println("GET Cert");
            X509Certificate cert = sigFac.getCertificate(0, pin);
            
            // add a Signature
            System.out.println("Prepare signature");
            //String[] roles = { "K�kerdaja" };
            String[] roles = new String[0];
            SignatureProductionPlace adr = new SignatureProductionPlace("Tallinn",
                "Harjumaa - ����", "Eesti Vabariik", "12345");
            Signature sig = sdoc.prepareSignature(cert, roles, adr);
            byte[] sidigest = sig.calculateSignedInfoDigest();
            
            byte[] sigval = sigFac.sign(sidigest, 0, pin);
            System.out.println("Finalize signature");
            sig.setSignatureValue(sigval);
             
            // get confirmation
            System.out.println("Get confirmation");
            sig.getConfirmation();
            System.out.println("Confirmation OK!");
            //System.out.println("Signature: " + sig);
             
            // write it in a file
            //A Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            //System.out.println("Writing in file: " + ddoc_file);
            sdoc.writeToFile(new File(zip_file));  
            
            DigiDocFactory digFac = ConfigManager.
                instance().getDigiDocFactory();
            sdoc = digFac.readSignedDoc(zip_file);
            //System.out.println("GOT: " + sdoc.toXML());
            //L Inga <2008 aprill> BDOCiga seotud muudatused xml-is 1
            // verify signature
            //Signature sig = null;
            for(int i = 0; i < sdoc.countSignatures(); i++) {
                sig = sdoc.getSignature(i);
                System.out.println("Signature: " + sig.getId() + " - " +
                    sig.getKeyInfo().getSubjectLastName() + "," +
                    sig.getKeyInfo().getSubjectFirstName() + "," +
                    sig.getKeyInfo().getSubjectPersonalCode());
                //System.out.println("SigInfo: " + sig.getSignedInfo().toString());
                ArrayList errs = sig.verify(sdoc, false, false);
                if(errs.size() == 0)
                    System.out.println("OK");
                for(int j = 0; j < errs.size(); j++) 
                    System.out.println((DigiDocException)errs.get(i));                
                System.out.println("");
            }
             
            
        } catch(DigiDocException ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
        } catch(Exception ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
        }
    }
}
