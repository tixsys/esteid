package ee.sk.digidoc;

import java.io.Serializable;

/**
 * Represents a signature file inside BDOC
 * 
 * @author  Kalev Suik
 * @version 0.7
 */
public class SignatureFile  implements Serializable{
	/** fullpath inside BDOC container */
	private String fullPath;
	/** mime type as described in  BDOC's mainfest file	 */
	private String type;
	/**
	 * @return path inside BDOC container
	 */
	public String getFullPath() {
		return fullPath;
	}
	/**
	 * Sets path inside BDOC container
	 * @param fullPath
	 */
	public void setFullPath(String fullPath) {
		this.fullPath = fullPath;
	}
	/**
	 * @return mime type as described in  BDOC's mainfest file
	 */
	public String getType() {
		return type;
	}
	/**
	 * Sets mime type as described in  BDOC's mainfest file
	 * @param type
	 */
	public void setType(String type) {
		this.type = type;
	}
}
