package ee.itp.dds.hibernate;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.hibernate.HibernateException;
import org.hibernate.cfg.Configuration;
import org.springframework.orm.hibernate3.HibernateTemplate;

/**
 * Configures and provides access to Hibernate sessions, tied to the current thread of execution. 
 * Follows the Thread Local Session pattern, see {@link http://hibernate.org/42.html }.
 */
public class HibernateSessionFactory {

  private static final Log log = LogFactory.getLog(HibernateSessionFactory.class);    
    
  private static final String CONFIG_FILE_LOCATION = "/hibernate.cfg.xml";
  private static final ThreadLocal<HibernateTemplate> threadLocal = new ThreadLocal<HibernateTemplate>();
  private static Configuration configuration = new Configuration();
  private static org.hibernate.SessionFactory sessionFactory;
  private static String configFile = CONFIG_FILE_LOCATION;

  static {
    try {
      configuration.configure(configFile);
      sessionFactory = configuration.buildSessionFactory();
    } catch (Exception e) {
      log.error("%%%% Error Creating SessionFactory %%%%");
    }
  }

  private HibernateSessionFactory() {
  }

  /**
   * Returns the ThreadLocal HibernateTemplate instance. Lazy initialize the <code>SessionFactory</code> if needed.
   * 
   * @return HibernateTemplate
   * @throws HibernateException
   */
  public static HibernateTemplate getHibernate() throws HibernateException {
    HibernateTemplate hibernate = (HibernateTemplate) threadLocal.get();
    if (hibernate == null ) {
      if (sessionFactory == null) {
        rebuildSessionFactory();
      }
      hibernate = (sessionFactory != null) ? new HibernateTemplate(sessionFactory) : null;
      threadLocal.set(hibernate);
    }
    return hibernate;
  }

  /**
   * Rebuild hibernate session factory
   * 
   */
  public static void rebuildSessionFactory() {
    try {
      configuration.configure(configFile);
      sessionFactory = configuration.buildSessionFactory();
    } catch (Exception e) {
      log.error("%%%% Error Creating SessionFactory %%%%");
    }
  }

  /**
   * Close the single HibernateTemplate instance.
   * 
   * @throws HibernateException
   */
  public static void closeSession() throws HibernateException {
    HibernateTemplate hibernate = (HibernateTemplate) threadLocal.get();
    threadLocal.set(null);
    if (hibernate != null) {
      hibernate.flush();
      hibernate = null;
    }
  }

  /**
   * return session factory
   * 
   */
  public static org.hibernate.SessionFactory getSessionFactory() {
    return sessionFactory;
  }

  /**
   * return session factory
   * 
   * session factory will be rebuilded in the next call
   */
  public static void setConfigFile(String configFile) {
    HibernateSessionFactory.configFile = configFile;
    sessionFactory = null;
  }

  /**
   * return hibernate configuration
   * 
   */
  public static Configuration getConfiguration() {
    return configuration;
  }

}