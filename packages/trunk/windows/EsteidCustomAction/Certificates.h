void handleError(char *s);
void removeCertificate(PCCERT_CONTEXT pCertContext);
void printHash(PCCERT_CONTEXT pCertContext);
void printName(PCCERT_CONTEXT pCertContext);
PCCERT_CONTEXT findCertificate(HCERTSTORE hSystemStore, unsigned char *hash);
bool findJuurSK(void);
void findRemoveCertificate(HCERTSTORE hSystemStore, unsigned char *hash);
void traverseStore(wchar_t * storeName);
