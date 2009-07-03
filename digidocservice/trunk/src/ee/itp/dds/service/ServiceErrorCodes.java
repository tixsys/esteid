/**
 * 
 */
package ee.itp.dds.service;

/**
 * @author konstantin.tarletski
 *
 */
public class ServiceErrorCodes {
  

  public static final String GENERAL_ERROR = "100"; //General error
  public static final String INCORRECT_INPUT_PARAMETERS = "101"; //Incorrect input parameters
  public static final String MISSING_PARAMETERS = "102"; //Some of required input parameters is missing
  public static final String ACCESS_DENIED = "103"; //Access denied (for example ServiceName is not
  //registered with DigiDocService provider)
  public static final String GENERAL_SERVICE_ERROR = "200"; //General error of the service
  public static final String MISSING_USER_CERTIFICATE = "201"; //Missing user certificate
  public static final String UNABELE_TOVEREFY_CERTIFICATE_VALIDITY = "202";// Unable to verify certificate validity
  public static final String GENERAL_USER_MOBILE_ERROR= "300";// General error related to user’s mobile phone
  public static final String NOT_A_MOBILE_ID_USER = "301";// Not a Mobile-ID user
  public static final String USER_CERTIFICATE_IS_NOT_VALID = "302";// The certificate of the user is not valid (OCSP said:
  //REVOKED)
  public static final String UNKNOWN_CERTIFICATE = "303"; //Certificate is not activated or/and status of the
  //certificate is unknown (OCSP said: UNKNOWN)

}
