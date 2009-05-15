package ee.itp.dds.hibernate;

import java.io.Serializable;
import java.util.List;

/**
* 
* @author Aleksandr Vassin
*
* @param <T> The class of the pojo being persisted.
* @param <I> the class of the pojo's id property.
*/
public interface IBaseDAO<T,I extends Serializable> {
	
  
  public static final String SCHEMA_NAME = "ddservice";
  
  
    /**
     * Get the object with the id specified.
     * 
     * @param id the id of the instance to retrieve.
     * 
     * @return the instance with the given id.
     */
    T get(I id);
    
    /**
     * Get all instances that match the properties that are set in the given 
     * object using a standard Query by Example method.
     *  
     * @param t the example bean
     * 
     * @return a list of beans that match the example.
     */
    List<T> findByExample(T t);
    
    /**
     * Get all instances of this bean that have been persisted.
     * 
     * @return a list of all instances.
     */
    List<T> findAll();
    
    /**
     * Persist the given bean.
     * 
     * @param t the bean to persist.
     */
    void save(T t);
    
    /**
     * Save or Update the given bean.
     * 
     * @param t the bean to persist.
     */
    void saveOrUpdate(T t);    
    
    /**
     * Remove the bean with the given id.
     *
     * @param id the id of the bean to remove.
     */
    void delete(I id);
    
    /**
     * Remove the bean passed.  same as remove(t.<idProoertyGetter>())
     * 
     * @param t the object to remove.
     */
    void delete(T t);	

    /**
     * Get all instances property of that match the given value 
     *  
     * @param propertyName - property name
     * @param value - property value
     * @return a list of beans that matche.
     */
    List<T> findByProperty(String propertyName, Object value);
    
    /**
     * Get first instance property of that match the given value 
     *  
     * @param propertyName - property name
     * @param value - property value
     * @return a list of beans that matche.
     */    
    T findFirstByProperty(String propertyName, Object value);    
    
    /**
     * Execute query with parameters and returns list of found instances
     * @param queryString - query string
     * @param values - array of parameters
     * @return
     */
	List<T> find(String queryString, Object[] values);
	
    /**
     * Execute query with single parameter and returns list of found instances
     * @param queryString - query string
     * @param value - parameter
     * @return
     */
	List<T> find(String queryString, Object value);
	
    /**
     * Execute query returns list of found instances
     * @param queryString - query string
     * @return
     */
	List<T> find(String queryString);

    /**
     * Execute query with parameters and returns first found instance
     * @param queryString - query string
     * @param values - array of parameters
     * @return
     */
	T findFirst(String queryString, Object[] values);

    /**
     * Execute query with single parameter and returns first found instances
     * @param queryString - query string
     * @param value - parameter
     * @return
     */
	T findFirst(String queryString, Object value);

    /**
     * Execute query returns first found instances
     * @param queryString - query string
     * @return
     */	
	T findFirst(String queryString);
    
	
    /**
     * Get count of instances
     * @return
     */	
	int count();
	
}