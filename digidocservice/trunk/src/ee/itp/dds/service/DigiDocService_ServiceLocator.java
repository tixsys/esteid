/**
 * DigiDocService_ServiceLocator.java
 * 
 * This file was auto-generated from WSDL by the Apache Axis 1.4 Apr 22, 2006 (06:55:48 PDT) WSDL2Java emitter.
 */

package ee.itp.dds.service;

public class DigiDocService_ServiceLocator extends org.apache.axis.client.Service implements ee.itp.dds.service.DigiDocService_Service {

  private String DigiDocService_address = "http://www.sk.ee:8095/DigiDocService";
  private String DigiDocServiceWSDDServiceName = "DigiDocService";

  /**
   * Digital signature service
   */
  public DigiDocService_ServiceLocator() {
  }

  public DigiDocService_ServiceLocator(String serviceLocation) {
    this.DigiDocService_address = serviceLocation;
  }

  public DigiDocService_ServiceLocator(org.apache.axis.EngineConfiguration config) {
    super(config);
  }

  public DigiDocService_ServiceLocator(String wsdlLoc, javax.xml.namespace.QName sName) throws javax.xml.rpc.ServiceException {
    super(wsdlLoc, sName);
  }

  public String getDigiDocServiceAddress() {
    return DigiDocService_address;
  }

  public String getDigiDocServiceWSDDServiceName() {
    return DigiDocServiceWSDDServiceName;
  }

  public void setDigiDocServiceWSDDServiceName(String name) {
    DigiDocServiceWSDDServiceName = name;
  }

  public ee.itp.dds.service.DigiDocServicePortType getDigiDocService() throws javax.xml.rpc.ServiceException {
    java.net.URL endpoint;
    try {
      endpoint = new java.net.URL(DigiDocService_address);
    } catch (java.net.MalformedURLException e) {
      throw new javax.xml.rpc.ServiceException(e);
    }
    return getDigiDocService(endpoint);
  }

  public ee.itp.dds.service.DigiDocServicePortType getDigiDocService(java.net.URL portAddress) throws javax.xml.rpc.ServiceException {
    try {
      ee.itp.dds.service.DigiDocServiceStub _stub = new ee.itp.dds.service.DigiDocServiceStub(portAddress, this);
      _stub.setPortName(getDigiDocServiceWSDDServiceName());
      return _stub;
    } catch (org.apache.axis.AxisFault e) {
      return null;
    }
  }

  public void setDigiDocServiceEndpointAddress(String address) {
    DigiDocService_address = address;
  }

  /**
   * For the given interface, get the stub implementation. If this service has no port for the given interface, then
   * ServiceException is thrown.
   */
  public java.rmi.Remote getPort(Class serviceEndpointInterface) throws javax.xml.rpc.ServiceException {
    try {
      if (ee.itp.dds.service.DigiDocServicePortType.class.isAssignableFrom(serviceEndpointInterface)) {
        ee.itp.dds.service.DigiDocServiceStub _stub = new ee.itp.dds.service.DigiDocServiceStub(new java.net.URL(DigiDocService_address), this);
        _stub.setPortName(getDigiDocServiceWSDDServiceName());
        return _stub;
      }
    } catch (Throwable t) {
      throw new javax.xml.rpc.ServiceException(t);
    }
    throw new javax.xml.rpc.ServiceException("There is no stub implementation for the interface:  " + (serviceEndpointInterface == null ? "null" : serviceEndpointInterface.getName()));
  }

  /**
   * For the given interface, get the stub implementation. If this service has no port for the given interface, then
   * ServiceException is thrown.
   */
  public java.rmi.Remote getPort(javax.xml.namespace.QName portName, Class serviceEndpointInterface) throws javax.xml.rpc.ServiceException {
    if (portName == null) { return getPort(serviceEndpointInterface); }
    String inputPortName = portName.getLocalPart();
    if ("DigiDocService".equals(inputPortName)) {
      return getDigiDocService();
    } else {
      java.rmi.Remote _stub = getPort(serviceEndpointInterface);
      ((org.apache.axis.client.Stub) _stub).setPortName(portName);
      return _stub;
    }
  }

  public javax.xml.namespace.QName getServiceName() {
    return new javax.xml.namespace.QName("http://www.sk.ee:8095/DigiDocService/DigiDocService.wsdl", "DigiDocService");
  }

  private java.util.HashSet ports = null;

  public java.util.Iterator getPorts() {
    if (ports == null) {
      ports = new java.util.HashSet();
      ports.add(new javax.xml.namespace.QName("http://www.sk.ee:8095/DigiDocService/DigiDocService.wsdl", "DigiDocService"));
    }
    return ports.iterator();
  }

  /**
   * Set the endpoint address for the specified port name.
   */
  public void setEndpointAddress(String portName, String address) throws javax.xml.rpc.ServiceException {

    if ("DigiDocService".equals(portName)) {
      setDigiDocServiceEndpointAddress(address);
    } else { // Unknown Port Name
      throw new javax.xml.rpc.ServiceException(" Cannot set Endpoint Address for Unknown Port" + portName);
    }
  }

  /**
   * Set the endpoint address for the specified port name.
   */
  public void setEndpointAddress(javax.xml.namespace.QName portName, String address) throws javax.xml.rpc.ServiceException {
    setEndpointAddress(portName.getLocalPart(), address);
  }

}
