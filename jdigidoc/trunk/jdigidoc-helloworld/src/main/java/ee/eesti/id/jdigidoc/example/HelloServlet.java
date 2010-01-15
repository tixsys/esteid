package ee.eesti.id.jdigidoc.example;

import java.io.*;
import java.security.cert.*;
import java.util.ArrayList;
import ee.sk.digidoc.*;
import ee.sk.digidoc.factory.*;
import ee.sk.utils.*;

import javax.servlet.*;
import javax.servlet.http.*;

public class HelloServlet extends HttpServlet {
    String myPath;

    @Override    
    public void init() throws ServletException {
        super.init();
        
        // Find out our filesystem path
        myPath = getServletContext().getRealPath("/").toString();
        log("webapp working directory is: " + myPath);
        
        // Load configuration file
        //ConfigManager.init(myPath + "/jdigidoc.cfg");
        ConfigManager.init("jar://jdigidoc.cfg");
    }
    
    @Override    
    protected void doGet(HttpServletRequest req, HttpServletResponse res)
            throws ServletException, IOException {

        PrintWriter out = res.getWriter();

        try {
            String ddocfile = "hello.ddoc";

            out.println("Reading container file " + ddocfile);
            DigiDocFactory digFac = ConfigManager.
                instance().getDigiDocFactory();
            SignedDoc sdoc = digFac.readSignedDoc(myPath + "/" + ddocfile);

            /* List data files */
            for(int i = 0; i < sdoc.countDataFiles(); i++) {
                DataFile df = sdoc.getDataFile(i);
                out.println("DataFile: " + df.getId() + " - " +
                    df.getFileName());
            }

            /* List and verify signatures */
            for(int i = 0; i < sdoc.countSignatures(); i++) {
                Signature sig = sdoc.getSignature(i);
                out.print("Signature: " + sig.getId() + " - " +
                    sig.getKeyInfo().getSubjectLastName() + "," +
                    sig.getKeyInfo().getSubjectFirstName() + "," +
                    sig.getKeyInfo().getSubjectPersonalCode());
                ArrayList errs = sig.verify(sdoc, false, false);
                if(errs.size() == 0)
                    out.println(" - Valid");
                for(int j = 0; j < errs.size(); j++) 
                    out.println((DigiDocException)errs.get(i));
                out.println("");
            }

        } catch(Exception ex) {
            ex.printStackTrace(out);
        }

        out.close();
    }
}
