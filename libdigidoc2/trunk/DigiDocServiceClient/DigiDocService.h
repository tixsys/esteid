//gsoap d service name: DigiDocService
//gsoap d service style: rpc
//gsoap d service encoding: encoded
//gsoap d schema elementForm: unqualified 
//gsoap d schema attributeForm: qualified
//gsoap d service location: http://www.sk.ee:8095/DigiDocService
//gsoap d schema namespace: http://www.sk.ee/DigiDocService/DigiDocService_2_3.wsdl
//gsoap d service method-action: add "" 
//gsoap d service documentation: Digital signature service

typedef char *xsd__string; // encode xsd string value as the xsd:string schema type 
typedef char *xsd__anyURI; // encode xsd anyURI value as the xsd:anyURI schema type 
typedef float xsd__float; // encode xsd float value as the xsd:float schema type 
typedef long xsd__int; // encode xsd int value as the xsd:int schema type 
typedef bool xsd__boolean; // encode xsd boolean value as the xsd:boolean schema type 
typedef unsigned long long xsd__positiveInteger; // encode xsd positiveInteger value as the xsd:positiveInteger schema type
typedef char* XML;
typedef char* xsd__dateTime;

//enum xsd__boolean { false_, true_ };

struct xsd__hexBinary 
{
 public: 
  unsigned char *__ptr; 
  int __size; 
};

struct d__DataFileAttribute
{
  xsd__string Name;
  xsd__string Value;
};


struct d__DataFileInfo
{
  xsd__string Id;
  xsd__string Filename;
  xsd__string MimeType;
  xsd__string ContentType;
  xsd__int Size;
  xsd__string DigestType;
  xsd__string DigestValue;
  // optional atributes
  int __sizeOfAttributes;
  struct d__DataFileAttribute* __ptrAttributes;
};

struct d__SignerRole
{
  xsd__int    Certified;
  xsd__string Role;
};

struct d__SignatureProductionPlace
{
  xsd__string City;
  xsd__string StateOrProvince;
  xsd__string PostalCode;
  xsd__string CountryName;
};

struct d__CertificatePolicy
{
  xsd__string OID;
  xsd__string URL;
  xsd__string Description;
};

struct d__CertificateInfo
{
  xsd__string Issuer;
  xsd__string Subject;
  xsd__dateTime ValidFrom;
  xsd__dateTime ValidTo;
  xsd__string IssuerSerial;
  int __sizePolicies;
  struct d__CertificatePolicy* __ptrPolicies;
};

struct d__SignerInfo
{
  xsd__string CommonName;
  xsd__string IDCode;
  struct d__CertificateInfo* Certificate;
};

struct d__ConfirmationInfo
{
  xsd__string ResponderID;
  xsd__string ProducedAt;
  struct d__CertificateInfo* ResponderCertificate;
};

struct d__TstInfo
{
  xsd__string Id;
  xsd__string Type;
  xsd__string SerialNumber;
  xsd__dateTime CreationTime;
  xsd__string Policy;
  xsd__string ErrorBound;
  xsd__boolean Ordered;
  xsd__string TSA;
  struct d__CertificateInfo* Certificate;
};

struct d__RevokedInfo
{
  xsd__positiveInteger Sequence;
  xsd__string SerialNumber;
  xsd__dateTime RevocationDate;
};

struct d__CRLInfo
{
  xsd__string Issuer;
  xsd__dateTime LastUpdate;
  xsd__dateTime NextUpdate;
  // optional atributes
  int __sizeOfRevocations;
  struct d__RevokedInfo* __ptrRevocations;
};


struct d__Error
{
  xsd__int  Code;
  xsd__string Category;
  xsd__string Description;
};

struct d__SignatureInfo
{
  xsd__string  Id;
  xsd__string  Status;
  // optional error
  struct d__Error* Error;
  xsd__dateTime  SigningTime;
  // roles
  int __sizeRoles;
  struct d__SignerRole*  __ptrSignerRole;
  // address
  struct d__SignatureProductionPlace* SignatureProductionPlace;
  // signer info
  struct d__SignerInfo* Signer;
  // confirmation info
  struct d__ConfirmationInfo* Confirmation;
  // timestamps
  int __sizeTimestamps;
  struct d__TstInfo* __ptrTimestamps;
  // CRL info
  struct d__CRLInfo* CRLInfo;
};

struct d__SignedDocInfo
{
  xsd__string Format;
  xsd__string Version;
  // DataFile -s
  int __sizeOfDataFileInfos;
  struct d__DataFileInfo* __ptrDataFileInfo;
  // SignatureInfos
  int __sizeOfSignatureInfos;
  struct d__SignatureInfo* __ptrSignatureInfo;
};


