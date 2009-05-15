package ee.itp.dds.test.core;

import java.io.IOException;
import java.lang.reflect.Method;
import java.util.EventListener;
import java.util.HashMap;

import javax.servlet.http.HttpServletResponse;

import junit.framework.TestCase;

import org.apache.axis.transport.http.AxisHTTPSessionListener;
import org.apache.axis.transport.http.AxisServlet;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.methods.GetMethod;
import org.mortbay.jetty.Server;
import org.mortbay.jetty.servlet.Context;
import org.mortbay.jetty.servlet.ServletHolder;
import org.springframework.web.context.ContextLoaderListener;

/**
 * 
 * @author Aleksandr Vassin
 * @created  21.04.2009
 */
public class JettyServerTestServer extends TestCase {
  
  protected final static int servicePort = 9999; 
  protected final static String serviceLocation = "http://localhost:"+servicePort+"/services/DigiDocService";
  private static int testNumber = 0; 
  protected static int testsCount = 0;
  private static  Server server;

  protected int getTestCount() {
      int cnt = 0;
      Class personClass = this.getClass();
      Method[] methods = personClass.getDeclaredMethods();
      for (Method method : methods)
          if (method.getName().startsWith("test"))
              cnt++;
      return cnt;
  }
  
  
  /* (non-Javadoc)
   * @see junit.framework.TestCase#setUp()
   */
  protected void setUp() throws Exception {
	    super.setUp();
	    testNumber++;
		if (server == null ) {
		    testNumber = 0;
		    server = new Server(servicePort);
		    Context root = new Context(server,"/",Context.SESSIONS);
		    root.setResourceBase("web/");
		    final HashMap<String, String> params = new HashMap<String, String>();
		    params.put("contextConfigLocation", "classpath*:test-applicationContext*.xml");
		    root.setInitParams(params);
		    root.setEventListeners(new EventListener[] {
		        new ContextLoaderListener(),
		        new AxisHTTPSessionListener()
		    });
		    root.addServlet(new ServletHolder(new AxisServlet()), "/services/*");
		    server.start();
		}
	  }

	  /* (non-Javadoc)
  	 * @see junit.framework.TestCase#tearDown()
  	 */
  	protected final void tearDown() throws Exception {
  	    if (testNumber >= testsCount + 1) { //+1 is DummyTest 
  	        server.stop();
  	        server = null;
  	    }
	    super.tearDown();
	  }
  
  /**
	 * Test dummy.
	 * 
	 * @throws IOException
	 *             Signals that an I/O exception has occurred.
	 */
  public void testJettyDummy() throws IOException {
    HttpClient client = new HttpClient();
  
    final GetMethod getMethod = new GetMethod(serviceLocation + "?wsdl");
    int result = client.executeMethod(getMethod);
    assertTrue(result == HttpServletResponse.SC_OK);
  }
  
}
