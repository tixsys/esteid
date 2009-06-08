#include <string.h>

#include <util/String.h>

#include "SignatureBESTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(digidoc::SignatureBESTest);

void digidoc::SignatureBESTest::setUp()
{
}

void digidoc::SignatureBESTest::testFailingSignatures()
{
	// XMl file tests
	//   1. Tühi fail
	//   2. Kataloog
	//   3. Mitte XML fail
	//   4. XML fail, kuid mitte Signature XML fail
	//   5. Korrektne BES signature fail, kui namespace on defineerimata (ds: on igal pool ees,
	//     kuid üleval pole defineeritud seda ehk puudub atribuut xmlns:ds="http://www.w3.org/....")

	// Signature->SignedInfo tests
	//   1. SignedInfo tag on puudu
	// Signature->SignedInfo->CanonicalizationMethod tests
	//   2. CanonicalizationMethod on puudu Signature->SignedInfo seest
	//   3. Algorithm atribuut on puudu Signature->SignedInfo->CanonicalizationMethod tagil
	//   4. Algorithm atribuudis ei ole korrektne C14N väärtus
	// Signature->SignedInfo->SignatureMethod tests
	//   5. SignatureMethod on puudu Signature->SignedInfo seest
	//   6. Algorithm atribuut on puudu Signature->SignedInfo->SignatureMethod tagil
	//   7. Algorithm atribuudis on tundmatu signature tüübi väärtus DSA-SHA1 (http://www.w3.org/2000/09/xmldsig#dsa-sha1)
	// Signature->SignedInfo->Reference tests (nendes testides on vaja ette anda bdoc klass, mis sisaldab viiteid failidele, mis BDOC'is on)
	//   8. ühtegi Reference elementi pole Signature->SignedInfo sees
	//   9. üks Reference element, kuid sellel puudub URI atribuut
	//  10. üks Reference element, kuid URI atribuut väärtus näitab failile, mida BDOC BES conteineris pole
	//  11. üks Reference element, kuid URI atribuudis on '/' faili nime algusest puudu (fail ise on olemas)
	// Signature->SignedInfo->Reference->DigestMethod tests
	//  12. üks Reference element, kuid selle seest on puudu tag DigestMethod
	//  13. üks Reference element, kuid DigestMethod tagil on Algorithm atribuut puudu
	//  14. üks Reference element, kuid DigestMethod tagi Algorithm atribuut ei ole toetatud,
	//      SHA 384 (http://www.w3.org/2001/04/xmldsig-more#sha384)
	// Signature->SignedInfo->Reference->DigestValue tests
	//  15. üks Reference element, kuid selle seest on puudu tag DigestValue
	//  16. üks Reference element, kuid DigestValue väärtus on tühi
	//  17. üks Reference element, kuid DigestValue väärtus ei ole base 64 väärtus (sisaldab tähti, mida base64 ei tohi sisaldada, kasvõi öäüõ)
	//  18. üks Reference element, kuid DigestValue väärtus ei ole korrektne, ei vasta faili tegelikule digestile
	//  19. Reference elemente on rohkem, kui BDoc klassi küljes faile
	//  20. Reference elemente on vähem, kui BDoc klassi küljes faile

	// Signature->SignatureValue tests
	//   1. SignatureValue on puudu
	//   2. SignatureValue on tühi
	//   3. SignatureValue väärtus ei ole base 64 väärtus (sisaldab tähti, mida base64 ei tohi sisaldada, kasvõi öäüõ)
	//   4. SignatureValue väärtus ei ole korrektne, ei vasta Signature->SignedInfo bloki digestile

	// Signature->KeyInfo tests
	//   1. KeyInfo on puudu
	// Signature->KeyInfo->KeyValue tests
	//   2. KeyValue on puudu
	//   3. KeyValue->RSAKeyValue on puudu
	//   4. KeyValue->RSAKeyValue->Modulus on puudu
	//   5. KeyValue->RSAKeyValue->Modulus väärtus on tühi
	//   6. KeyValue->RSAKeyValue->Modulus väärtus ei ole base 64 väärtus (sisaldab tähti, mida base64 ei tohi sisaldada, kasvõi öäüõ)
	//   7. KeyValue->RSAKeyValue->Modulus väärtus ei ei vasta Signature->KeyInfo->X509Data->X509Certificate seest parsitud X509'le
	//   8. KeyValue->RSAKeyValue->Exponent on puudu
	//   9. KeyValue->RSAKeyValue->Exponent väärtus on tühi
	//  10. KeyValue->RSAKeyValue->Exponent väärtus ei ole base 64 väärtus (sisaldab tähti, mida base64 ei tohi sisaldada, kasvõi öäüõ)
	//  11. KeyValue->RSAKeyValue->Exponent väärtus ei ei vasta Signature->KeyInfo->X509Data->X509Certificate seest parsitud X509'le
	// Signature->KeyInfo->X509Data tests
	//  12. X509Data on puudu
	//  13. X509Data->X509Certificate on puudu
	//  14. X509Data->X509Certificate väärtus on tühi
	//  15. X509Data->X509Certificate väärtus ei ole base 64 väärtus (sisaldab tähti, mida base64 ei tohi sisaldada, kasvõi öäüõ)
	//  16. X509Data->X509Certificate väärtus ei ei X509 formaadis cert
	//  17. X509Data->X509Certificate on korrektne X509, kuid CA on tundmatu (pole X509 cert stores)

	// Signature->Object tests

}
