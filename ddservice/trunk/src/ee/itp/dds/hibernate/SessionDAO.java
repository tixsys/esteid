package ee.itp.dds.hibernate;

import java.util.Date;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import ee.itp.dds.util.UuidUtil;

/**
 * 
 * @see ee.itp.dds.hibernate.Session
 * @author AV
 */

public class SessionDAO extends BaseHibernateDAO<Session,Long> {
  private static final Log log = LogFactory.getLog(SessionDAO.class);
  // property constants
  public static final String CODE = "code";
  public static final String HOST = "host";
  public static final String CREATE_DATE = "create_date"; 
  public static final String ACCESS_DATE = "access_date"; 
  public static final String CLOSE_DATE = "close_date"; 
  
  
  public Session newSession(String host) {
    Session session = new Session();
    session.setCode("" + UuidUtil.generateIntUuid());
    session.setCreateDate((new Date()).getTime());
    session.setHost(host);
    save(session);
    return session;
  }
  
  
}