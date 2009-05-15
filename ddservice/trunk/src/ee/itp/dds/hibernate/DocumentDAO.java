package ee.itp.dds.hibernate;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

/**
 * 
 * @author Konstantin Tarletsky
 * @created  11.04.2009
 */
public class DocumentDAO extends BaseHibernateDAO<Document, Long> {
  
  private static final Log log = LogFactory.getLog(DocumentDAO.class);
  // property constants
  public static final String ID = "id";

}
