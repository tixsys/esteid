/*
 * EsteidWixCustomAction - custom actions for Estonian EID card WiX installer
 *
 * Copyright (C) 2009-2010  Estonian Informatics Centre
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"

#pragma comment(lib, "crypt32.lib")

#include <stdlib.h>
#include <stdio.h>
#include <Wincrypt.h>
#include "hashes.h"
#include "skHashes.h"

// TODO: add MSI error handling / logging
void handleError(char *s)
{
	fprintf(stderr, "An error occurred in running the program. \n");
	fprintf(stderr, "%s\n", s);
	fprintf(stderr, "Error number %x.\n", GetLastError());
	fprintf(stderr, "Program terminating. \n");
	exit(1);
}

void removeCertificate(PCCERT_CONTEXT pCertContext)
{
	PCCERT_CONTEXT pDupCertContext = NULL;

	// Create a duplicate pointer to the certificate.
	pDupCertContext = CertDuplicateCertificateContext(pCertContext);
	if (!pDupCertContext) {
		handleError("Duplication of the certificate pointer failed.");
	}
	// Delete the certificate.
	if (!CertDeleteCertificateFromStore(pDupCertContext)) {
		handleError("The deletion of the certificate failed.");
	}
}

void printHash(PCCERT_CONTEXT pCertContext)
{
	BYTE *pvData;
	DWORD cbData;
	DWORD dwPropId = CERT_SHA1_HASH_PROP_ID;

	// Get property size. 
	if (!CertGetCertificateContextProperty(pCertContext,
					       dwPropId, NULL, &cbData)) {
		handleError("Call #1 to GetCertContextProperty failed.");
	}

	pvData = (BYTE *) malloc(cbData);
	if (!pvData)
		handleError("Memory allocation failed.");

	if (!CertGetCertificateContextProperty
	    (pCertContext, dwPropId, pvData, &cbData)) {
		handleError("Call #2 failed.");
	}

	for (size_t i = 0; i < cbData; i++) {
		printf("0x%02x,", pvData[i]);
	}
	printf("\n");

	free(pvData);
}

// Get and display the name of the subject of the certificate.
void printName(PCCERT_CONTEXT pCertContext)
{
	TCHAR szNameString[256];

	if (CertGetNameString(pCertContext,
			      CERT_NAME_SIMPLE_DISPLAY_TYPE,
			      0, NULL, szNameString, 128)) {
		wprintf(L"Certificate: %s\n", szNameString);
	} else {
		handleError("CertGetName failed.");
	}

}

PCCERT_CONTEXT findCertificate(HCERTSTORE hSystemStore, unsigned char *hash)
{
	PCCERT_CONTEXT pCertContext = NULL;
	CRYPT_HASH_BLOB toFindData;

	toFindData.cbData = HASH_LEN;
	toFindData.pbData = hash;

	pCertContext = CertFindCertificateInStore(hSystemStore,
						  X509_ASN_ENCODING |
						  PKCS_7_ASN_ENCODING, 0,
						  CERT_FIND_HASH, &toFindData,
						  NULL);
	return pCertContext;
}

bool findJuurSK(void)
{
	HANDLE hSystemStore;
	PCCERT_CONTEXT pCertContext;
	bool ret;
	wchar_t *storeName = L"Root";
	unsigned char juurSKHash[] = { 0x40,0x9D,0x4B,0xD9,0x17,0xB5,0x5C,0x27,0xB6,0x9B,0x64,0xCB,0x98,0x22,0x44,0x0D,0xCD,0x09,0xB8,0x89 };

	hSystemStore = CertOpenSystemStore(NULL, storeName);
	if (!hSystemStore) {
		handleError("Error opening store");
	}

	wprintf(L"Accessing certificate store: %s\n", storeName);

	pCertContext = findCertificate(hSystemStore, juurSKHash);
	if (pCertContext) {
		printf("Found JUUR SK\n");
		printName(pCertContext);
		printHash(pCertContext);

		ret = true;
	} else {
		ret = false;
	}

	CertCloseStore(hSystemStore, 0);
	return ret;
}

void findRemoveCertificate(HCERTSTORE hSystemStore, unsigned char *hash)
{
	PCCERT_CONTEXT pCertContext;

	pCertContext = findCertificate(hSystemStore, hash);
	if (pCertContext) {
		printName(pCertContext);
		printHash(pCertContext);

		removeCertificate(pCertContext);
	}
}

void traverseStore(wchar_t *storeName)
{
	HANDLE hSystemStore;

	hSystemStore = CertOpenStore(CERT_STORE_PROV_SYSTEM_REGISTRY,
				     0,
				     NULL,
				     CERT_SYSTEM_STORE_LOCAL_MACHINE |
				     CERT_STORE_OPEN_EXISTING_FLAG, storeName);
	if (!hSystemStore) {
		handleError("Error opening store");
	}

	wprintf(L"Accessing certificate store: %s\n", storeName);

	for (int i = 0; i < SK_HASH_COUNT; i++)
		findRemoveCertificate(hSystemStore, skHashes[i]);

	for (int i = 0; i < HASH_COUNT; i++)
		findRemoveCertificate(hSystemStore, hashes[i]);

	CertCloseStore(hSystemStore, 0);
}

/*
void main()
{
	findJuurSK();
	traverseStore(L"CA");
}
*/
