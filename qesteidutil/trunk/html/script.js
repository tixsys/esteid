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
			alert("PIN1 muutmine ebaõnnestus.");
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
			alert("PIN2 muutmine ebaõnnestus.");
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
			alert("PUK koodi muutmine ebaõnnestus.");
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
			alert("Blokeeringu tühistamine ebaõnnestus.");
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
			alert("Blokeeringu tühistamine ebaõnnestus.");
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
		alert("Ühtegi kiipkaardi lugejat pole ühendatud.")

	if ( esteidData.getPin1RetryCount() != 0 )
	{
		document.getElementById('authCertStatus').className='statusValid';
		document.getElementById('authCertStatus').innerHTML='kehtiv ja kasutatav';
		document.getElementById('authKeyText').style.display='block';
		document.getElementById('authKeyBlocked').style.display='none';
		document.getElementById('authValidButtons').style.display='block';
		document.getElementById('authBlockedButtons').style.display='none';
	} else {
		document.getElementById('authCertStatus').className='statusBlocked';
		document.getElementById('authCertStatus').innerHTML='kehtiv kuid blokeeritud';
		document.getElementById('authKeyText').style.display='none';
		document.getElementById('authKeyBlocked').style.display='block';
		document.getElementById('authValidButtons').style.display='none';
		document.getElementById('authBlockedButtons').style.display='block';
	}
	document.getElementById('authCertValidTo').className=(esteidData.authCert.isValid() ? 'certValid' : 'certBlocked');

	if (esteidData.getPin2RetryCount() != 0 )
	{
		document.getElementById('signCertStatus').className='statusValid';
		document.getElementById('signCertStatus').innerHTML='kehtiv ja kasutatav';
		document.getElementById('signKeyText').style.display='block';
		document.getElementById('signKeyBlocked').style.display='none';
		document.getElementById('signValidButtons').style.display='block';
		document.getElementById('signBlockedButtons').style.display='none';
	} else {
		document.getElementById('signCertStatus').className='statusBlocked';
		document.getElementById('signCertStatus').innerHTML='kehtiv kuid blokeeritud';
		document.getElementById('signKeyText').style.display='none';
		document.getElementById('signKeyBlocked').style.display='block';
		document.getElementById('signValidButtons').style.display='none';
		document.getElementById('signBlockedButtons').style.display='block';
	}
	document.getElementById('signCertValidTo').className=(esteidData.signCert.isValid() ? 'certValid' : 'certBlocked');

	if(esteidData.getPukRetryCount() == 0)
		alert("PUK on lukus.")
}

function setActive( content, el )
{
	var buttons = document.getElementById('leftMenus').getElementsByTagName('input');
	for( var i in buttons )
		buttons[i].className = 'button';
	el.className = 'buttonSelected';
	
	var divs = document.getElementsByClassName('content');
	for( var i in divs )
	{
		if ( divs[i].id.indexOf( content ) )
			divs[i].style.display = 'none';
		else
			divs[i].style.display = 'block';
	}
}
