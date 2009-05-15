/**
 * DigiDocService_Service.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis 1.4 Apr 22, 2006 (06:55:48 PDT) WSDL2Java emitter.
 */

package ee.itp.dds.service;

public interface DigiDocService_Service extends javax.xml.rpc.Service {

/**
 * Digital signature service
 */
    public java.lang.String getDigiDocServiceAddress();

    public ee.itp.dds.service.DigiDocServicePortType getDigiDocService() throws javax.xml.rpc.ServiceException;

    public ee.itp.dds.service.DigiDocServicePortType getDigiDocService(java.net.URL portAddress) throws javax.xml.rpc.ServiceException;
}
