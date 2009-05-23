var emailsLoaded = false;
var activeCardId = "";

function changePin1()
{ changePin( 1 ); }

function changePin2()
{ changePin( 2 ); }

function changePin( type )
{
	var oldVal=document.getElementById('pin' + type + 'OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert( _( 'PIN' + type + 'Enter' ) );
		document.getElementById('pin' + type + 'OldPin').focus();
		return;
	}		
	if ( !eval("esteidData.validatePin" + type + "(oldVal)") )
	{
		ret = eval("esteidData.getPin" + type + "RetryCount() ");
		if ( ret == 0 )
		{
			document.getElementById('pin' + type + 'OldPin').value = "";
			document.getElementById('pin' + type + 'NewPin').value = "";
			document.getElementById('pin' + type + 'NewPin2').value = "";
			alert( _("PIN" + type + "Blocked") );
			readCardData();
			setActive('cert','');
			return;
		}
		alert( _( 'PIN' + type + 'InvalidRetry' ).replace( /%d/, ret ) );
		document.getElementById('pin' + type + 'OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pin' + type + 'NewPin').value;
	var newVal2=document.getElementById('pin' + type + 'NewPin2').value;
	if (newVal==null || newVal == "") 
	{
		alert( _( 'PIN' + type + 'EnterNew' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}		
	//PIN1 length 4-12, PIN2 5-12
	if ( (type == 1 && (newVal.length<4 || newVal.length > 12) ) || 
		(type == 2 && (newVal.length<5 || newVal.length > 12) ) ) 
	{
		alert( _( 'PIN' + type + 'Length' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}
	//check date of birth and birth year inside pin
	if ( !esteidData.checkPin( newVal ) )
	{
		alert( _( 'PINCheck' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}
	if (newVal2==null || newVal2 == "" )
	{
		alert( _( 'PIN' + type + 'Retry' ) );
		document.getElementById('pin' + type + 'NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert( _( 'PIN' + type + 'Different' ) );
		document.getElementById('pin' + type + 'NewPin2').focus();
		return;		
	}
	if (eval("esteidData.changePin" + type + "(newVal, oldVal)"))
	{
		document.getElementById('pin' + type + 'OldPin').value = "";
		document.getElementById('pin' + type + 'NewPin').value = "";
		document.getElementById('pin' + type + 'NewPin2').value = "";
		alert( _( 'PIN' + type + 'Changed' ) );
		setActive('cert','');
	} else
		alert( _( 'PIN' + type + 'Unsuccess' ) );
}

function changePuk()
{
	var oldVal=document.getElementById('pukOldPin').value;
	if (oldVal==null || oldVal == "")
	{
		alert( _('PUKEnterOld') );
		document.getElementById('pukOldPin').focus();
		return;
	}		
	if ( !esteidData.validatePuk(oldVal) )
	{
		ret = esteidData.getPukRetryCount();
		if ( ret == 0 )
		{
			document.getElementById('pukOldPin').value = "";
			document.getElementById('pukNewPin').value = "";
			document.getElementById('pukNewPin2').value = "";
			alert( _("PUKBlocked") );
			readCardData();
			setActive('cert','');
			return;
		}
		alert( _('PUKInvalidRetry').replace( /%d/, ret ) );
		document.getElementById('pukOldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pukNewPin').value;
	var newVal2=document.getElementById('pukNewPin2').value;
	if (newVal==null || newVal == "") 
	{
		alert( _('PUKEnterNew') );
		document.getElementById('pukNewPin').focus();
		return;
	}
	//PUK length 8-12
	if ( newVal.length<8 || newVal.length > 12 ) 
	{
		alert( _( 'PUKLength' ) );
		document.getElementById('pukNewPin').focus();
		return;
	}		
	
	if (newVal2==null || newVal2 == "")
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
		document.getElementById('pukOldPin').value = "";
		document.getElementById('pukNewPin').value = "";
		document.getElementById('pukNewPin2').value = "";
		alert( _('PUKChanged') );
		setActive('puk','');
	} else
		alert( _('PUKUnsuccess') );
}

function unblockPin1()
{ unblockPin( 1 ); }

function unblockPin2()
{ unblockPin( 2 ); }

function unblockPin( type )
{
	var oldVal=document.getElementById('bpin' + type + 'OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		alert( _('PUKEnter') );
		document.getElementById('bpin' + type + 'OldPin').focus();
		return;
	}		
	if ( !esteidData.validatePuk(oldVal) )
	{
		ret = esteidData.getPukRetryCount();
		if ( ret == 0 )
		{
			document.getElementById('bpin' + type + 'OldPin').value = "";
			document.getElementById('bpin' + type + 'NewPin').value = "";
			document.getElementById('bpin' + type + 'NewPin2').value = "";
			alert( _("PUKBlocked") );
			readCardData();
			setActive('cert','');
			return;
		}
		alert( _("PUKInvalidRetry").replace( /%d/, ret ) );
		document.getElementById('bpin' + type + 'OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('bpin' + type + 'NewPin').value;
	var newVal2=document.getElementById('bpin' + type + 'NewPin2').value;
	if (newVal==null || newVal == "") 
	{
		alert( _('PIN' + type + 'EnterNew') );
		document.getElementById('bpin' + type + 'NewPin').focus();
		return;
	}		
	//PIN1 length 4-12, PIN2 5-12
	if ( (type == 1 && (newVal.length<4 || newVal.length > 12) ) || 
		(type == 2 && (newVal.length<5 || newVal.length > 12) ) ) 
	{
		alert( _( 'PIN' + type + 'Length' ) );
		document.getElementById('bpin' + type + 'NewPin').focus();
		return;
	}
	//check date of birth and birth year inside pin
	if ( !esteidData.checkPin( newVal ) )
	{
		alert( _( 'PINCheck' ) );
		document.getElementById('bpin' + type + 'NewPin').focus();
		return;
	}
	if (newVal2==null || newVal2 == "")
	{
		alert( _('PIN' + type + 'Retry') );
		document.getElementById('bpin' + type + 'NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		alert( _('PIN' + type + 'Different') );
		document.getElementById('bpin' + type + 'NewPin2').focus();
		return;		
	}
	if (eval('esteidData.unblockPin' + type + '(newVal, oldVal)'))
	{
		document.getElementById('bpin' + type + 'OldPin').value = "";
		document.getElementById('bpin' + type + 'NewPin').value = "";
		document.getElementById('bpin' + type + 'NewPin2').value = "";
		alert( _('PIN' + type + 'UnblockSuccess') );
		readCardData();
		setActive('cert','');
	} else
		alert( _('PIN' + type + 'UnblockFailed') );
}

function cardInserted(i)
{
	//alert("Kaart sisestati lugejasse " + cardManager.getReaderName(i))
	checkReaderCount();
	if ( !cardManager.isInReader( activeCardId ) )
	{
		document.getElementById('cardInfoNoCard').style.display='none';	
		extender.showLoading( _('loadCardData') );
		activeCardId = "";
		emailsLoaded = false;
		cardManager.selectReader( i );
		if ( esteidData.canReadCard() )
			activeCardId = esteidData.getId();
		extender.closeLoading();
	}
	readCardData();
}

function cardRemoved(i)
{
	checkReaderCount();
	//alert("Kaart eemaldati lugejast " + cardManager.getReaderName(i));
	if ( !cardManager.isInReader( activeCardId ) )
	{
		activeCardId = "";
		emailsLoaded = false;
		cardManager.findCard();
	}
	readCardData();
}

function selectReader()
{
	extender.showLoading( _('loadCardData') );
	var select = document.getElementById('readerSelect'); 
	cardManager.selectReader( select.options[select.selectedIndex].value );
	if ( esteidData.canReadCard() )
		activeCardId = esteidData.getId();
	extender.closeLoading();
	readCardData();
}

function checkReaderCount()
{
	var cards = 0;
	var reader = document.getElementById( 'readerSelect' );
	while ( reader.options.length > 0 )
		reader.remove(0);
	for( var i = 0; i < cardManager.getReaderCount(); i++ )
	{
		if ( cardManager.isInReader( i ) )
		{
			var el = document.createElement( 'option' );
			el.text = cardManager.cardId( i );
			el.value = i;
			if ( activeCardId != "" && el.text == activeCardId )
				el.selected = true;
			reader.add( el, null );
			cards++;
		}
	}
	if ( cards < 2 )
	{
		document.getElementById( 'headerMenus' ).style.right = '90px';
		reader.style.display = 'none';
	} else {
		document.getElementById( 'headerMenus' ).style.right = '210px';
		reader.style.display = 'block';
	}
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
		alert( _('PUKBlocked') )
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
		case "email": if ( !emailsLoaded ) document.getElementById('emailsContentCheck').style.display = 'block'; break;
	}
}

function loadEmails()
{
	if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
		return;
	extender.showLoading( _('loadEmail') );
	extender.loadEmails();
}

function loadPicture()
{
	if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
		return;
	extender.showLoading( _('loadPic') );
	extender.loadPicture();
}

function setPicture( img, code )
{
	if ( code != "" )
		alert( _(code) );
	else
		document.getElementById('photo').innerHTML = "<img src=\"" + img + "\">";
	extender.closeLoading();
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
	extender.closeLoading();
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
	if ( emailsLoaded )
		document.getElementById('emailsContentCheck').style.display = 'none';
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
	document.getElementById('emailAddress').value = "";
}

function handleError(msg)
{
	if ( msg == "" )
		return;
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
	
	document.getElementById('emailsContent').innerHTML = "";
	document.getElementById('photo').innerHTML = '<div id="photoContent" style="padding-top:50px;"><a href="#" onClick="loadPicture();"><trtag trcode="loadPicture">' + _('loadPicture') + '</trtag></a></div>';
	
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
