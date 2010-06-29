/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */


import ee.sk.digidoc.*;
import java.security.cert.*;
import ee.sk.utils.ConfigManager;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.util.Iterator;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;
import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

/**
 *
 * @author lauri
 */
public class BdocServlet extends HttpServlet {
    private static  Logger log= Logger.getLogger(BdocServlet.class.getName());

    // <editor-fold defaultstate="collapsed" desc="HttpServlet methods. Click on the + sign on the left to edit the code.">
    /** 
     * Handles the HTTP <code>GET</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doGet(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {
        sendDocumentStreamToBrowserClientForVerification(req, res);
    } 

    /** 
     * Handles the HTTP <code>POST</code> method.
     * @param request servlet request
     * @param response servlet response
     * @throws ServletException if a servlet-specific error occurs
     * @throws IOException if an I/O error occurs
     */
    @Override
    protected void doPost(HttpServletRequest req, HttpServletResponse res)
    throws ServletException, IOException {
        try {
            String mode = req.getParameter("mode");
            if(mode == null) {
                     uploadFilesAndCertToGenerateHash(req, res);                
            }
            else{
                  publishDigiDocument(req, res);
            }
            
        } catch (Exception ex) {           
            log.log(Level.SEVERE, null, ex);
            
        }
    }

    public void uploadFilesAndCertToGenerateHash(HttpServletRequest request, HttpServletResponse response) throws IOException
    {
         String jsonResult="";
        try {
            String ddocFileName = "";
            String cert = "";
            SignedDoc sdoc = new SignedDoc(SignedDoc.FORMAT_BDOC, SignedDoc.BDOC_VERSION_1_0);
            //parse request
            ServletFileUpload servletFileUpload = new ServletFileUpload(new DiskFileItemFactory());
            if (ServletFileUpload.isMultipartContent(request)) {
                List fileItemsList = servletFileUpload.parseRequest(request);
                Iterator it = fileItemsList.iterator();
                while (it.hasNext()) {
                    FileItem fileItem = (FileItem) it.next();
                    if (fileItem.isFormField()) {
                        // The file item contains a simple name-value pair of a form field
                        if (fileItem.getFieldName().equals("ddocName")) {
                            ddocFileName = fileItem.getString();
                        } else if (fileItem.getFieldName().equals("cert")) {
                            cert = fileItem.getString();
                        }
                    } else {
                        String itemName = fileItem.getName();
                        if (!"".equals(itemName)) {
                            File file = new File(myPath + "/docks/Files/" + itemName);
                            fileItem.write(file);
                            sdoc.addDataFile(file, "file", DataFile.CONTENT_EMBEDDED);
                        }
                    }
                }
                
                CertificateFactory cf = CertificateFactory.getInstance("X.509");
                X509Certificate certificate =(X509Certificate)cf.generateCertificate(new ByteArrayInputStream(Base64Util.decode(cert)));
                Signature signature = sdoc.prepareSignature(certificate, null, null);
                String hash = SignedDoc.bin2hex(signature.calculateSignedInfoDigest());
                HttpSession sess = request.getSession(true);
                sess.setAttribute("docName", ddocFileName);
                sess.setAttribute("sig", signature);//put signature into session
                 jsonResult="{'status':'OK','hash':'"+hash+"','doc':'"+ddocFileName+"'}";
               
            }
        }catch (Exception ex) {
            Logger.getLogger(BdocServlet.class.getName()).log(Level.SEVERE, null, ex);
            jsonResult="{'status':'ERROR','message':'"+ex.getMessage()+"'}";
        }
          response.setContentType("text/plain");
          response.getWriter().write(jsonResult);//send hash back to client plugin
    }

    public void sendDocumentStreamToBrowserClientForVerification(HttpServletRequest req, HttpServletResponse res) throws IOException
    {
        HttpSession sess = req.getSession(true);
        Signature sig = (Signature)sess.getAttribute("sig");

        if(sig != null) {
            res.setContentType("application/octet-stream");
            res.setHeader("Content-Disposition", "attachment; filename=temp.bdoc");
            try {
                sig.getSignedDoc().writeToStream(res.getOutputStream());
            } catch(Exception e) {
                log("Error sending Doc to browser", e);
            }
        } else {
            res.setContentType("text/plain");
            res.getWriter().write("ERROR: No signing operation in progress");
            res.getWriter().write("{'status':'ERROR','message':'No signing operation in progress'}");
        }
    }

    public void publishDigiDocument(HttpServletRequest req, HttpServletResponse res) throws IOException
    {
        String sigHex = req.getParameter("sigHex");
        String jsonResult="";
        HttpSession sess = req.getSession(true);
        String documentName=(String)req.getParameter("docName");
        if(sigHex != null) {
            Signature sig = (Signature)sess.getAttribute("sig");
            
            if(sig != null) {
                res.setContentType( "text/plain" );
                try {
                    sig.setSignatureValue(SignedDoc.hex2bin(sigHex));
                    //sig.getConfirmation();
                    SignedDoc sdoc = sig.getSignedDoc();
                    File file = new File(myPath + "/docks/" + documentName + ".bdoc");
                    if(file.exists())
                    {
                        file.delete();
                    }
                    sdoc.writeToFile(file);
                    jsonResult="{'status':'OK','doc':'"+documentName+"','message':'finished'}";
                } catch(Exception e) {
                    log("Error Signing doc", e);
                    jsonResult="{'status':'ERROR','doc':'"+documentName+"','message':'"+e.getMessage()+"'}";
                }
            } else {
                jsonResult="{'status':'ERROR','doc':'"+documentName+"','message':'No signing operation in progress'}";
            }
        } else {
            jsonResult="{'status':'ERROR','doc':'"+documentName+"','message':'doSign called without sigHex'}";
            log("doSign called without sigHex");
        }

        sess.invalidate();//clear the session
        res.setContentType("text/plain");
        res.getWriter().write(jsonResult);//send hash back to client plugin
    }

  
    /** 
     * Returns a short description of the servlet.
     * @return a String containing servlet description
     */
    @Override
    public String getServletInfo() {
        return "Short description";
    }// </editor-fold>

    String myPath;

    @Override
    public void init() throws ServletException {
        super.init();

        // Find out our filesystem path
        myPath = getServletContext().getRealPath("/").toString();
        log("working directory set to: " + myPath);
        
        // Load configuration file
        ConfigManager.init(myPath + "WEB-INF/jdigidoc.cfg");
    }
}
