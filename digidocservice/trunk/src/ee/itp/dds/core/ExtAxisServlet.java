package ee.itp.dds.core;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.axis.transport.http.AxisServlet;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class ExtAxisServlet extends AxisServlet {
    
    private static final Log log = LogFactory.getLog(ExtAxisServlet.class);

    public void init() throws ServletException
    {
      String contextConfigLocation = (String) getServletContext().getInitParameter("contextConfigLocation");
      ReceivedDataHolder.setContextConfigLocation(contextConfigLocation);
      super.init();
    }

    public void destroy()
    {
      super.destroy(); // Just puts "destroy" string in log
    }

    public void doGet( HttpServletRequest request, HttpServletResponse response ) throws ServletException, IOException
    {
      super.doGet(request, response);
    }


    public void doPost( HttpServletRequest request, HttpServletResponse response ) throws ServletException, IOException
    {
      super.doPost(request, response);
    }
    

}
