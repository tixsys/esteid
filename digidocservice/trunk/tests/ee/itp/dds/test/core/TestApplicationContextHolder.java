package ee.itp.dds.test.core;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.springframework.context.ApplicationContext;
import org.springframework.mock.web.MockServletContext;
import org.springframework.web.context.ContextLoader;

/**
 * Simple class to hold reference for application context in a static variable, usefull for tests So we can use the same application context for all tests, initializing it only
 * once.
 */
public class TestApplicationContextHolder {

    /** The Constant CONFIG_LOCATIONS. */
    private static final String CONFIG_LOCATIONS = "test-applicationContext.xml";

    /** The application context. */
    private static ApplicationContext applicationContext;

    /** The servlet context. */
    private static ServletContext servletContext;

    /**
     * Gets the application context.
     * 
     * @return the application context
     */
    public static ApplicationContext getApplicationContext() {
        return applicationContext;
    }

    /**
     * Checks if is initialized.
     * 
     * @return true, if is initialized
     */
    public static boolean isInitialized() {
        return applicationContext != null;
    }

    /**
     * Initialize context.
     * 
     * @throws ServletException
     *             the servlet exception
     */
    public static void initializeContext() throws ServletException {
        servletContext = new MockServletContext();
        ((MockServletContext) servletContext).addInitParameter(ContextLoader.CONFIG_LOCATION_PARAM, CONFIG_LOCATIONS);
        applicationContext = (new ContextLoader()).initWebApplicationContext(servletContext);
    }

    /**
     * Gets the servlet context.
     * 
     * @return the servlet context
     */
    public static ServletContext getServletContext() {
        return servletContext;
    }
}
