package ee.itp.dds.test.core;

import java.sql.Types;

import org.dbunit.dataset.datatype.DataType;
import org.dbunit.dataset.datatype.DataTypeException;
import org.dbunit.dataset.datatype.DefaultDataTypeFactory;

/**
 * The Class HsqlDataTypeFactory.
 * 
 */
public class HsqlDataTypeFactory extends DefaultDataTypeFactory {
  
  /**
   * @see org.dbunit.dataset.datatype.DefaultDataTypeFactory#createDataType(int, java.lang.String)
   */
  public DataType createDataType(int sqlType, String sqlTypeName)
    throws DataTypeException {
    
    if (sqlType == Types.BOOLEAN) {
      return DataType.BOOLEAN;
    }
    return super.createDataType(sqlType, sqlTypeName);
  }
}