var emailsLoaded = false;
var activeCardId = "";

function changePin1()
{
	var oldVal=document.getElementById('pin1OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert( _( 'PIN1Enter' ) );
		document.getElementById('pin1OldPin').focus();
		return;
	}		
	if ( !esteidData.validatePin1(oldVal) )
	{
		ret = esteidData.getPin1RetryCount();
		alert( _( 'PIN1InvalidRetry' ).replace( /%d/, ret ) );
		if ( ret == 0 )
		{
			readCardData();
			setActive('cert','');
			return;
		}
		document.getElementById('pin1OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pin1NewPin').value;
	var newVal2=document.getElementById('pin1NewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert( _( 'PIN1EnterNew' ) );
		document.getElementById('pin1NewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert( _( 'PIN1Retry' ) );
		document.getElementById('pin1NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert( _( 'PIN1Different' ) );
		document.getElementById('pin1NewPin2').focus();
		return;		
	}
	if (esteidData.changePin1(newVal, oldVal))
	{
		alert( _( 'PIN1Changed' ) );
		setActive('cert','');
	} else
		alert( _( 'PIN1Unsuccess' ) );
}

function changePin2()
{
	var oldVal=document.getElementById('pin2OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert( _( 'PIN2Enter' ) );
		document.getElementById('pin2OldPin').focus();
		return;
	}		
	if ( !esteidData.validatePin2(oldVal) )
	{
		ret = esteidData.getPin2RetryCount();
		alert( _( 'PIN2InvalidRetry' ).replace( /%d/, ret ) );
		if ( ret == 0 )
		{
			readCardData();
			setActive('cert','');
			return;
		}
		document.getElementById('pin2OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pin2NewPin').value;
	var newVal2=document.getElementById('pin2NewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert( _( 'PIN2EnterNew' ) );
		document.getElementById('pin2NewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert( _( 'PIN2Retry' ) );
		document.getElementById('pin2NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert( _( 'PIN2Different' ) );
		document.getElementById('pin2NewPin2').focus();
		return;		
	}
	if (esteidData.changePin2(newVal, oldVal))
	{
		alert( _( 'PIN2Changed' ) );
		setActive('cert','');
	} else
		alert( _( 'PIN2Unsuccess' ) );
}

function changePuk()
{
	var oldVal=document.getElementById('pukOldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert( _('PUKEnterOld') );
		document.getElementById('pukOldPin').focus();
		return;
	}		
	if ( !esteidData.validatePuk(oldVal) )
	{
		ret = esteidData.getPukRetryCount();
		alert( _('PUKInvalidRetry').replace( /%d/, ret ) );
		if ( ret == 0 )
		{
			readCardData();
			setActive('cert','');
			return;
		}
		document.getElementById('pukOldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pukNewPin').value;
	var newVal2=document.getElementById('pukNewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert( _('PUKEnterNew') );
		document.getElementById('pukNewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert( _('PUKRetry') );
		document.getElementById('pukNewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert( _('PUKDifferent') );
		document.getElementById('pukNewPin2').focus();
		return;		
	}
	if (esteidData.changePuk(newVal, oldVal))
	{
		alert( _('PUKChanged') );
		setActive('puk','');
	} else
		alert( _('PUKUnsuccess') );
}

function unblockPin1()
{
	var oldVal=document.getElementById('bpin1OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert( _('PUKEnter') );
		document.getElementById('bpin1OldPin').focus();
		return;
	}		
	if ( !esteidData.validatePuk(oldVal) )
	{
		ret = esteidData.getPukRetryCount();
		alert( _("PUKInvalidRetry").replace( /%d/, ret ) );
		if ( ret == 0 )
		{
			readCardData();
			setActive('cert','');
			return;
		}
		document.getElementById('bpin1OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('bpin1NewPin').value;
	var newVal2=document.getElementById('bpin1NewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert( _('PIN1EnterNew') );
		document.getElementById('bpin1NewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert( _('PIN1Retry') );
		document.getElementById('bpin1NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert( _('PIN1Different') );
		document.getElementById('bpin1NewPin2').focus();
		return;		
	}
	if (esteidData.unblockPin1(newVal, oldVal))
	{
		alert( _('PIN1UnblockSuccess') );
		readCardData();
		setActive('cert','');
	} else
		alert( _('PIN1UnblockFailed') );
}

function unblockPin2()
{
	var oldVal=document.getElementById('bpin2OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert( _('PUKEnter') );
		document.getElementById('bpin2OldPin').focus();
		return;
	}		
	if ( !esteidData.validatePuk(oldVal) )
	{
		ret = esteidData.getPukRetryCount();
		alert( _("PUKInvalidRetry").replace( /%d/, ret ) );
		if ( ret == 0 )
		{
			readCardData();
			setActive('cert','');
			return;
		}
		document.getElementById('bpin2OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('bpin2NewPin').value;
	var newVal2=document.getElementById('bpin2NewPin2').value;
	if (newVal==null || newVal.length<4) 
	{
		alert( _('PIN2EnterNew') );
		document.getElementById('bpin2NewPin').focus();
		return;
	}		
	if (newVal2==null || newVal2.length<4)
	{
		alert( _('PIN2Retry') );
		document.getElementById('bpin2NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert( _('PIN2NewDifferent') );
		document.getElementById('bpin2NewPin2').focus();
		return;		
	}
	if (esteidData.unblockPin2(newVal, oldVal))
	{
		alert( _('PIN2UnblockSuccess') );
		readCardData();
		setActive('cert','');
	} else
		alert( _('PIN2UnblockFailed') );
}

function cardInserted(i)
{
	//alert("Kaart sisestati lugejasse " + cardManager.getReaderName(i))
	if ( !cardManager.isInReader( activeCardId ) )
	{
		activeCardId = "";
		emailsLoaded = false;
		cardManager.selectReader( i );
		if ( esteidData.canReadCard() )
			activeCardId = esteidData.getId();
	}
	readCardData();
}

function cardRemoved(i)
{
	//alert("Kaart eemaldati lugejast " + cardManager.getReaderName(i));
	if ( !cardManager.isInReader( activeCardId ) )
	{
		activeCardId = "";
		emailsLoaded = false;
	}
	if ( cardManager.getReaderCount() > 0 )
		cardManager.findCard();
	readCardData();
}

function readCardData()
{
	if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
	{
		disableFields();
		return;
	} else
		enableFields();
	if ( activeCardId == "" )
		activeCardId = esteidData.getId();
		
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

	if ( esteidData.getPin1RetryCount() != 0 )
	{
		document.getElementById('authCertStatus').className=esteidData.authCert.isValid() ? 'statusValid' : 'statusBlocked';
		document.getElementById('authCertStatus').innerHTML=_( esteidData.authCert.isValid() ? 'certValid' : 'certBlocked' );
		document.getElementById('authKeyText').style.display='block';
		document.getElementById('authKeyBlocked').style.display='none';
		document.getElementById('authValidButtons').style.display='block';
		document.getElementById('authBlockedButtons').style.display='none';
	} else {
		document.getElementById('authCertStatus').className='statusBlocked';
		document.getElementById('authCertStatus').innerHTML=_('validBlocked');
		document.getElementById('authKeyText').style.display='none';
		document.getElementById('authKeyBlocked').style.display='block';
		document.getElementById('authValidButtons').style.display='none';
		document.getElementById('authBlockedButtons').style.display='block';
	}
	document.getElementById('authCertValidTo').className=(esteidData.authCert.isValid() ? 'certValid' : 'certBlocked');

	if (esteidData.getPin2RetryCount() != 0 )
	{
		document.getElementById('signCertStatus').className=esteidData.signCert.isValid() ? 'statusValid' : 'statusBlocked';
		document.getElementById('signCertStatus').innerHTML=_( esteidData.signCert.isValid() ? 'certValid' : 'certBlocked' );
		document.getElementById('signKeyText').style.display='block';
		document.getElementById('signKeyBlocked').style.display='none';
		document.getElementById('signValidButtons').style.display='block';
		document.getElementById('signBlockedButtons').style.display='none';
	} else {
		document.getElementById('signCertStatus').className='statusBlocked';
		document.getElementById('signCertStatus').innerHTML=_('validBlocked');
		document.getElementById('signKeyText').style.display='none';
		document.getElementById('signKeyBlocked').style.display='block';
		document.getElementById('signValidButtons').style.display='none';
		document.getElementById('signBlockedButtons').style.display='block';
	}
	document.getElementById('signCertValidTo').className=(esteidData.signCert.isValid() ? 'certValid' : 'certBlocked');

	if(esteidData.getPukRetryCount() == 0)
		alert( _('PUKLocked') )
}

function setActive( content, el )
{
	if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
		return;

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
		if ( typeof divs[i].style == "undefined" )
			continue;
		if ( divs[i].id.indexOf( content ) )
			divs[i].style.display = 'none';
		else
			divs[i].style.display = 'block';
	}

	switch( content )
	{
		case "cpuk": document.getElementById('pukOldPin').focus(); break;
		case "pin1": document.getElementById('pin1OldPin').focus(); break;
		case "pin2": document.getElementById('pin2OldPin').focus(); break;
		case "bpin1": document.getElementById('bpin1OldPin').focus(); break;
		case "bpin2": document.getElementById('bpin2OldPin').focus(); break;
		case "email": if ( !emailsLoaded ) loadEmails(); break;
	}
}

function loadEmails()
{
	if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
		return;
	document.getElementById('loading').style.display = 'block';
	document.getElementById('loading').innerHTML = _("loadEmail");
	extender.loadEmails();
}

function loadPicture()
{
	if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
		return;
	document.getElementById('loading').style.display = 'block';
	document.getElementById('loading').innerHTML = _("loadPic");
	extender.loadPicture();
}

function setPicture( img, code )
{
	if ( code != "" )
		alert( _(code) );
	else
		document.getElementById('photo').innerHTML = "<img src=\"" + img + "\">";
	document.getElementById('loading').style.display = 'none';
}

function setEmailActivate( msg )
{
	//emails activated, lets load again
	if ( msg == "0" )
	{
		document.getElementById('emailsContentActivate').style.display = "none"
		extender.loadEmails();
		return;
	}
	document.getElementById('emailsContent').innerHTML = _(msg);
}

function setEmails( code, msg )
{
	document.getElementById('loading').style.display = 'none';
	if ( code == "0" && msg.indexOf( ' - ' ) == -1 )
		code = "20";
	//not activated
	if ( code == "20" )
		document.getElementById('emailsContentActivate').style.display = "block";
	//success
	if ( code != "0" && code != "20" )
		alert( _(code) );
	else {
		if ( code == "0" )
			code = msg;
		document.getElementById('emailsContent').innerHTML = _(code);
	}
	if ( code != "loadFailed" )
		emailsLoaded = true;	
}

function activateEmail()
{
	if ( document.getElementById('emailAddress').value == "" )
	{
		alert( _('emailEnter') );
		document.getElementById('emailAddress').focus();
		return;
	}
	extender.activateEmail( document.getElementById('emailAddress').value );
}

function handleError(msg)
{
	if ( msg == "PIN1InvalidRetry" )
	{
		var ret = esteidData.getPin1RetryCount();
		alert( _( msg ).replace( /%d/, ret ) );
		if ( ret == 0 )
			readCardData();
		return;
	}
	alert( _('errorFound') + _( msg ) )
}

function disableFields()
{
	document.getElementById('documentId').innerHTML = "";
	document.getElementById('expiry').innerHTML = "";
	document.getElementById('firstName').innerHTML = "";
	document.getElementById('middleName').innerHTML = "";
	document.getElementById('surName').innerHTML = "";
	document.getElementById('id').innerHTML = "";
	document.getElementById('birthDate').innerHTML = "";
	document.getElementById('birthPlace').innerHTML = "";
	document.getElementById('citizen').innerHTML = "";
	document.getElementById('email').innerHTML = "";

	document.getElementById('cardInfo').style.display='none';
	document.getElementById('cardInfoNoCard').style.display='block';
	document.getElementById('cardInfoNoCardText').innerHTML=_( cardManager.getReaderCount() == 0 ? 'noReaders' : 'noCard' );
	var divs = document.getElementsByClassName('content');
	for( var i in divs )
		if ( typeof divs[i] != "undefined" && typeof divs[i].style != "undefined" )
			divs[i].style.display = 'none';
}

function enableFields()
{
	document.getElementById('cardInfo').style.display='block';
	document.getElementById('cardInfoNoCard').style.display='none';
	var buttons = document.getElementById('leftMenus').getElementsByTagName('input');
	var selected = "";
	for( var i in buttons )
		if ( !buttons[i].className.indexOf( "buttonSelected" ) )
		{
			selected = buttons[i].name;
			break;
		}
	if ( selected != "" )
	{
		var divs = document.getElementsByClassName('content');
		for( var i in divs )
		{
			if ( typeof divs[i].id == "undefined" )
				continue;
			if ( !divs[i].id.indexOf( selected ) )
				divs[i].style.display = 'block';
		}
	}
}
