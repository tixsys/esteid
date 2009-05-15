/**
 * 
 */
package ee.itp.dds.core;

import ee.itp.dds.hibernate.Action;
import ee.itp.dds.hibernate.ActionEnum;
import ee.itp.dds.hibernate.Document;
import ee.itp.dds.hibernate.Session;

/**
 * @author Aleksandr Vassin
 * @created  11.04.2009
 */
public interface SessionHolderComponent {

    
  /**
   * find opened session by sessionCode
   * @param sessionCode
   * @return
   */
  public Session loadSession(String sessionCode);
  
  
  /**
   * Saves given session
   * 
   * @param session
   */
  public void saveSession(Session session);

  /**
   * Saves session with given action
   * 
   * @param session
   */
  public void saveSessionWithAction(Session session, ActionEnum action);
  
  
  /**
   * Create new session 
   * @param hostIp
   * @return
   */
  public Session createNewSession(String hostIp);

  /**
   * Adding DigiDoc document with files (need TODO)
   * @param session
   * @param file
   * @return
   */
  public void saveDocument(String sessioncode, Document digiDoc);
  
  
  
  /**
   * Load DigiDoc document with files (need TODO)
   * 
   * @param session
   * @return
   */
  public Document loadDocument(String sesscode);
  
  /**
   * Close before opened session 
   * @param sessionCode
   * @return true - session closed, false - can't find specified session
   */
  public boolean closeSession(String sessionCode);

}