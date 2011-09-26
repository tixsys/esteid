package ee.sk.digidoc;

import java.io.Serializable;

/**
 * Models the QualifyingProperties element of
 * an BDOC.
 * @author  Kalev Suik
 * @version 1.0
 */

public class QualifyingProperties  implements Serializable{
	private String m_Target;

	public String getTarget() {
		return m_Target;
	}

	public void setTarget(String target) {
		m_Target = target;
	}

}
