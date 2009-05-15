package ee.itp.dds.hibernate;

import java.util.List;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * @see ee.itp.dds.hibernate.Action
 * @author AV
 */

public class ActionDAO extends BaseHibernateDAO<Action,Long> {
  
  private static final Log log = LogFactory.getLog(ActionDAO.class);
  // property constants
  public static final String NAME = "name";
  public static final String VALID = "valid";
  
  
  public Action findActionByName(ActionEnum value) {
    List<Action> list = (List<Action>) find("from Action a");
    if (list != null ) 
      for (Action action : list) {
      if (action.getName().equals(value)) 
        return action;
      }
    Action a = new Action();
    a.setName(value);
    a.setValid(true);
    save(a);
    flush();
    return a;
  }
  
  
  
}