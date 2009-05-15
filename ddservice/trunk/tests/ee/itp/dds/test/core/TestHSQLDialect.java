package ee.itp.dds.test.core;

import org.hibernate.dialect.HSQLDialect;

/**
 * Custome dialect to use in tests.
 * 
 */
public class TestHSQLDialect extends HSQLDialect{
  
  /**
   * @see org.hibernate.dialect.Dialect#hasAlterTable()
   */
  public boolean hasAlterTable() {
    return false;
  }
}
