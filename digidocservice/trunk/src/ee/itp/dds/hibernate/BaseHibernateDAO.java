package ee.itp.dds.hibernate;

import java.io.Serializable;
import java.lang.reflect.ParameterizedType;
import java.util.List;

import org.hibernate.HibernateException;
import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.criterion.Example;
import org.hibernate.criterion.MatchMode;
import org.springframework.orm.hibernate3.HibernateCallback;
import org.springframework.orm.hibernate3.HibernateTemplate;

/**
 * Basic hibernate DAO object 
 * @author Aleksandr Vassin
 */
public abstract class BaseHibernateDAO<T,I extends Serializable> implements IBaseDAO<T,I> {

  
    /**
     * The class of the pojo being persisted.
     */
    private Class<? extends T> clazz;
    
    protected BaseHibernateDAO(Class<? extends T> clazz) {
        this.clazz = clazz;
    }

    protected HibernateTemplate getHibernate() {
      return HibernateSessionFactory.getHibernate();
    }
    
    @SuppressWarnings("unchecked")
  	protected BaseHibernateDAO() {
          this.clazz  = (Class<T>) ((ParameterizedType) getClass().getGenericSuperclass()).getActualTypeArguments()[0];
    }

    /**
     * @return the clazz
     */
    protected Class<? extends T> getClazz() {
        return clazz;
    }

    /**
     * @param clazz the clazz to set
     */
    protected void setClazz(Class<? extends T> clazz) {
        this.clazz = clazz;
    }

    /**
     * @see spotex.hibernate.IBaseDAO#get(java.io.Serializable)
     */
    @SuppressWarnings("unchecked")
    public T get(I id) {
        return (T) getHibernate().load(clazz, id);
    }

    /**
     * @see spotex.hibernate.IBaseDAO#get(java.lang.Object)
     */
    @SuppressWarnings("unchecked")
    public List<T> findByExample(final T t) {
        if (t == null) {
            return findAll();
        } else {
            // filter on properties set in the customer
            HibernateCallback callback = new HibernateCallback() {
                public Object doInHibernate(Session session) throws HibernateException {
                    Example ex = Example.create(t).ignoreCase().enableLike(MatchMode.ANYWHERE);
                    return session.createCriteria(clazz).add(ex).list();
                }
            };
            return (List<T>) getHibernate().execute(callback);
        }
    }

    /**
     * @see spotex.hibernate.IBaseDAO#get()
     */
    @SuppressWarnings("unchecked")
    public List<T> findAll() {
        return getHibernate().loadAll(clazz);
    }

    /**
     * @see spotex.hibernate.IBaseDAO#remove(java.io.Serializable)
     */
    public void delete(I id) {
        delete(get(id));
    }

    /**
     * @see spotex.hibernate.IBaseDAO#remove(java.lang.Object)
     */
    public void delete(T t) {
        getHibernate().delete(t);
    }

    /**
     * @see spotex.hibernate.IBaseDAO#save(java.lang.Object)
     */
    public void save(T t) {
        getHibernate().save(t);
    }

    public void flush() {
        getHibernate().flush();
    }    
    
    
    /**
     * @see spotex.hibernate.IBaseDAO#saveOrUpdate(java.lang.Object)
     */
    public void saveOrUpdate(T t) {
        getHibernate().saveOrUpdate(t);
    }    

	public List<T> findByProperty(String propertyName, Object value) {
		try {
			String queryString = "from " + this.clazz.getName() + " as model where model." + propertyName + "= ?";
			return (List<T>) getHibernate().find(queryString , new Object[]{value});
		} catch (RuntimeException re) {
			throw re;
		}
	}
	
	public T findFirstByProperty(String propertyName, Object value) {
		try {
			String queryString = "from " + this.clazz.getName() +" as model where model." + propertyName + "= ?";
			List<T> list = (List<T>) getHibernate().find(queryString , new Object[]{value});
			if (list.isEmpty()) return null;
			return (T) list.get(0);
		} catch (RuntimeException re) {
			throw re;
		}
	}	
	
	public List<T> find(String queryString, Object[] values) {
		try {
			return (List<T>) getHibernate().find(queryString , new Object[]{values});
		} catch (RuntimeException re) {
			throw re;
		}
	}
	
	public List<T> find(String queryString, Object value) {
		return find(queryString,new Object[] {value});
	}	
	
	public List<T> find(String queryString) {
		return (List<T>) getHibernate().find(queryString);
	}	

	public T findFirst(String queryString, Object[] values) {
		List list = find(queryString,values);
		if (list.isEmpty()) return null;
		return (T) list.get(0); 
	}

	public T findFirst(String queryString, Object value) {
		List list = find(queryString,value);
		if (list.isEmpty()) return null;
		return (T) list.get(0); 
	}
	
	public T findFirst(String queryString) {
		List list = find(queryString);
		if (list.isEmpty()) return null;
		return (T) list.get(0); 
	}
	
    public int count() {
        List list = getHibernate().find("select count(*) from " + this.clazz.getName() + " x");
        Integer count = (Integer) list.get(0);
        return count.intValue();
    }
	
}