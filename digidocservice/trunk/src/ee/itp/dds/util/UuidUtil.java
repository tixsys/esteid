/**
 * 
 */
package ee.itp.dds.util;

import java.util.Random;
import java.util.UUID;

/**
 * @author Aleksandr Vassin
 *
 */
public class UuidUtil {

  private static final Random rnd = new Random( System.currentTimeMillis() );
    
  public static String generateUuid()  {
    return UUID.randomUUID().toString().replaceAll("-", "");
  }

  public static int generateIntUuid()  {
      return rnd.nextInt(0xFFFE) + 1;
  }
  
  
}
