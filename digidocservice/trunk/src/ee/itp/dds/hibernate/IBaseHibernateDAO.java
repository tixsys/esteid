package ee.itp.dds.hibernate;

import org.hibernate.Session;


/**
 * Data access interface for domain model
 * @author Aleksandr Vassin
 */
public interface IBaseHibernateDAO {
	public Session getSession();
}