package ee.itp.dds.hibernate;

import java.util.Date;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * Event DAO
 * @see ee.itp.dds.hibernate.Event
 * @author AV
 */

public class EventDAO extends BaseHibernateDAO<Event,Long> {
  private static final Log log = LogFactory.getLog(EventDAO.class);

  // property constants
  public static final String BLOB_VALID = "blobValid";
  public static final String COMMENT = "comment";
  public static final String BLOB = "blob";


  public Event saveEvent(Action action, Session session, String comment ) {
    Event event = new Event();
    event.setAction(action);
    event.setEventTime((new Date()).getTime());
    event.setSession(session);
    event.setComment(comment);
    save(event);
    return event;
  }
  
  
}