struct d__DataFileData
{
  xsd__string Id;
  xsd__string Filename;
  xsd__string MimeType;
  xsd__string ContentType;
  xsd__string DigestType;
  xsd__string DigestValue;
  xsd__int Size;
  int __sizeOfAttributes;
  struct d__DataFileAttribute* __ptrAttributes;
  xsd__string DfData;
};


struct d__StartSessionResponse { 
  xsd__string Status;
  xsd__int Sesscode;
  struct d__SignedDocInfo* SignedDocInfo; 
};

//gsoap d service method-documentation: d__StartSession This method starts a new session
//gsoap d service method-output-header-part: d__StartSession d__SessionCode
int d__StartSession(xsd__string SigningProfile, xsd__string SigDocXML, xsd__boolean bHoldSession,
		    struct d__DataFileData* datafile, struct d__StartSessionResponse* result);

struct d__CloseSessionResponse {
  xsd__string Status;
};

//gsoap d service method-documentation: d__CloseSession This method close an existing session
//gsoap d service method-header-part: d__CloseSession d__SessionCode
int d__CloseSession(xsd__int Sesscode, struct d__CloseSessionResponse* result);


struct d__CreateSignedDocResponse {
  xsd__string Status;
  struct d__SignedDocInfo* SignedDocInfo;
};

//gsoap d service method-documentation: d__CreateSignedDoc This method creates a new digidoc container and deletes old one if it had existed in this session
//gsoap d service method-header-part: d__CreateSignedDoc d__SessionCode
int d__CreateSignedDoc(xsd__int Sesscode, xsd__string Format, xsd__string Version, 
		       struct d__CreateSignedDocResponse* result);


struct d__AddDataFileResponse {
  xsd__string Status;
  struct d__SignedDocInfo* SignedDocInfo;
};

//gsoap d service method-documentation: d__AddDataFile This method adds a new DataFile
//gsoap d service method-header-part: d__AddDataFile d__SessionCode
int d__AddDataFile(xsd__int Sesscode, xsd__string FileName, xsd__string MimeType, 
		   xsd__string ContentType, xsd__int Size,
		   xsd__string DigestType, xsd__string DigestValue,
		   xsd__string Content, struct d__AddDataFileResponse* result);

struct d__RemoveDataFileResponse {
  xsd__string Status;
  struct d__SignedDocInfo* SignedDocInfo;
};

//gsoap d service method-documentation: d__RemoveDataFile This method removes an existing DataFile
//gsoap d service method-header-part: d__RemoveDataFile d__SessionCode
int d__RemoveDataFile(xsd__int Sesscode, xsd__string DataFileId, 
		      struct d__RemoveDataFileResponse* result);


struct d__GetSignedDocResponse { 
  xsd__string Status;
  xsd__string SignedDocData;
};

//gsoap d service method-documentation: d__GetSignedDoc This method returns the digidoc document
//gsoap d service method-header-part: d__GetSignedDoc d__SessionCode
int d__GetSignedDoc(xsd__int Sesscode, 
		    struct d__GetSignedDocResponse* result);


struct d__GetSignedDocInfoResponse { 
  xsd__string Status;
  struct d__SignedDocInfo* SignedDocInfo;
};

//gsoap d service method-documentation: d__GetSignedDocInfo This method returns the digidoc info
//gsoap d service method-header-part: d__GetSignedDocInfo d__SessionCode
int d__GetSignedDocInfo(xsd__int Sesscode, 
			struct d__GetSignedDocInfoResponse* result);


struct d__GetDataFileResponse {
  xsd__string Status;
  struct d__DataFileData* DataFileData;
};

//gsoap d service method-documentation: d__GetDataFile This method return existing DataFile info and data
//gsoap d service method-header-part: d__GetDataFile d__SessionCode
int d__GetDataFile(xsd__int Sesscode, xsd__string DataFileId, 
		   struct d__GetDataFileResponse* result);


struct d__GetSignersCertificateResponse {
  xsd__string Status;
  xsd__string CertificateData;
};

//gsoap d service method-documentation: d__GetSignersCertificate This method returns signers certificate in PEM form
//gsoap d service method-header-part: d__GetSignersCertificate d__SessionCode
int d__GetSignersCertificate(xsd__int Sesscode, xsd__string SignatureId, 
			     struct d__GetSignersCertificateResponse* result);

struct d__GetNotarysCertificateResponse {
  xsd__string Status;
  xsd__string CertificateData;
};

//gsoap d service method-documentation: d__GetNotarysCertificate This method returns notarys certificate in PEM form
//gsoap d service method-header-part: d__GetNotarysCertificate d__SessionCode
int d__GetNotarysCertificate(xsd__int Sesscode, xsd__string SignatureId, 
			     struct d__GetNotarysCertificateResponse* result);

struct d__GetNotaryResponse {
  xsd__string Status;
  xsd__string OcspData;
};

