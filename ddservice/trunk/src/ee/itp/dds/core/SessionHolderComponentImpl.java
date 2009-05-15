/**
 * 
 */
package ee.itp.dds.core;

import java.util.Date;

import javax.swing.plaf.metal.MetalBorders.Flush3DBorder;

import ee.itp.dds.hibernate.Action;
import ee.itp.dds.hibernate.ActionDAO;
import ee.itp.dds.hibernate.ActionEnum;
import ee.itp.dds.hibernate.Document;
import ee.itp.dds.hibernate.DocumentDAO;
import ee.itp.dds.hibernate.Event;
import ee.itp.dds.hibernate.EventDAO;
import ee.itp.dds.hibernate.Session;
import ee.itp.dds.hibernate.SessionDAO;
import ee.itp.dds.util.DateUtils;

/**
 * @author Aleksandr Vassin
 * 
 */
public class SessionHolderComponentImpl implements Component, SessionHolderComponent {

    public static final int SESSION_LIVE_TIME_MINUTES = 30;

    private SessionDAO sessionDAO;
    private ActionDAO actionDAO;
    private EventDAO eventDAO;
    private DocumentDAO documentDAO;

    /**
     * Creates new Session
     */
    public SessionHolderComponentImpl() {

    }

    /*
     * (non-Javadoc)
     * 
     * @see ee.itp.dds.core.SessionHolderComponent#loadSession(java.lang.String)
     */
    public Session loadSession(String sessionCode) {
        Session session = (Session) sessionDAO.findFirstByProperty(SessionDAO.CODE, sessionCode);

        if (session != null && session.getAccessDate() != null) {

            Date currentTime = new Date();
            Date sessionExpiredTime = new Date(session.getAccessDate());
            sessionExpiredTime = DateUtils.addMinutes(SESSION_LIVE_TIME_MINUTES, sessionExpiredTime);

            for (Event theEvent : session.getEvents()) {

                Action theAction = theEvent.getAction();
                if (theAction != null) {

                    if (theAction.getName().name().equals(ActionEnum.SESSION_CLOSED.name())) {

                        session = null;
                        break;

                    }// if

                }// if

            }// for

            if (session != null && currentTime.after(sessionExpiredTime)) {

                closeSession(sessionCode);
                session = null;

            }// if

        }// if

        return session;
    }// loadSession

    public void saveSession(Session session) {

        documentDAO.saveOrUpdate(session.getDocument());
        session.setAccessDate(new Date().getTime());
        sessionDAO.saveOrUpdate(session);

    }// saveSession

    /*
     * (non-Javadoc)
     * 
     * @see ee.itp.dds.core.SessionHolderComponent#createNewSession(java.lang.String)
     */
    public Session createNewSession(String hostIp) {
        Session session = sessionDAO.newSession(hostIp);
        Action action = actionDAO.findActionByName(ActionEnum.SESSION_CREATED);
        eventDAO.saveEvent(action, session, "createNewSession()");
        return session;
    }

    /*
     * (non-Javadoc)
     * 
     * @see ee.itp.dds.core.SessionHolderComponent#createNewSession(java.lang.String)
     */
    public boolean closeSession(String code) {

        Session session = loadSession(code);
        if (session == null)
            return false;

        Action action = actionDAO.findActionByName(ActionEnum.SESSION_CLOSED);
        Event closeEvent = eventDAO.saveEvent(action, session, "closeSession()");
        session.setCloseDate((new Date()).getTime());
        sessionDAO.saveOrUpdate(session);
        return true;
    }// closeSession

    /*
     * (non-Javadoc)
     * 
     * @see ee.itp.dds.core.SessionHolderComponent#addDataFile(ee.itp.dds.hibernate.Session, byte[])
     */
    public void saveDocument(String sessionCode, Document digiDoc) {

        long time = (new Date()).getTime();

        if (sessionCode != null && !sessionCode.equals("")) {

            Session session = loadSession(sessionCode);
            session.setAccessDate(time);
            Action action = actionDAO.findActionByName(ActionEnum.ADD_DATA_FILE);
            Event ev = new Event();
            ev.setAction(action);
            ev.setSession(session);
            ev.setEventTime(time);
            eventDAO.save(ev);
            documentDAO.saveOrUpdate(digiDoc);

        }// if

    }// saveDocument

    public void saveSessionWithAction(Session session, ActionEnum action) {
        long time = (new Date()).getTime();
        session.setAccessDate(time);
        Event ev = new Event();
        Action act = actionDAO.findActionByName(action);
        ev.setAction(act);
        ev.setSession(session);
        ev.setEventTime(time);
        eventDAO.save(ev);
        sessionDAO.saveOrUpdate(session);
    }
    
    
    
    public Document loadDocument(String sessionCode) {

        Document ret = null;
        long time = (new Date()).getTime();

        if (sessionCode != null && !sessionCode.equals("")) {

            Session session = loadSession(sessionCode);
            session.setAccessDate(time);
            Action action = actionDAO.findActionByName(ActionEnum.LOAD_DATA_FILE);
            Event ev = new Event();
            ev.setAction(action);
            ev.setSession(session);
            ev.setEventTime(time);
            eventDAO.save(ev);

        }// if
        return ret;
    }// loadDocument

    public SessionDAO getSessionDAO() {
        return sessionDAO;
    }

    public void setSessionDAO(SessionDAO sessionDAO) {
        this.sessionDAO = sessionDAO;
    }

    public Session getSession() {
        return null;// session;
    }

    public EventDAO getEventDAO() {
        return eventDAO;
    }

    public void setEventDAO(EventDAO eventDAO) {
        this.eventDAO = eventDAO;
    }

    public ActionDAO getActionDAO() {
        return actionDAO;
    }

    public void setActionDAO(ActionDAO actionDAO) {
        this.actionDAO = actionDAO;
    }

    public DocumentDAO getDocumentDAO() {
        return documentDAO;
    }

    public void setDocumentDAO(DocumentDAO documentDAO) {
        this.documentDAO = documentDAO;
    }


}
