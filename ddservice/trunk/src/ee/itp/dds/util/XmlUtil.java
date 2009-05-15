package ee.itp.dds.util;

import java.io.StringReader;

/**
 * 
 * @author Aleksandr Vassin
 * @created  17.04.2009
 */
public class XmlUtil {

	private final static org.apache.log4j.Logger LOG = org.apache.log4j.Logger.getLogger(XmlUtil.class);

	/**
	 * Parse an XML file into a Document
	 */

	public static org.jdom.Document parseXML(java.io.File doc_file) {
		org.jdom.Document doc = null;
		try {
			org.jdom.input.SAXBuilder sax_builder = new org.jdom.input.SAXBuilder(false);
			java.io.FileReader file_reader = new java.io.FileReader(doc_file);
			java.io.BufferedReader buf_reader = new java.io.BufferedReader(file_reader);

			doc = sax_builder.build(buf_reader);
		} catch (Exception e) {
			LOG.error("parse XML", e);
		}
		return doc;
	}

	/**
	 * Parses an xml string
	 * 
	 * @param xmlString
	 * @return
	 */
	public static org.jdom.Document parseXML(String xmlString) {
		org.jdom.Document doc = null;
		try {
			org.jdom.input.SAXBuilder sax_builder = new org.jdom.input.SAXBuilder(false);
			doc = sax_builder.build(new StringReader(xmlString));
		} catch (Exception e) {
			LOG.error("parse XML", e);
		}
		return doc;

	}

	/**
	 * Format an XML Element into a String
	 */

	public static String formatXML(org.jdom.Element node) {
		String text = null;

		try {
			java.io.StringWriter writer = new java.io.StringWriter();

			org.jdom.output.XMLOutputter xml_outputter = new org.jdom.output.XMLOutputter(org.jdom.output.Format.getPrettyFormat());

			xml_outputter.output(node, writer);

			text = writer.toString();
		} catch (Exception e) {
			LOG.error("format XML", e);
		}

		return text;
	}

}
