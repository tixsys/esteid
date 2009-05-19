#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import string
import cStringIO
import logging
import logging.handlers
import unittest
import doctest
import tempfile
import warnings

from subprocess import *

#Create conf file
def createTempConf(bdoc_lib_home_path, cert_store_path="/etc/certs"):
    conf_template = string.Template(
"""<?xml version="1.0" encoding="UTF-8"?>
<!--Auto generated configuration for bdoc lib-->
<configuration xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:noNamespaceSchemaLocation="${bdoc_lib_home}/etc/schema/conf.xsd">
    <!--algorithm to use for digest calculation when creating new Bdoc
        see http://www.ietf.org/rfc/rfc4051.txt-->
    <!--http://www.w3.org/2000/09/xmldsig#sha1-->
    <!--http://www.w3.org/2001/04/xmlenc#sha256-->
    <param name="digest.uri">http://www.w3.org/2000/09/xmldsig#sha1</param>
    <!--pkcs11 driver location-->
    <param name="pkcs11.driver.path">/usr/lib/opensc-pkcs11.so</param>
    <!-- OCSP responder url. Used for validating signing certificates and generating BDoc-TM signatures-->
    <param name="ocsp.url">http://www.openxades.org/cgi-bin/ocsp.cgi</param>
    <!--Trusted CA certs in PEM format-->
    <param name="cert.store.path">${bdoc_lib_home}${cert_store}</param>
    <!--OCSP responder public certificate for validating OCSP response-->
    <param name="ocsp.certs.file">${bdoc_lib_home}/etc/certs/sk-test-ocsp-responder-2005.pem</param>

    <param name="manifest.xsd.path">file:///${bdoc_lib_home}/etc/schema/OpenDocument_manifest.xsd</param>
    <param name="xades.xsd.path">file:///${bdoc_lib_home}/etc/schema/XAdES.xsd</param>
    <param name="dsig.xsd.path">file:///${bdoc_lib_home}/etc/schema/xmldsig-core-schema.xsd</param>
</configuration>
"""
)
    conf_str = conf_template.substitute(bdoc_lib_home=bdoc_lib_home_path, cert_store=cert_store_path)
    
    
    f, conf_name = tempfile.mkstemp(suffix='.conf') 
    conf_file = os.fdopen(f, 'w')
    conf_file.write(conf_str)
    conf_file.close()
    #print "Generated configuration file:", conf_name
    return conf_name

#run cmd and return cmd output
#@cmd list command and its arguments
#return output from cmd
def execCmd(cmd):
    #logging.debug("execCmd("+str(cmd)+")")
    #e = {}
    #e['BDOCLIB_CONF_XML'] = projectDir+'etc/bdoclib.conf'
    #e['LD_LIBRARY_PATH']  = projectDir+'/build/dist'
    #print g_environment['BDOCLIB_CONF_XML']
    global g_environment
    p = Popen(cmd, stdout=PIPE, stderr=STDOUT, universal_newlines=True, env=g_environment)
    command = ""
    for s in cmd:
        command += " "+s

    logging.info("Executing: "+command)
    output = p.communicate()[0]
    logging.info(output)
    retcode = p.returncode
    if retcode < 0:
        raise CalledProcessError(retcode, command)
    return output