//gsoap d service method-documentation: d__GetNotary This method returns notarys certificate OCSP responsein PEM form
int d__GetNotary(xsd__int Sesscode, xsd__string SignatureId, 
		 struct d__GetNotaryResponse* result);


struct d__GetTSACertificateResponse {
  xsd__string Status;
  xsd__string CertificateData;
};

//gsoap d service method-documentation: d__GetTSACertificate This method returns TSA certificate in PEM form
int d__GetTSACertificate(xsd__int Sesscode, xsd__string TimestampId, 
			     struct d__GetTSACertificateResponse* result);

struct d__GetTimestampResponse {
  xsd__string Status;
  xsd__string TimestampData;
};

//gsoap d service method-documentation: d__GetNotary This method returns notarys certificate OCSP responsein PEM form
int d__GetTimestamp(xsd__int Sesscode, xsd__string TimestampId, 
		 struct d__GetTimestampResponse* result);


struct d__GetCRLResponse {
  xsd__string Status;
  xsd__string CRLData;
};

//gsoap d service method-documentation: d__GetCRL This method returns CRL in PEM form
int d__GetCRL(xsd__int Sesscode, xsd__string SignatureId, 
		 struct d__GetCRLResponse* result);



struct d__SignatureModule {
  xsd__string Name;        // name of the file or nothing for HTML or input element name/id
  xsd__string Type;        // type: FILE, HTML, HTML-INPUT
  xsd__string Location;    // DOCROOT, HTML-HEAD, HTML-BODY, HTML-FORM-BEGIN, HTML-FORM-END
  xsd__string ContentType; // HTML, BASE64
  xsd__string Content;     // files content in base64 or plain HTML text
};


struct d__SignatureModulesArray {
  int __sizeOfModules;
  struct d__SignatureModule* __ptrModules;
};

struct d__GetSignatureModulesResponse {
  xsd__string Status;
  struct d__SignatureModulesArray* Modules;
};

//gsoap d service method-documentation: d__GetSignatureModules This method returns signature modules and HTML page content for signing pages
//gsoap d service method-header-part: d__GetSignatureModules d__SessionCode
int d__GetSignatureModules(xsd__int Sesscode, 
			   xsd__string Platform, // LINUX-MOZILLA, WIN32-MOZILLA, WIN32-IE
			   xsd__string Phase, // PREPARE or FINALIZE
			   xsd__string Type,  // DYNAMIC or STATIC or ALL
			   struct d__GetSignatureModulesResponse* result);


struct d__PrepareSignatureResponse {
  xsd__string Status;
  xsd__string SignatureId;
  xsd__string SignedInfoDigest;
};

//gsoap d service method-documentation: d__PrepareSignature This method prepares a new signature
//gsoap d service method-header-part: d__PrepareSignature d__SessionCode
int d__PrepareSignature(xsd__int Sesscode, 
			xsd__string SignersCertificate,
			xsd__string SignersTokenId,
			xsd__string Role,
			xsd__string City,
			xsd__string State,
			xsd__string PostalCode,
			xsd__string Country,
			xsd__string SigningProfile, 
			struct d__PrepareSignatureResponse* result);


struct d__FinalizeSignatureResponse {
  xsd__string Status;
  struct d__SignedDocInfo* SignedDocInfo;
};

//gsoap d service method-documentation: d__FinalizeSignature This method finalizes a new signature
//gsoap d service method-header-part: d__FinalizeSignature d__SessionCode
int d__FinalizeSignature(xsd__int Sesscode, 
			 xsd__string SignatureId, xsd__string SignatureValue,
			struct d__FinalizeSignatureResponse* result);


struct d__RemoveSignatureResponse {
  xsd__string Status;
  struct d__SignedDocInfo* SignedDocInfo;
};

//gsoap d service method-documentation: d__RemoveSignature This method removes an existing signature
//gsoap d service method-header-part: d__RemoveSignature d__SessionCode
int d__RemoveSignature(xsd__int Sesscode, xsd__string SignatureId, 
		       struct d__RemoveSignatureResponse* result);


struct d__GetVersionResponse {
  xsd__string Name;
  xsd__string Version;
};

//gsoap d service method-documentation: d__GetVersion This method returns the name and version of service
int d__GetVersion(struct d__GetVersionResponse* result);

struct d__MobileSignResponse {
	xsd__string Status;
	xsd__string StatusCode;
	xsd__string ChallengeID;
};

//gsoap d service method-documentation: d__MobileSignRequest sends a request for mobile signing
int d__MobileSign(xsd__int Sesscode,
				  xsd__string SignerIDCode, 
				  xsd__string SignersCountry,
				  xsd__string SignerPhoneNo,
				  xsd__string ServiceName,
				  xsd__string AdditionalDataToBeDisplayed,
				  xsd__string Language,
				  xsd__string Role,
				  xsd__string City,
				  xsd__string StateOrProvince,
				  xsd__string PostalCode,
				  xsd__string CountryName,
				  xsd__string SigningProfile,
				  xsd__string MessagingMode,
				  xsd__int AsyncConfiguration,
				  xsd__boolean ReturnDocInfo,
				  xsd__boolean ReturnDocData,
				  struct d__MobileSignResponse* result);

