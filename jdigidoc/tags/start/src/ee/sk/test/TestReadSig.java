/*
 * TestReadSig.java
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
import java.io.File;
import java.io.InputStream;

import ee.sk.digidoc.*;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.ConfigManager;
import java.io.*;


public class TestReadSig {
	public static String config_file = "./jdigidoc.cfg";
    
    
    
    public static void main(String[] args) 
    {
        String sigfile = null, ddocfile = null, outfile = null;
        if(args != null && args.length == 3) {
        	ddocfile = args[0];
        	sigfile = args[1];
        	outfile = args[2];
        } else {
        	System.err.println("USAGE: <digidoc-file> <signature-file> <output-file>");
        	return;
        }
        try {
            ConfigManager.init(config_file);
            
            // read from file taht contains only DataFile       
            System.out.println("Reading file" + ddocfile);
            DigiDocFactory digFac = ConfigManager.
        	instance().getDigiDocFactory();
            SignedDoc sdoc = digFac.readSignedDoc(ddocfile);
        
            
            // read in signature
            System.out.println("Reading signature" + sigfile);
            sdoc.readSignature(new FileInputStream(sigfile));
            // write to a new file
            System.out.println("Writing digidoc" + outfile);
            sdoc.writeToFile(new File(outfile));  
            
            System.out.println("Done!");            
        } catch(DigiDocException ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
        } catch(Exception ex) {
            System.err.println(ex);
            ex.printStackTrace(System.err);
        }
    }
}
