/**
 * 
 */
package ee.itp.dds.service;

import java.io.Serializable;

import javax.xml.namespace.QName;

import org.apache.axis.AxisFault;

/**
 * @author konstantin.tarletski
 *
 */
@SuppressWarnings("serial")
public class DigiDocServiceException extends AxisFault implements Serializable{
  
  
  
  public DigiDocServiceException(String message, String code, String detail , boolean isDebug){
    
    super(message);
    this.removeHostname();
    
    if(code != null && !code.equals("")){
      this.setFaultCodeAsString(code);
    }//if
    
    if(!isDebug){
      this.clearFaultDetails();
    }//if
    
    if(detail != null && !detail.equals("")){
      QName mesage = new QName("mesage");
      this.addFaultDetail(mesage,detail);
    }//if
    
  }//ServiceError
  

}