struct d__GetStatusInfoResponse {
	xsd__string Status;
	xsd__string StatusCode;
	struct d__SignedDocInfo* SignedDocInfo;
};

//gsoap d service method-documentation: d__GetStatusInfoRequest sends mobile signing status info request
int d__GetStatusInfo(xsd__int Sesscode,
					 xsd__boolean ReturnDocInfo,
					 xsd__boolean WaitSignature,
					 struct d__GetStatusInfoResponse* result);


struct d__MobileAuthenticateResponse 
{
	xsd__int Sesscode;
	xsd__string Status;
	xsd__string UserIDCode;
	xsd__string UserGivenname;
	xsd__string UserSurname;
	xsd__string UserCountry;
	xsd__string UserCN;
	xsd__string CertificateData;
	xsd__string ChallengeID;
	xsd__string Challenge;
	xsd__string RevocationData;
};

//gsoap d service method-documentation: d__MobileAuthenticateRequest sends a request for mobile authentication
int d__MobileAuthenticate(xsd__string IDCode,
						  xsd__string CountryCode, 
						  xsd__string PhoneNo,
						  xsd__string Language,
						  xsd__string ServiceName,
						  xsd__string MessageToDisplay,
						  xsd__string SPChallenge,
						  xsd__string MessagingMode,
						  xsd__int AsyncConfiguration,
						  xsd__boolean ReturnCertData,
						  xsd__boolean ReturnRevocationData,
						  struct d__MobileAuthenticateResponse* result);


struct d__GetMobileAuthenticateStatusResponse
{
  xsd__string Status;
  xsd__string Signature; // pkcs#1 konteiner base64 kujul
};

//gsoap d service method-documentation: d__GetMobileAuthenticateStatus sends mobile authenticate status info request
int d__GetMobileAuthenticateStatus(xsd__int Sesscode,
								   xsd__boolean WaitSignature,
								   struct d__GetMobileAuthenticateStatusResponse* result);

struct d__DataFileDigest
{
	xsd__string Id;
	xsd__string DigestType;
	xsd__string DigestValue;
};

struct d__DataFileDigestList
{
	int __sizeOfDataFileDigests;
 	struct d__DataFileDigest* __ptrDataFileDigest;
};

struct d__MobileCreateSignatureResponse
{ 
	xsd__int Sesscode;
	xsd__string ChallengeID;
	xsd__string Status;
};
				   
//gsoap g service method-documentation: d_MobileCreateSignature sends a request to sign external digidoc by a mobile 
int d__MobileCreateSignature(xsd__string IDCode,
							 xsd__string SignersCountry,
							 xsd__string PhoneNo,
							 xsd__string Language,
							 xsd__string ServiceName,
							 xsd__string MessageToDisplay,
							 xsd__string Role,
							 xsd__string City,
							 xsd__string StateOrProvince,
							 xsd__string PostalCode,
							 xsd__string CountryName,
							 xsd__string SigningProfile,
							 struct d__DataFileDigestList* DataFiles,
							 xsd__string Format,
							 xsd__string Version,
							 xsd__string SignatureID,
							 xsd__string MessagingMode,
							 xsd__int AsyncConfiguration,
							 struct d__MobileCreateSignatureResponse* result);
 				
 
struct d__GetMobileCreateSignatureStatusResponse
{ 
	xsd__int Sesscode;
	xsd__string Status;
	xsd__string Signature;
};
 
//gsoap d service method-documentation: d__GetMobileCreateSignatureStatus sends mobile signature status info request
int d__GetMobileCreateSignatureStatus(xsd__int Sesscode,
									  xsd__boolean WaitSignature,
									  struct d__GetMobileCreateSignatureStatusResponse* result);



struct mss_DummyResp {
  xsd__string str;
};

//gsoap mss service name: MSSP_GW
//gsoap mss service style: rpc
//gsoap mss service encoding: literal
//gsoap mss schema elementForm: qualified 
//gsoap mss schema attributeForm: qualified
//gsoap mss service location: http://www.sk.ee:8096/MSSP_GW
//gsoap mss schema namespace: http://www.sk.ee:8096/MSSP_GW/MSSP_GW.wsdl
//gsoap mss service method-action: add "" 
//gsoap mss service documentation: MSSP Gateway Service

//gsoap mss service method-documentation: mss__MSS_SignatureDummy This method request a mobile signature
int mss__MSS_USCORESignatureDummy(struct mss_DummyResp* resp);



