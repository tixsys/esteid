
Dim ISign

Set ISign= CreateObject("EIDCard.Sign")

wscript.echo "getInfo(): " & ISign.getInfo

wscript.echo "Allkirjastav sert: " & ISign.getSigningCertificate

wscript.echo ISign.getSignedHash("010203040506070809000A0B0C0D0e0F01020304",ISign.SelectedCertNumber)

wscript.echo "SigningCertIssuer :" & ISign.SigningCertIssuer
wscript.echo "SigningCertSubject:" & ISign.SigningCertSubject
wscript.echo "SigningCertcount  :" & ISign.SigningCertCount
wscript.echo "SigningCSPName  :" & ISign.SigningCSPName
wscript.echo "SigningKeyContainerName:" & ISign.SigningKeyContainerName
