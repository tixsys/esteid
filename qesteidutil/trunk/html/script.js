function changePin1()
{
	var oldVal=prompt("Vana PIN1:");
	if (oldVal!=null && oldVal.length >= 4)
	{
		if ( !esteidData.validatePin1(oldVal) )
		{
			ret = esteidData.getPin1RetryCount();
			alert("Vale PIN1. Saad veel proovida " + ret + " korda.");
			if ( ret == 0 )
				readCardData();
			return;
		}
	} else
		return;
	var newVal=prompt("Uus PIN1:");
	if (newVal!=null && newVal.length>=4) 
	{
		if (esteidData.changePin1(newVal, oldVal))
			alert("PIN1 kood muudetud!");
		else
			alert("PIN1 muutmine eba�nnestus.");
	}
}

function changePin2()
{
	var oldVal=prompt("Vana PIN2:");
	if (oldVal!=null && oldVal.length >= 4)
	{
		if ( !esteidData.validatePin2(oldVal) )
		{
			ret = esteidData.getPin2RetryCount();
			alert("Vale PIN2. Saad veel proovida " + ret + " korda.");
			if ( ret == 0 )
				readCardData();
			return;
		}
	} else
		return;
	var newVal=prompt("Uus PIN2:");
	if (newVal!=null && newVal.length>=4)
	{
		if (esteidData.changePin2(newVal, oldVal))
			alert("PIN2 kood muudetud!");
		else
			alert("PIN2 muutmine eba�nnestus.");
	}
}

function changePuk()
{
	var oldVal=prompt("Vana PUK:");
	if (oldVal!=null && oldVal.length>=4)
	{
		if ( !esteidData.validatePuk(oldVal) )
		{
			ret = esteidData.getPukRetryCount();
			alert("Vale PUK. Saad veel proovida " + ret + " korda.");
			if ( ret == 0 )
				readCardData();
			return;
		}
	} else
		return;
	var newVal=prompt("Uus PUK:");
	if (newVal!=null && newVal.length>=4)
	{
		ret = esteidData.changePuk(newVal, oldVal);
		if (ret)
		{
			alert("PUK kood muudetud!");
			readCardData();
		} else 
			alert("PUK koodi muutmine eba�nnestus.");
	}
}

function unblockPin1()
{
	var puk=prompt("PUK:");
	if (puk!=null && puk.length>=4)
	{
		if ( !esteidData.validatePuk(puk) )
		{
			ret = esteidData.getPukRetryCount();
			alert("Vale PUK. Saad veel proovida " + ret + " korda.");
			if ( ret == 0 )
				readCardData();
			return;
		}
	} else
		return;
	var newVal=prompt("Uus PIN1:");
	if (newVal!=null && newVal.length>=4)
	{
		ret = esteidData.unblockPin1(newVal, puk);
		if (ret)
		{
			alert("PIN1 kood muudetud!");
			readCardData();
		} else
			alert("Blokeeringu t�histamine eba�nnestus.");
	}
}

function unblockPin2()
{
	var puk=prompt("PUK:");
	if (puk!=null && puk.length>=4)
	{
		if ( !esteidData.validatePuk(puk) )
		{
			ret = esteidData.getPukRetryCount();
			alert("Vale PUK. Saad veel proovida " + ret + " korda.");
			if ( ret == 0 )
				readCardData();
			return;
		}
	} else
		return;
	var newVal=prompt("Uus PIN1:");
	if (newVal!=null && newVal.length>=4)
	{
		ret = esteidData.unblockPin2(newVal, puk);
		if (ret)
		{
			alert("PIN2 kood muudetud!");
			readCardData();
		} else
			alert("Blokeeringu t�histamine eba�nnestus.");
	}
}

function cardInserted(i)
{
	alert("Kaart sisestati lugejasse " + cardManager.getReaderName(i))
	cardManager.selectReader(i);
	readCardData();
}

function cardRemoved(i)
{
	alert("Kaart eemaldati lugejast " + cardManager.getReaderName(i))
}

function handleError(msg)
{
	alert("Tekkis viga: " + msg)
}

function readCardData()
{
	document.getElementById('documentId').innerHTML = esteidData.getDocumentId();
	document.getElementById('expiry').innerHTML = esteidData.getExpiry();
	document.getElementById('firstName').innerHTML = esteidData.getFirstName();
	document.getElementById('middleName').innerHTML = esteidData.getMiddleName();
	document.getElementById('surName').innerHTML = esteidData.getSurName();
	document.getElementById('id').innerHTML = esteidData.getId();
	document.getElementById('birthDate').innerHTML = esteidData.getBirthDate();
	document.getElementById('birthPlace').innerHTML = esteidData.getBirthPlace();
	document.getElementById('citizen').innerHTML = esteidData.getCitizen();
	document.getElementById('email').innerHTML = esteidData.authCert.getEmail();

	document.getElementById('authCertValidTo').innerHTML = esteidData.authCert.getValidTo();
	document.getElementById('authKeyUsage').innerHTML = esteidData.getAuthUsageCount();

	document.getElementById('signCertValidTo').innerHTML = esteidData.signCert.getValidTo();
	document.getElementById('signKeyUsage').innerHTML = esteidData.getSignUsageCount();

	if(cardManager.getReaderCount() == 0)
		alert("�htegi kiipkaardi lugejat pole �hendatud.")

	if ( esteidData.getPin1RetryCount() != 0 )
	{
		document.getElementById('authCertStatus').className='statusValid';
		document.getElementById('authCertStatus').innerHTML='kehtiv ja kasutatav';
		document.getElementById('authKeyText').style.visibility='visible';
		document.getElementById('authKeyBlocked').style.visibility='hidden';
		document.getElementById('authValidButtons').style.visibility='visible';
		document.getElementById('authBlockedButtons').style.visibility='hidden';
	} else {
		document.getElementById('authCertStatus').className='statusBlocked';
		document.getElementById('authCertStatus').innerHTML='kehtiv kuid blokeeritud';
		document.getElementById('authKeyText').style.visibility='hidden';
		document.getElementById('authKeyBlocked').style.visibility='visible';
		document.getElementById('authValidButtons').style.visibility='hidden';
		document.getElementById('authBlockedButtons').style.visibility='visible';
	}
	document.getElementById('authCertValidTo').className=(esteidData.authCert.isValid() ? 'certValid' : 'certBlocked');

	if (esteidData.getPin2RetryCount() != 0 )
	{
		document.getElementById('signCertStatus').className='statusValid';
		document.getElementById('signCertStatus').innerHTML='kehtiv ja kasutatav';
		document.getElementById('signKeyText').style.visibility='visible';
		document.getElementById('signKeyBlocked').style.visibility='hidden';
		document.getElementById('signValidButtons').style.visibility='visible';
		document.getElementById('signBlockedButtons').style.visibility='hidden';
	} else {
		document.getElementById('signCertStatus').className='statusBlocked';
		document.getElementById('signCertStatus').innerHTML='kehtiv kuid blokeeritud';
		document.getElementById('signKeyText').style.visibility='hidden';
		document.getElementById('signKeyBlocked').style.visibility='visible';
		document.getElementById('signValidButtons').style.visibility='hidden';
		document.getElementById('signBlockedButtons').style.visibility='visible';
	}
	document.getElementById('signCertValidTo').className=(esteidData.signCert.isValid() ? 'certValid' : 'certBlocked');

	if(esteidData.getPukRetryCount() == 0)
		alert("PUK on lukus.")
}
