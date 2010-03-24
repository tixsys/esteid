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

void handleError(char *s);
void removeCertificate(PCCERT_CONTEXT pCertContext);
void printHash(PCCERT_CONTEXT pCertContext);
void printName(PCCERT_CONTEXT pCertContext);
PCCERT_CONTEXT findCertificate(HCERTSTORE hSystemStore, unsigned char *hash);
bool findJuurSK(void);
void findRemoveCertificate(HCERTSTORE hSystemStore, unsigned char *hash);
void traverseStore(wchar_t * storeName);
