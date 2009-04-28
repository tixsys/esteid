function changePin1()
{
	var oldVal=document.getElementById('pin1OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert('Sisesta vana PIN1 kood');
		document.getElementById('pin1OldPin').focus();
		return;
	}		
	if ( !esteidData.validatePin1(oldVal) )
	{
		ret = esteidData.getPin1RetryCount();
		alert("Vale PIN1 kood. Saad veel proovida " + ret + " korda.");
		if ( ret == 0 )
			readCardData();
		document.getElementById('pin1OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pin1NewPin').value;
	var newVal2=document.getElementById('pin1NewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert('Sisesta uus PIN1 kood');
		document.getElementById('pin1NewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert('Korda uut PIN1 koodi');
		document.getElementById('pin1NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert('Uued PIN1 koodid on erinevad');
		document.getElementById('pin1NewPin2').focus();
		return;		
	}
	if (esteidData.changePin1(newVal, oldVal))
	{
		alert("PIN1 kood muudetud!");
		setActive('cert','');
	} else
		alert("PIN1 muutmine ebaõnnestus.");
}

function changePin2()
{
	var oldVal=document.getElementById('pin2OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert('Sisesta vana PIN2 kood');
		document.getElementById('pin2OldPin').focus();
		return;
	}		
	if ( !esteidData.validatePin2(oldVal) )
	{
		ret = esteidData.getPin2RetryCount();
		alert("Vale PIN2 kood. Saad veel proovida " + ret + " korda.");
		if ( ret == 0 )
			readCardData();
		document.getElementById('pin2OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pin2NewPin').value;
	var newVal2=document.getElementById('pin2NewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert('Sisesta uus PIN2 kood');
		document.getElementById('pin2NewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert('Korda uut PIN2 koodi');
		document.getElementById('pin2NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert('Uued PIN2 koodid on erinevad');
		document.getElementById('pin2NewPin2').focus();
		return;		
	}
	if (esteidData.changePin2(newVal, oldVal))
	{
		alert("PIN2 kood muudetud!");
		setActive('cert','');
	} else
		alert("PIN2 muutmine ebaõnnestus.");
}

function changePuk()
{
	var oldVal=document.getElementById('pukOldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert('Sisesta vana PUK kood');
		document.getElementById('pukOldPin').focus();
		return;
	}		
	if ( !esteidData.validatePuk(oldVal) )
	{
		ret = esteidData.getPukRetryCount();
		alert("Vale PUK kood. Saad veel proovida " + ret + " korda.");
		if ( ret == 0 )
			readCardData();
		document.getElementById('pukOldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pukNewPin').value;
	var newVal2=document.getElementById('pukNewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert('Sisesta uus PUK kood');
		document.getElementById('pukNewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert('Korda uut PUK koodi');
		document.getElementById('pukNewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert('Uued PUK koodid on erinevad');
		document.getElementById('pukNewPin2').focus();
		return;		
	}
	if (esteidData.changePuk(newVal, oldVal))
	{
		alert("PUK kood muudetud!");
		setActive('puk','');
	} else
		alert("PUK koodi muutmine ebaõnnestus.");
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
	if ( el != '' )
	{
		var buttons = document.getElementById('leftMenus').getElementsByTagName('input');
		for( var i in buttons )
			buttons[i].className = 'button';
		el.className = 'buttonSelected';
	}
	
	var divs = document.getElementsByClassName('content');
	for( var i in divs )
	{
		if ( divs[i].id.indexOf( content ) )
			divs[i].style.display = 'none';
		else
			divs[i].style.display = 'block';
	}
}
