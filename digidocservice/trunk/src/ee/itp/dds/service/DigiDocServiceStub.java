/**
 * DigiDocServiceStub.java
 *
 * This file was auto-generated from WSDL
 * by the Apache Axis 1.4 Apr 22, 2006 (06:55:48 PDT) WSDL2Java emitter.
 */

package ee.itp.dds.service;

import java.util.Vector;

import javax.xml.soap.SOAPException;

import org.apache.axis.description.OperationDesc;
import org.apache.axis.message.SOAPFault;

public class DigiDocServiceStub extends org.apache.axis.client.Stub implements ee.itp.dds.service.DigiDocServicePortType {
    private Vector cachedSerClasses = new Vector();
    private Vector cachedSerQNames = new Vector();
    private Vector cachedSerFactories = new Vector();
    private Vector cachedDeserFactories = new Vector();

    static OperationDesc[] _operations;

    static {
        _operations = new OperationDesc[20];
        _initOperationDesc1();
        _initOperationDesc2();
    }

    private static void _initOperationDesc1() {
        org.apache.axis.description.OperationDesc oper;
        org.apache.axis.description.ParameterDesc param;
        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("StartSession");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SigningProfile"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SigDocXML"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "bHoldSession"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "boolean"), boolean.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "datafile"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "DataFileData"), ee.itp.dds.service.DataFileData.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[0] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("CloseSession");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(new javax.xml.namespace.QName("http://www.w3.org/2001/XMLSchema", "string"));
        oper.setReturnClass(java.lang.String.class);
        oper.setReturnQName(new javax.xml.namespace.QName("", "Status"));
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[1] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("CreateSignedDoc");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Format"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Version"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[2] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("AddDataFile");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "FileName"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "MimeType"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "ContentType"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Size"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "DigestType"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "DigestValue"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Content"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[3] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("RemoveDataFile");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "DataFileId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[4] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetSignedDoc");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[5] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetSignedDocInfo");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[6] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetDataFile");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "DataFileId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "DataFileData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "DataFileData"), ee.itp.dds.service.DataFileData.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[7] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetSignersCertificate");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "CertificateData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[8] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetNotarysCertificate");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "CertificateData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[9] = oper;

    }

    private static void _initOperationDesc2() {
        org.apache.axis.description.OperationDesc oper;
        org.apache.axis.description.ParameterDesc param;
        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetNotary");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "OcspData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[10] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetTSACertificate");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "TimestampId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "CertificateData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[11] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetTimestamp");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "TimestampId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "TimestampData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[12] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetCRL");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "CRLData"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[13] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetSignatureModules");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Platform"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Phase"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Type"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Modules"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignatureModulesArray"), ee.itp.dds.service.SignatureModule[].class, false, false);
        param.setItemQName(new javax.xml.namespace.QName("", "Modules"));
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[14] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("PrepareSignature");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignersCertificate"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignersTokenId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Role"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "City"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "State"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "PostalCode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Country"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SigningProfile"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureId"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedInfoDigest"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[15] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("FinalizeSignature");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureValue"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[16] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("RemoveSignature");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignatureId"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[17] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetVersion");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Name"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Version"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[18] = oper;

        oper = new org.apache.axis.description.OperationDesc();
        oper.setName("GetStatusInfo");
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Sesscode"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "int"), int.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "ReturnDocInfo"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "boolean"), boolean.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "WaitSignature"), org.apache.axis.description.ParameterDesc.IN, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "boolean"), boolean.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "Status"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "StatusCode"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.w3.org/2001/XMLSchema", "string"), java.lang.String.class, false, false);
        oper.addParameter(param);
        param = new org.apache.axis.description.ParameterDesc(new javax.xml.namespace.QName("", "SignedDocInfo"), org.apache.axis.description.ParameterDesc.OUT, new javax.xml.namespace.QName(
                "http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo"), ee.itp.dds.service.SignedDocInfo.class, false, false);
        oper.addParameter(param);
        oper.setReturnType(org.apache.axis.encoding.XMLType.AXIS_VOID);
        oper.setStyle(org.apache.axis.constants.Style.RPC);
        oper.setUse(org.apache.axis.constants.Use.ENCODED);
        _operations[19] = oper;

    }

    public DigiDocServiceStub() throws org.apache.axis.AxisFault {
        this(null);
    }

    public DigiDocServiceStub(java.net.URL endpointURL, javax.xml.rpc.Service service) throws org.apache.axis.AxisFault {
        this(service);
        super.cachedEndpoint = endpointURL;
    }

    public DigiDocServiceStub(javax.xml.rpc.Service service) throws org.apache.axis.AxisFault {
        if (service == null) {
            super.service = new org.apache.axis.client.Service();
        } else {
            super.service = service;
        }
        ((org.apache.axis.client.Service) super.service).setTypeMappingVersion("1.2");
        java.lang.Class cls;
        javax.xml.namespace.QName qName;
        javax.xml.namespace.QName qName2;
        java.lang.Class beansf = org.apache.axis.encoding.ser.BeanSerializerFactory.class;
        java.lang.Class beandf = org.apache.axis.encoding.ser.BeanDeserializerFactory.class;
        java.lang.Class enumsf = org.apache.axis.encoding.ser.EnumSerializerFactory.class;
        java.lang.Class enumdf = org.apache.axis.encoding.ser.EnumDeserializerFactory.class;
        java.lang.Class arraysf = org.apache.axis.encoding.ser.ArraySerializerFactory.class;
        java.lang.Class arraydf = org.apache.axis.encoding.ser.ArrayDeserializerFactory.class;
        java.lang.Class simplesf = org.apache.axis.encoding.ser.SimpleSerializerFactory.class;
        java.lang.Class simpledf = org.apache.axis.encoding.ser.SimpleDeserializerFactory.class;
        java.lang.Class simplelistsf = org.apache.axis.encoding.ser.SimpleListSerializerFactory.class;
        java.lang.Class simplelistdf = org.apache.axis.encoding.ser.SimpleListDeserializerFactory.class;
        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "CertificateInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.CertificateInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "CertificatePolicy");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.CertificatePolicy.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "ConfirmationInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.ConfirmationInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "CRLInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.CRLInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "DataFileAttribute");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.DataFileAttribute.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "DataFileData");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.DataFileData.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "DataFileInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.DataFileInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "Error");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.Error.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "RevokedInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.RevokedInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignatureInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.SignatureInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignatureModule");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.SignatureModule.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignatureModulesArray");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.SignatureModule[].class;
        cachedSerClasses.add(cls);
        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignatureModule");
        qName2 = new javax.xml.namespace.QName("", "Modules");
        cachedSerFactories.add(new org.apache.axis.encoding.ser.ArraySerializerFactory(qName, qName2));
        cachedDeserFactories.add(new org.apache.axis.encoding.ser.ArrayDeserializerFactory());

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignatureProductionPlace");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.SignatureProductionPlace.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignedDocInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.SignedDocInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignerInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.SignerInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "SignerRole");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.SignerRole.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

        qName = new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "TstInfo");
        cachedSerQNames.add(qName);
        cls = ee.itp.dds.service.TstInfo.class;
        cachedSerClasses.add(cls);
        cachedSerFactories.add(beansf);
        cachedDeserFactories.add(beandf);

    }

    protected org.apache.axis.client.Call createCall() throws java.rmi.RemoteException {
        try {
            org.apache.axis.client.Call _call = super._createCall();
            if (super.maintainSessionSet) {
                _call.setMaintainSession(super.maintainSession);
            }
            if (super.cachedUsername != null) {
                _call.setUsername(super.cachedUsername);
            }
            if (super.cachedPassword != null) {
                _call.setPassword(super.cachedPassword);
            }
            if (super.cachedEndpoint != null) {
                _call.setTargetEndpointAddress(super.cachedEndpoint);
            }
            if (super.cachedTimeout != null) {
                _call.setTimeout(super.cachedTimeout);
            }
            if (super.cachedPortName != null) {
                _call.setPortName(super.cachedPortName);
            }
            java.util.Enumeration keys = super.cachedProperties.keys();
            while (keys.hasMoreElements()) {
                java.lang.String key = (java.lang.String) keys.nextElement();
                _call.setProperty(key, super.cachedProperties.get(key));
            }
            // All the type mapping information is registered
            // when the first call is made.
            // The type mapping information is actually registered in
            // the TypeMappingRegistry of the service, which
            // is the reason why registration is only needed for the first call.
            synchronized (this) {
                if (firstCall()) {
                    // must set encoding style before registering serializers
                    _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
                    _call.setEncodingStyle(org.apache.axis.Constants.URI_SOAP11_ENC);
                    for (int i = 0; i < cachedSerFactories.size(); ++i) {
                        java.lang.Class cls = (java.lang.Class) cachedSerClasses.get(i);
                        javax.xml.namespace.QName qName = (javax.xml.namespace.QName) cachedSerQNames.get(i);
                        java.lang.Object x = cachedSerFactories.get(i);
                        if (x instanceof Class) {
                            java.lang.Class sf = (java.lang.Class) cachedSerFactories.get(i);
                            java.lang.Class df = (java.lang.Class) cachedDeserFactories.get(i);
                            _call.registerTypeMapping(cls, qName, sf, df, false);
                        } else if (x instanceof javax.xml.rpc.encoding.SerializerFactory) {
                            org.apache.axis.encoding.SerializerFactory sf = (org.apache.axis.encoding.SerializerFactory) cachedSerFactories.get(i);
                            org.apache.axis.encoding.DeserializerFactory df = (org.apache.axis.encoding.DeserializerFactory) cachedDeserFactories.get(i);
                            _call.registerTypeMapping(cls, qName, sf, df, false);
                        }
                    }
                }
            }
            return _call;
        } catch (java.lang.Throwable _t) {
            throw new org.apache.axis.AxisFault("Failure trying to get the Call object", _t);
        }
    }

    public void startSession(java.lang.String signingProfile, java.lang.String sigDocXML, boolean bHoldSession, ee.itp.dds.service.DataFileData datafile, javax.xml.rpc.holders.StringHolder status,
            javax.xml.rpc.holders.IntHolder sesscode, ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[0]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "StartSession"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { signingProfile, sigDocXML, new java.lang.Boolean(bHoldSession), datafile });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    sesscode.value = ((java.lang.Integer) _output.get(new javax.xml.namespace.QName("", "Sesscode"))).intValue();
                } catch (java.lang.Exception _exception) {
                    sesscode.value = ((java.lang.Integer) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Sesscode")), int.class)).intValue();
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public java.lang.String closeSession(int sesscode) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[1]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "CloseSession"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode) });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                try {
                    return (java.lang.String) _resp;
                } catch (java.lang.Exception _exception) {
                    return (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_resp, java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void createSignedDoc(int sesscode, java.lang.String format, java.lang.String version, javax.xml.rpc.holders.StringHolder status, ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo)
            throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[2]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "CreateSignedDoc"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), format, version });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void addDataFile(int sesscode, java.lang.String fileName, java.lang.String mimeType, java.lang.String contentType, int size, java.lang.String digestType, java.lang.String digestValue,
            java.lang.String content, javax.xml.rpc.holders.StringHolder status, ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[3]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "AddDataFile"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), fileName, mimeType, contentType, new java.lang.Integer(size), digestType, digestValue,
                    content });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void removeDataFile(int sesscode, java.lang.String dataFileId, javax.xml.rpc.holders.StringHolder status, ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo)
            throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[4]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "RemoveDataFile"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), dataFileId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getSignedDoc(int sesscode, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder signedDocData) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[5]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetSignedDoc"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode) });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signedDocData.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "SignedDocData"));
                } catch (java.lang.Exception _exception) {
                    signedDocData.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocData")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getSignedDocInfo(int sesscode, javax.xml.rpc.holders.StringHolder status, ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[6]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetSignedDocInfo"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode) });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getDataFile(int sesscode, java.lang.String dataFileId, javax.xml.rpc.holders.StringHolder status, ee.itp.dds.service.holders.DataFileDataHolder dataFileData)
            throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[7]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetDataFile"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), dataFileId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    dataFileData.value = (ee.itp.dds.service.DataFileData) _output.get(new javax.xml.namespace.QName("", "DataFileData"));
                } catch (java.lang.Exception _exception) {
                    dataFileData.value = (ee.itp.dds.service.DataFileData) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "DataFileData")),
                            ee.itp.dds.service.DataFileData.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getSignersCertificate(int sesscode, java.lang.String signatureId, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder certificateData)
            throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[8]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetSignersCertificate"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), signatureId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    certificateData.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "CertificateData"));
                } catch (java.lang.Exception _exception) {
                    certificateData.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "CertificateData")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getNotarysCertificate(int sesscode, java.lang.String signatureId, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder certificateData)
            throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[9]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetNotarysCertificate"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), signatureId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    certificateData.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "CertificateData"));
                } catch (java.lang.Exception _exception) {
                    certificateData.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "CertificateData")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getNotary(int sesscode, java.lang.String signatureId, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder ocspData) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[10]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetNotary"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), signatureId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    ocspData.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "OcspData"));
                } catch (java.lang.Exception _exception) {
                    ocspData.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "OcspData")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getTSACertificate(int sesscode, java.lang.String timestampId, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder certificateData)
            throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[11]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetTSACertificate"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), timestampId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    certificateData.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "CertificateData"));
                } catch (java.lang.Exception _exception) {
                    certificateData.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "CertificateData")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getTimestamp(int sesscode, java.lang.String timestampId, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder timestampData) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[12]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetTimestamp"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), timestampId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    timestampData.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "TimestampData"));
                } catch (java.lang.Exception _exception) {
                    timestampData.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "TimestampData")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getCRL(int sesscode, java.lang.String signatureId, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder CRLData) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[13]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetCRL"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), signatureId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    CRLData.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "CRLData"));
                } catch (java.lang.Exception _exception) {
                    CRLData.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "CRLData")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getSignatureModules(int sesscode, java.lang.String platform, java.lang.String phase, java.lang.String type, javax.xml.rpc.holders.StringHolder status,
            ee.itp.dds.service.holders.SignatureModulesArrayHolder modules) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[14]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetSignatureModules"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), platform, phase, type });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    modules.value = (ee.itp.dds.service.SignatureModule[]) _output.get(new javax.xml.namespace.QName("", "Modules"));
                } catch (java.lang.Exception _exception) {
                    modules.value = (ee.itp.dds.service.SignatureModule[]) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Modules")),
                            ee.itp.dds.service.SignatureModule[].class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void prepareSignature(int sesscode, java.lang.String signersCertificate, java.lang.String signersTokenId, java.lang.String role, java.lang.String city, java.lang.String state,
            java.lang.String postalCode, java.lang.String country, java.lang.String signingProfile, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder signatureId,
            javax.xml.rpc.holders.StringHolder signedInfoDigest) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[15]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "PrepareSignature"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call
                    .invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), signersCertificate, signersTokenId, role, city, state, postalCode, country, signingProfile });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signatureId.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "SignatureId"));
                } catch (java.lang.Exception _exception) {
                    signatureId.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignatureId")), java.lang.String.class);
                }
                try {
                    signedInfoDigest.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "SignedInfoDigest"));
                } catch (java.lang.Exception _exception) {
                    signedInfoDigest.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedInfoDigest")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void finalizeSignature(int sesscode, java.lang.String signatureId, java.lang.String signatureValue, javax.xml.rpc.holders.StringHolder status,
            ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[16]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "FinalizeSignature"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), signatureId, signatureValue });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void removeSignature(int sesscode, java.lang.String signatureId, javax.xml.rpc.holders.StringHolder status, ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo)
            throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[17]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "RemoveSignature"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), signatureId });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getVersion(javax.xml.rpc.holders.StringHolder name, javax.xml.rpc.holders.StringHolder version) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[18]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetVersion"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] {});

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    name.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Name"));
                } catch (java.lang.Exception _exception) {
                    name.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Name")), java.lang.String.class);
                }
                try {
                    version.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Version"));
                } catch (java.lang.Exception _exception) {
                    version.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Version")), java.lang.String.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }

    public void getStatusInfo(int sesscode, boolean returnDocInfo, boolean waitSignature, javax.xml.rpc.holders.StringHolder status, javax.xml.rpc.holders.StringHolder statusCode,
            ee.itp.dds.service.holders.SignedDocInfoHolder signedDocInfo) throws java.rmi.RemoteException {
        if (super.cachedEndpoint == null) {
            throw new org.apache.axis.NoEndPointException();
        }
        org.apache.axis.client.Call _call = createCall();
        _call.setOperation(_operations[19]);
        _call.setUseSOAPAction(true);
        _call.setSOAPActionURI("");
        _call.setSOAPVersion(org.apache.axis.soap.SOAPConstants.SOAP11_CONSTANTS);
        _call.setOperationName(new javax.xml.namespace.QName("http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl", "GetStatusInfo"));

        setRequestHeaders(_call);
        setAttachments(_call);
        try {
            java.lang.Object _resp = _call.invoke(new java.lang.Object[] { new java.lang.Integer(sesscode), new java.lang.Boolean(returnDocInfo), new java.lang.Boolean(waitSignature) });

            if (_resp instanceof java.rmi.RemoteException) {
                throw (java.rmi.RemoteException) _resp;
            } else {
                extractAttachments(_call);
                java.util.Map _output;
                _output = _call.getOutputParams();
                try {
                    status.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "Status"));
                } catch (java.lang.Exception _exception) {
                    status.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "Status")), java.lang.String.class);
                }
                try {
                    statusCode.value = (java.lang.String) _output.get(new javax.xml.namespace.QName("", "StatusCode"));
                } catch (java.lang.Exception _exception) {
                    statusCode.value = (java.lang.String) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "StatusCode")), java.lang.String.class);
                }
                try {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) _output.get(new javax.xml.namespace.QName("", "SignedDocInfo"));
                } catch (java.lang.Exception _exception) {
                    signedDocInfo.value = (ee.itp.dds.service.SignedDocInfo) org.apache.axis.utils.JavaUtils.convert(_output.get(new javax.xml.namespace.QName("", "SignedDocInfo")),
                            ee.itp.dds.service.SignedDocInfo.class);
                }
            }
        } catch (org.apache.axis.AxisFault axisFaultException) {
            throw axisFaultException;
        }
    }
    
    public boolean hasResponseFaultString() {
      try {
        return _call.getMessageContext().getResponseMessage().getSOAPPart().getEnvelope().getBody().hasFault();
      } catch (SOAPException e) {
        return false;
      }
    }

    public String getFaultString() {
      try {
        javax.xml.soap.SOAPFault fault = _call.getMessageContext().getResponseMessage().getSOAPBody().getFault();
        String fs = fault.getFaultString();
        if (fs.startsWith("java.rmi.RemoteException:")) 
          return fs.substring("java.rmi.RemoteException:".length()+1);
        return fs;
      } catch (SOAPException e) {
        return null;
      }
    }
    
    

}
