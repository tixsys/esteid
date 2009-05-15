/**
 * 
 */
package ee.itp.dds.hibernate;

/**
 * @author Aleksandr Vassin
 * 
 */
public enum ActionEnum {
  SESSION_CREATED, SESSION_CLOSED, ADD_DATA_FILE,
  LOAD_DATA_FILE, REMOVE_DATA_FILE, GET_SIGNED_DOC, 
  GET_SIGNED_DOC_INFO, GET_DATA_FILE, GET_SIGNERS_CERTIFICATE, 
  GET_NOTARYS_CERTIFICATE, CREATE_SIGNED_DOC, GET_NOTARY
}
