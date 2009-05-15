package ee.itp.dds.test.core;

import java.io.IOException;
import java.io.InputStream;
import java.sql.Connection;

import javax.sql.DataSource;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.dbunit.DatabaseTestCase;
import org.dbunit.database.DatabaseConfig;
import org.dbunit.database.DatabaseConnection;
import org.dbunit.database.IDatabaseConnection;
import org.dbunit.dataset.DataSetException;
import org.dbunit.dataset.ReplacementDataSet;
import org.dbunit.dataset.xml.FlatXmlDataSet;
import org.springframework.context.ApplicationContext;
import org.springframework.jdbc.core.JdbcTemplate;
import org.springframework.jdbc.datasource.DataSourceUtils;
import org.springframework.orm.hibernate3.HibernateTemplate;
import org.springframework.orm.hibernate3.LocalSessionFactoryBean;
import org.springframework.transaction.support.TransactionTemplate;

/**
 * The Class DatabaseAwareTestCase.
 * 
 */
public abstract class DatabaseAwareTestCase extends DatabaseTestCase {
  
  /** The LOG. */
  protected static Log LOG = LogFactory.getLog(DatabaseAwareTestCase.class);

  /* (non-Javadoc)
   * @see org.dbunit.DatabaseTestCase#getConnection()
   */
  protected IDatabaseConnection getConnection() throws Exception {
    final DataSource dataSource = (DataSource)TestApplicationContextHolder.getApplicationContext().getBean("dataSource");
    String[] sqls = getInitializingSQLs();
    if (sqls != null && sqls.length > 0) {
      final JdbcTemplate template = new JdbcTemplate(dataSource);
      for (String sql : sqls) {
        template.execute(sql);
      }
    }
    Connection jdbcConnection = DataSourceUtils.getConnection(dataSource);
    final DatabaseConnection connection = new DatabaseConnection(jdbcConnection);
    DatabaseConfig config = connection.getConfig();
    config.setProperty(DatabaseConfig.PROPERTY_DATATYPE_FACTORY, new HsqlDataTypeFactory());
    config.setFeature(DatabaseConfig.FEATURE_QUALIFIED_TABLE_NAMES, true);
    return connection;
  }

  /**
	 * Override if you want some SQLs to be executed before the tests (as
	 * initializing code) Good example is definition of functions.
	 * 
	 * @return an array of Strings containing valid SQL statements
	 */
  protected String[] getInitializingSQLs() {
    return new String[0];
  }

  /**
	 * Creates the transaction template.
	 * 
	 * @return the transaction template
	 */
  protected TransactionTemplate createTransactionTemplate() {
    return (TransactionTemplate)TestApplicationContextHolder.getApplicationContext().getBean("transactionTemplate");
  }

  /**
	 * Convenience method that could be used in {@link #getDataSet()}
	 * implementation. Will create the {@link
	 * org.dbunit.dataset.ReplacementDataSet} and add the replacement object
	 * [NULL] to it.
	 * 
	 * @param datasetFile
	 *            the name of the file (in classpath) containing FlatXmlDataSet
	 * 
	 * @return a dataset built from the file, already containing the
	 *         replacementObject [NULL]
	 * 
	 * @throws java.io.IOException
	 *             if file not found or ome other IO error
	 * @throws org.dbunit.dataset.DataSetException
	 *             if data for some reason could not be inserted
	 * @throws IOException
	 *             Signals that an I/O exception has occurred.
	 * @throws DataSetException
	 *             the data set exception
	 */
  protected ReplacementDataSet getReplacementDataset(String datasetFile) throws IOException, DataSetException {
    InputStream stream = getClass().getClassLoader().getResourceAsStream(datasetFile);
    ReplacementDataSet dataSet = new ReplacementDataSet(new FlatXmlDataSet(stream));
    dataSet.addReplacementObject("[NULL]", null);
    return dataSet;
  }

  /* (non-Javadoc)
   * @see org.dbunit.DatabaseTestCase#setUp()
   */
  protected final void setUp() throws Exception {
    beforeSetUp();
    if (!TestApplicationContextHolder.isInitialized()) {
      TestApplicationContextHolder.initializeContext();
    }
    else {
      getSessionFactory().createDatabaseSchema();
      ((HibernateTemplate)TestApplicationContextHolder.getApplicationContext().getBean("hibernateTemplate")).clear();
    }

    LOG.info("Inserting test data...");
    super.setUp();
    LOG.info("Inserted.");
    afterSetUp();
  }

  /**
	 * Gets the bean.
	 * 
	 * @param id
	 *            the id
	 * 
	 * @return the bean
	 */
  @SuppressWarnings("unchecked")
  protected <T> T getBean(final String id) {
    return (T)getContext().getBean(id);
  }

  /**
	 * Gets the hibernate template.
	 * 
	 * @return the hibernate template
	 */
  protected HibernateTemplate getHibernateTemplate() {
    return getBean("hibernateTemplate");
  }

  /**
	 * Gets the session factory.
	 * 
	 * @return the session factory
	 */
  private LocalSessionFactoryBean getSessionFactory() {
    return (LocalSessionFactoryBean)TestApplicationContextHolder.getApplicationContext().getBean("&sessionFactory");
  }

  /* (non-Javadoc)
   * @see org.dbunit.DatabaseTestCase#tearDown()
   */
  protected final void tearDown() throws Exception {
    beforeTearDown();
    getSessionFactory().dropDatabaseSchema();
    super.tearDown();
    afterTearDown();
  }

  /**
	 * Gets the context.
	 * 
	 * @return the context
	 */
  public ApplicationContext getContext() {
    if (!TestApplicationContextHolder.isInitialized()) {
      throw new IllegalStateException("Context is null [not initialized]. Probably you have overrided setUp method without calling to super.setUp() or application context initialization has resulted in an error. See logs for details");
    }
    return TestApplicationContextHolder.getApplicationContext();
  }

  /**
	 * Before set up.
	 */
  protected void beforeSetUp() {
  }

  /**
	 * After set up.
	 */
  protected void afterSetUp() {
  }

  /**
	 * Before tear down.
	 */
  protected void beforeTearDown() {
  }

  /**
	 * After tear down.
	 */
  protected void afterTearDown() {
  }
}
