import java.io.*;
import java.security.cert.*;
import ee.sk.digidoc.*;
import ee.sk.utils.*;

import javax.servlet.*;
import javax.servlet.http.*;

public class SignServlet extends HttpServlet {
    String myPath;
    
    @Override
    public void init() throws ServletException {
        super.init();
        
        // Find out our filesystem path
        myPath = getServletContext().getRealPath("/").toString();
        log("working directory set to: " + myPath);
        
        // Load configuration files
        ConfigManager.init("jar://jdigidoc.cfg");
        ConfigManager.init(myPath + "/WEB-INF/jdigidoc.cfg");
    }
    
    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse res)
            throws ServletException, IOException {
        String mode = req.getParameter("mode");
        if(mode != null) {
                 if(mode.equals("prep")) doPrep(req, res);
            else if(mode.equals("sign")) doSign(req, res);
            else if(mode.equals("sdoc")) doSDoc(req, res);
        }
    }

    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse res)
            throws ServletException, IOException {
        doGet(req, res);
    }
    
    protected void doPrep(HttpServletRequest req, HttpServletResponse res)
            throws ServletException, IOException {
        String pem = req.getParameter("cert");
        if(pem != null) {
            String hash;
            
            res.setContentType("text/plain");
            try {
                Signature sig = prepareContract(pem);
                hash = SignedDoc.bin2hex(sig.calculateSignedInfoDigest());
                HttpSession sess = req.getSession(true);
                sess.setAttribute("sig", sig);
                res.getWriter().write(hash);
            } catch (Exception e) {
                log("Error Preparing document", e);
                res.setContentType("text/plain");
                res.getWriter().write("ERROR: " + e.getMessage());
            }
        } else {
            log("doPrep called without cert");
        }
    }
    
    protected void doSign(HttpServletRequest req, HttpServletResponse res)
            throws ServletException, IOException {
	boolean doOCSP = false;

        String sigHex = req.getParameter("sigHex");
        if(req.getParameter("doOCSP") != null) doOCSP = true;
        HttpSession sess = req.getSession(true);
        if(sigHex != null) {
            Signature sig = (Signature)sess.getAttribute("sig");
            if(sig != null) {
                res.setContentType( "text/plain" );
                try {
                    sig.setSignatureValue(SignedDoc.hex2bin(sigHex));
                    if(doOCSP) sig.getConfirmation();
                    SignedDoc sdoc = sig.getSignedDoc();
                    sdoc.writeToFile(new File(myPath + "/out/" + sess.getId() + ".bdoc"));
                    sess.removeAttribute("sig");
                    res.getWriter().write( "OK" );
                } catch(Exception e) {
                    log("Error Signing doc", e);
                    res.getWriter().write( "ERROR: " + e.getMessage() );
                }
            } else {
                res.getWriter().write("ERROR: No signing operation in progress");   
            }
        } else {
            log("doSign called without sigHex");
        }
        sess.invalidate();
    }
    
    protected void doSDoc(HttpServletRequest req, HttpServletResponse res)
            throws ServletException, IOException {
        HttpSession sess = req.getSession(true);
        Signature sig = (Signature)sess.getAttribute("sig");

        if(sig != null) {
            res.setContentType("application/octet-stream");
            res.setHeader("Content-Disposition", "attachment; filename=leping.bdoc");
            try {
                sig.getSignedDoc().writeToStream(res.getOutputStream());
            } catch(Exception e) {
                log("Error sending Doc to browser", e);
            }
        } else {
            res.setContentType("text/plain");
            res.getWriter().write("ERROR: No signing operation in progress");   
        }
    }
    
    private Signature prepareContract(String pem) throws DigiDocException, CertificateException {
        X509Certificate cert = parseCertificate(pem);
        SignedDoc sdoc = new SignedDoc(SignedDoc.FORMAT_BDOC, SignedDoc.BDOC_VERSION_1_0);
        sdoc.addDataFile(new File(myPath + "/leping.txt"),
                "text/plain", DataFile.CONTENT_EMBEDDED);
        return sdoc.prepareSignature(cert, null, null);
    }

    public X509Certificate parseCertificate(String pem) throws CertificateException {
        CertificateFactory cf = CertificateFactory.getInstance("X.509");
        return (X509Certificate)cf.generateCertificate(
                new ByteArrayInputStream(pem.getBytes()));
    }
}