class BDocTestCase(unittest.TestCase):

        
    def validateSignature(self, bdocFile, validOutputMustContain, validateOnline=False):
        global g_projectDir
        global g_tool
        projectDir = g_projectDir
        if (bdocFile[0] != '/' and bdocFile[0] != '\\'): #relative to test/data/bdoc/
            fullBdocPath = projectDir+'test/data/bdoc/'+bdocFile
        else: #full
            fullBdocPath = bdocFile
        cmd = [g_tool, '-open', fullBdocPath, '-valid_offline', '1']
        if(validateOnline):
            cmd[len(cmd):] = ['-valid_online', '1']
        output = execCmd(cmd)
        logging.info("Executing: "+str(cmd))
        logging.info(output)
        r = output.rfind(validOutputMustContain)
        errmsg = ""
        start = output.rfind("ERROR ")
        if (start > 0):
            errmsg =  output[start:]
        self.assert_( r > 0, "Validation of "+bdocFile+" failed "+errmsg )
        
    def validateOffline(self, bdocFile):
        self.validateSignature(bdocFile, "Signature 1 validated offline", validateOnline=False)

    def validateOnline(self,bdocFile):
        self.validateSignature(bdocFile, "Signature 1 validated online", validateOnline=True)

    def testValidateTM(self):
        """validateOffline  BDocTM_r441.bdoc"""
        bdocFile = ' BDocTM_r441.bdoc'
        self.validateOffline(bdocFile)
    
    def testValidateTM(self):
        """validateOffline BDocTM_RSASigner_r441.bdoc"""
        bdocFile = 'BDocTM_RSASigner_r441.bdoc'
        self.validateOffline(bdocFile) 
        
        

    def testParseInvalidXMLFailsGracefully(self):
        """Test that parsing invalid signature file fails gracefully - invalid/java_lib_BDocTM_0_7.bdoc"""
        bdocFile = 'invalid/java_lib_BDocTM_0_7.bdoc'
        self.validateSignature(bdocFile, " Failed to parse signature", validateOnline=False)
    
    def testInvalidIssuerMustFail(self):
        """Test that invalid issuer fails - invalid/java_lib_BDocTM_1_0_invalid_issuer.bdoc"""
        bdocFile = 'invalid/java_lib_BDocTM_1_0_invalid_issuer.bdoc'
        self.validateSignature(bdocFile, "Signing certificate issuer information does not match", validateOnline=False)
    
    def testUnimplementedTS(self):
        """testUnimplementedTS - validateOffline JavaLibBdoc java_lib_BDocTS_1_0.bdoc"""
        bdocFile = 'invalid/java_lib_BDocTS_1.0.bdoc'
        #self.validateOffline(bdocFile)
        self.validateSignature(bdocFile, "Unknown signature media type 'signature/bdoc-1.0/TS'", validateOnline=False)
        
    def testValidateBES(self):
        """validateOffline BDocBES_r423.bdoc"""
        bdocFile = 'BDocBES_r423.bdoc'
        self.validateOffline(bdocFile)
        
    def testValidateSHA256(self):
        """validateOffline SHA256  BDocTM_RSASigner_r441.bdoc"""
        bdocFile = 'BDocTM_RSASigner_r441.bdoc'
        self.validateOffline(bdocFile)
    
    #Check that unsupported digest doesn't cause crash
    def testUnknownSignatureMethodMustFail(self):
        """fail gracefully on unknown signature method rsasha1 - bdocTM_SHA512.bdoc"""
        bdocFile = 'invalid/bdocTM_SHA512.bdoc'
        self.validateSignature(bdocFile, 'Unsupported SignedInfo signature method "http://www.w3.org/2000/09/xmldsig#rsasha1"', validateOnline=False)
        
    def testValidateOnline(self):
        """validateOnline BDocTM_SHA256_r441.bdoc"""
        bdocFile = 'BDocTM_SHA256_r441.bdoc'
        self.validateOnline(bdocFile)
        
    
            
    def checkUnknownCACertNotFoundFails(self, bdoc):
        """Check that signature with unknown CA is invalid"""
        global g_environment
        global g_projectDir
        conf = g_environment['BDOCLIB_CONF_XML']
        #set invalid cert store
        tempConf = createTempConf(g_projectDir[0:-1], cert_store_path='/test/data/bdoc')
        g_environment['BDOCLIB_CONF_XML'] = tempConf
        try:
            self.validateSignature(bdoc, "Signature is invalid",validateOnline=False)
        finally:
            os.remove(tempConf)
            g_environment['BDOCLIB_CONF_XML'] = conf

    def testUnknownCACertNotFoundBESFails(self):
        """Check that signature with unknown CA is invalid - BDocTM_SHA256_r441.bdoc"""
        bdocFile = 'BDocTM_SHA256_r441.bdoc'
        self.checkUnknownCACertNotFoundFails(bdocFile)
        
    def testUnknownCACertNotFoundTMFails(self):
        """Check that signature with unknown CA is invalid - BDocTM_SHA256_r441.bdoc"""
        bdocFile = 'BDocTM_SHA256_r441.bdoc'
        self.checkUnknownCACertNotFoundFails(bdocFile)
    
    def testInvalidOCSPRefValueMustFail(self):
        """Check that TM with invalid OCSPRefValue fails - invalid/BDocTM_invalid_OCSPRefValue_r423.bdoc"""
        bdocFile = 'invalid/BDocTM_invalid_OCSPRefValue_r423.bdoc'
        self.validateSignature(bdocFile, "OCSPRef value doesn't match with hash of OCSP response", validateOnline=False)

    def createValidateBDoc(self, profile='TM', cmd=[]):
        """Test sign bdoc"""
        bdocFile = os.tmpnam()+ '.bdoc'
        #os.remove(bdocFile)
        global g_tool
        global g_projectDir
        tool = g_tool
        # ./tool -create -insert build.properties -sign test/data/cert/cert+priv_key.pem BES Tallinn Harjumaa 11213 Estonia tester -save_as out.bdoc
        if len(cmd) == 0: #no command parameter given
            cmd = [tool, 
                '-create', 
                '-insert', tool, #the file to be inserted, in this context
                '-sign', g_projectDir+'test/data/cert/cert+priv_key.pem',
                profile, 'Tallinn', 'Harjumaa', '13417', 'Estonia', 'automated_test',
                '-save_as', bdocFile 
                ]
        output = execCmd(cmd)
        logging.info("Executing: "+str(cmd))
        logging.info(output)
        r = output.rfind("Signed BDoc.")
        errmsg = ""
        start = output.rfind("ERROR ")
        if (start > 0):
            errmsg =  output[start:]
        self.assert_( r > 0, "Signing of "+bdocFile+" failed "+errmsg )
        
        self.validateOffline(cmd[-1:][0]) #the output file

    def testValidateLocaleSpecificChars(self):
	""" Test sign bdoc with non-ascii characters included in the xml data.
        Note: files to be inserted can have non-ascii characters in the filename
        too but this is not covered here in the automated tests yet. """
        global g_tool
        global g_projectDir
        tool = g_tool
        bdocFile = os.tmpnam()+ '.bdoc'
        bdocFile += 'õõääööüü'
        toolcmd = [tool,
                '-create',
                '-insert', g_projectDir+'test/testdoc.odt',
                '-sign', g_projectDir+'test/data/cert/cert+priv_key.pem',
                'BES', 'Tällinn', 'Härjumaa', '13417', 'Estünia', 'ütomated_test',
                '-save_as', bdocFile
                ]
        self.createValidateBDoc('BES', toolcmd)
        self.createValidateBDoc('TM', toolcmd)

    def testCreateBDocBES(self):
        """Create Bdoc BES and validate it offline"""
        self.createValidateBDoc(profile='BES')
    
    def testCreateBDocTM(self):
        """Create Bdoc TM and validate it offline"""
        self.createValidateBDoc(profile='TM')
        
    def testUnitTests(self):
        """Run unit tests"""
        global g_projectDir
        global g_unittests
        unittests_cmd = [g_unittests, g_projectDir+'test/data']
        output = execCmd(unittests_cmd)
        logging.info("Executing: "+str(unittests_cmd))
        logging.info(output)
        r = output.rfind("OK")
        self.assert_( r > 0, "Some unittests failed" )
    



