package ee.itp.dds.test.core;

import org.hibernate.SessionFactory;
import org.springframework.dao.DataAccessException;
import org.springframework.orm.hibernate3.LocalSessionFactoryBean;

import javax.sql.DataSource;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;

/**
 * The Class TestSessionFactoryBean.
 * 
 */
public class TestSessionFactoryBean extends LocalSessionFactoryBean {
  
  /* (non-Javadoc)
   * @see org.springframework.orm.hibernate3.LocalSessionFactoryBean#buildSessionFactory()
   */
  protected SessionFactory buildSessionFactory() throws Exception {
    preCreateSchema();
    return super.buildSessionFactory();
  }

  /**
	 * Pre create schema.
	 */
  private void preCreateSchema() {
    try {
      executeScript("CREATE SCHEMA ddservice AUTHORIZATION DBA");
    }
    catch (SQLException e) {
      logger.warn("Could not precreate schema " + e.getMessage());
    }
  }

  /**
	 * Execute script.
	 * 
	 * @param script
	 *            the script
	 * 
	 * @throws SQLException
	 *             the SQL exception
	 */
  private void executeScript(String script) throws SQLException {
    Connection con = null;
    Statement statement = null;
    final DataSource ds = getDataSource();
    try {
      con = ds.getConnection();
      statement = con.createStatement();
      statement.execute(script);
      logger.info("Executing script " + script);
    }
    finally {
      try {
        if (statement != null) {
          statement.close();
        }
      }
      finally {
        if (con != null) {
          con.close();
        }
      }
    }
  }

  /**
	 * Sysdate.
	 * 
	 * @return the java.sql. date
	 */
  public static java.sql.Date sysdate() {
    return new java.sql.Date(System.currentTimeMillis());
  }

  /**
   * @see org.springframework.orm.hibernate3.LocalSessionFactoryBean#createDatabaseSchema()
   */
  public void createDatabaseSchema() throws DataAccessException {
    preCreateSchema();
    try {
      super.createDatabaseSchema();
    }
    catch (Exception e) {
      logger.warn(e.getMessage());
    }
  }

  /**
   * @see org.springframework.orm.hibernate3.LocalSessionFactoryBean#dropDatabaseSchema()
   */
  public void dropDatabaseSchema() throws DataAccessException {
    try {
      executeScript("DROP SCHEMA ddservice CASCADE");
    }
    catch (SQLException e) {
      logger.warn("Could not drop schema " + e.getMessage());
    }
  }
}
