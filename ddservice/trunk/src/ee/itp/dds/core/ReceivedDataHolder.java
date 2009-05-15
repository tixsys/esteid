package ee.itp.dds.core;

import java.util.HashMap;
import java.util.Map;


/**
 * ReceivedDataHolder - class to store data and hold recieved by axis servlet data.
 * 
 * @author Aleksandr Vassin
 * @created  15.04.2009
 */
public class ReceivedDataHolder {
  
  public static final String HOST_ADDRESS = "host_address";  
  public static final String CONTEXT_CONFIGURATION = "contextConfigLocation";
  
  
  /** The received message. */
  private static ThreadLocal<Map<String,Object>> data = new ThreadLocal<Map<String,Object>>();

  static {
      if (data.get() == null)
          data.set(new HashMap<String,Object>());
  }

  public static String getHost() {
      return (data.get().containsKey(HOST_ADDRESS)?(String)data.get().get(HOST_ADDRESS):"NO_HOST");
  }
  
  public static void setHost(String addr) {
      data.get().put(HOST_ADDRESS, addr);
  }
  
  public static void setContextConfigLocation(String contextConfigLocation) {
      data.get().put(CONTEXT_CONFIGURATION, contextConfigLocation);
  }

  public static String getContextConfigLocation() {
      String ret = (String) data.get().get(CONTEXT_CONFIGURATION);
      return ret != null? ret : "classpath*:applicationContext*.xml" ;
  }
  
  
  public static Map<String,Object> getDataMap() {
      return data.get();
  }

}