#logging.basicConfig(level=logging.DEBUG,
#                    filename='test.log',
#                    format='%(asctime)s %(levelname)-8s \n%(message)s')
logging.basicConfig(level=logging.DEBUG, filename='test.log',format='%(asctime)s TEST: \n%(message)s\n')

#find bdoc lib home directory
g_projectDir = os.getcwd()+"/"
if g_projectDir.rfind('test/') == (len(g_projectDir) - len('test/')):
    g_projectDir = g_projectDir[0: len(g_projectDir) - len('test/')]
g_projectDir = g_projectDir.replace("\\","/")
print "Bdoc lib home:", g_projectDir


#set execution environment
g_environment = os.environ
g_environment['LD_LIBRARY_PATH']  = g_projectDir+'build/dist'
#g_environment['BDOCLIB_CONF_XML'] = projectDir+'etc/bdoclib.conf'
if 'BDOCLIB_CONF_XML' in os.environ:
    g_environment['BDOCLIB_CONF_XML'] = os.environ['BDOCLIB_CONF_XML']
else:     
    g_environment['BDOCLIB_CONF_XML'] = createTempConf(g_projectDir[0:-1])


print "Using Conf:", g_environment['BDOCLIB_CONF_XML']

#g_tool = g_projectDir+'build/dist/tool'
#g_demo = g_projectDir+'build/dist/demo'
#unittests = g_projectDir+'build/dist/unittests'


if os.name == 'nt': #WIN32
    exe_suffix = '.exe'
else:
    exe_suffix = ''    

if os.path.exists(g_projectDir+'build/dist/tool'+exe_suffix):
    g_tool = g_projectDir+'build/dist/tool'+exe_suffix
elif os.path.exists(g_projectDir+'src/digidoc-tool'+exe_suffix):
    g_tool = g_projectDir+'src/digidoc-tool'+exe_suffix
else:
    print "Can't find digidoc-tool"+exe_suffix    

#if os.path.exists(g_projectDir+'build/dist/unittests'+exe_suffix):
#    g_unittests = g_projectDir+'build/dist/unittests'+exe_suffix
if os.path.exists(g_projectDir+'test/src'+exe_suffix):
    g_unittests = g_projectDir+'test/src'+exe_suffix
else:
    print "Can't find unittests"+exe_suffix    


#g_demo = g_projectDir+'src/digidoc-demo'



logging.info("Using '"+g_environment['BDOCLIB_CONF_XML']+"' as configuration")
f = open(g_environment['BDOCLIB_CONF_XML'])
logging.info(""+g_environment['BDOCLIB_CONF_XML']+":"+f.read())
f.close()

#suppress RuntimeWarning that is caused by os.tmpnam()
warnings.simplefilter('ignore',  RuntimeWarning)

if __name__ == '__main__':
    suite = unittest.TestSuite()
    suite.addTest(unittest.makeSuite(BDocTestCase))
    #suite.addTest(BDocTestCase('testCreateBDocBES'))
    #suite.addTest(BDocTestCase('testValidateOnline'))
    #suite.addTest(BDocTestCase('testInvalidIssuerMustFail'))
    #suite.addTest(BDocTestCase('testParseInvalidXMLFailsGracefully'))
    #suite.addTest(BDocTestCase('testCreateBDocTM'))
    #suite.addTest(BDocTestCase('testUnimplementedTS'))
    #suite.addTest(BDocTestCase('testValidateTM'))
    #suite.addTest(BDocTestCase('testUnknownCACertNotFoundTMFails'))
    #suite.addTest(BDocTestCase('testValidateSHA256'))
    #suite.addTest(BDocTestCase('testUnknownSignatureMethodMustFail'))
    #suite.addTest(BDocTestCase('testValidateLocaleSpecificChars'))
    unittest.TextTestRunner(verbosity=2,descriptions=1).run(suite)


