var emailsLoaded = false;
var activeCardId = "";

document.onkeyup = checkAccessKeys;

function checkAccessKeys(e)
{
	if ( !e.altKey )
		return;
	var letter = String.fromCharCode(e.keyCode);
	
	var buttons = document.getElementById('leftMenus').getElementsByTagName('input');
	for( i=0;i<buttons.length;i++)
	{
		if ( (typeof buttons[i].attributes["accesskey"] != "undefined") && buttons[i].attributes["accesskey"].value.indexOf(letter) >= 0 )
		{
			buttons[i].focus();
			buttons[i].click();
			return;
		}
	}	
	var ahrefs = document.getElementById('headerMenus').getElementsByTagName('a');
	for( i=0;i<ahrefs.length;i++)
	{
		if ( (typeof ahrefs[i].attributes["accesskey"] != "undefined") && ahrefs[i].attributes["accesskey"].value.indexOf(letter) >= 0 )
		{
			ahrefs[i].focus();
			ahrefs[i].onclick();
			return;
		}
	}		
}

function checkNumeric(e)
{
	if ( e.keyCode == 13 || e.keyCode == 9 || e.keyCode == 8 || e.keyCode == 0 || String.fromCharCode(e.keyCode).match(/[\d]/) )
		return true;
	return false;
}

function handlekey(nextItem)
{
	if ( window.event.keyCode != 13 && window.event.keyCode != 9 )
		return;

	var el = document.getElementById(nextItem)
	if ( (typeof el == "undefined") || (typeof el.type == "undefined") )
		return;
	
	if ( el.type == "password" )
		el.focus();
	else if ( el.type == "button" )
		el.click();
}

function checkEnter( e, obj )
{
	if ( e.keyCode == 13 && ( typeof obj.click != "undefined" ) && obj.click )
		obj.click();
}

function cardInserted(i)
{
	//alert("Kaart sisestati lugejasse " + cardManager.getReaderName(i))
	checkReaderCount();
	setActive('cert',document.getElementById('buttonCert'));

	var inReader = false;
	try {
		if ( !cardManager.isInReader( activeCardId ) )
		{
			document.getElementById('cardInfoNoCard').style.display='none';
			activeCardId = "";
			emailsLoaded = false;
			if ( i != -1 )
			{
				extender.showLoading( _('loadCardData') );
				var retry = 3;
				while( retry > 0 )
				{
					cardManager.selectReader( i );
					if ( esteidData.canReadCard() )
					{
						activeCardId = esteidData.getDocumentId();
						break;
					}
					retry--;
				}
				if ( activeCardId != '' || !cardManager.anyCardsInReader() )
					extender.closeLoading();
			}
		} else
			inReader = true;
	} catch ( err ) {}

	if ( activeCardId == "" && !cardManager.anyCardsInReader() )
		document.getElementById('cardInfoNoCard').style.display='block';
		
	document.getElementById( 'forUpdate' ).innerHTML += ".";
	if ( !inReader && activeCardId != '' )
		readCardData();
}

function cardRemoved(i)
{
	//alert("Kaart eemaldati lugejast " + cardManager.getReaderName(i) + " " + activeCardId );
	checkReaderCount();
	setActive('cert',document.getElementById('buttonCert'));
	
	var inReader = false;
	try {
		if ( !cardManager.isInReader( activeCardId ) )
		{
			if ( cardManager.anyCardsInReader() )
				extender.showLoading( _('loadCardData') );
			emailsLoaded = false;
			activeCardId = "";
			disableFields();
			var retry = 3;
			while( retry > 0 )
			{
				cardManager.findCard();
				if ( esteidData.canReadCard() )
				{
					activeCardId = esteidData.getDocumentId();
					break;
				}
				retry--;
			}
		} else
			inReader = true;
	} catch( err ) {}

	extender.closeLoading();
	document.getElementById( 'forUpdate' ).innerHTML += ".";
	if ( !inReader )
		readCardData();
}

function selectReader()
{
	extender.showLoading( _('loadCardData') );
	disableFields();
	var select = document.getElementById('readerSelect'); 

	try {
		cardManager.selectReader( select.options[select.selectedIndex].value );
	} catch( err ) {}

	if ( esteidData.canReadCard() )
		activeCardId = esteidData.getDocumentId();

	extender.closeLoading();
	document.getElementById( 'forUpdate' ).innerHTML += ".";
	readCardData();
}

function checkReaderCount()
{
	var cards = 0;
	var reader = document.getElementById( 'readerSelect' );
	while ( reader.options.length > 0 )
		reader.remove(0);

	try {
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
	} catch ( err ) {}

	if ( cards < 2 )
	{
		document.getElementById( 'headerMenus' ).style.right = '100px';
		reader.style.display = 'none';
	} else {
		document.getElementById( 'headerMenus' ).style.right = '190px';
		reader.style.display = 'block';
	}
}

function readCardData()
{
	try {
		if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
		{
			disableFields();
			return;
		} else
			enableFields();

		if ( activeCardId == "" )
			activeCardId = esteidData.getDocumentId();

		checkReaderCount();

		var pukRetry = esteidData.getPukRetryCount();
		var esteidIsValid = esteidData.isValid();
		
		document.getElementById('documentId').innerHTML = activeCardId;
		document.getElementById('expiry').innerHTML = esteidData.getExpiry( language );
		document.getElementById('email').innerHTML = esteidData.authCert.getEmail();
		document.getElementById('labelCardValidity').innerHTML = _( esteidIsValid ? 'labelIsValid' : 'labelIsInValid' );
		document.getElementById('labelCardValidity').style.color = esteidIsValid ? '#509b00' : '#e80303';

		if ( esteidData.authCert.isTempel() )
		{
			document.getElementById('photo').innerHTML = '<div style="padding-top:25px;"><img width="75" height="75" src="qrc:/stamp"></div>';
			document.getElementById('id').innerHTML = esteidData.authCert.getSerialNum();
			document.getElementById('personCode').innerHTML = _('regcode');
			document.getElementById('firstName').innerHTML = esteidData.authCert.getSubjCN();
			document.getElementById('liPersonBirth').style.display = 'none';
			document.getElementById('liPersonCitizen').style.display = 'none';
		} else {
			document.getElementById('personCode').innerHTML = _('personCode');
			document.getElementById('liPersonBirth').style.display = 'block';
			document.getElementById('liPersonCitizen').style.display = 'block';
			document.getElementById('firstName').innerHTML = esteidData.getFirstName();
			document.getElementById('middleName').innerHTML = esteidData.getMiddleName();
			document.getElementById('surName').innerHTML = esteidData.getSurName();
			document.getElementById('id').innerHTML = esteidData.getId();
			document.getElementById('birthDate').innerHTML = esteidData.getBirthDate( language );
			document.getElementById('birthPlace').innerHTML = (esteidData.getBirthPlace() != "" ? ", " + esteidData.getBirthPlace() : "" );
			document.getElementById('citizen').innerHTML = esteidData.getCitizen();
		}
		
		var pin1Retry = esteidData.getPin1RetryCount();
		var pin2Retry = esteidData.getPin2RetryCount();
		
		document.getElementById('authCertValidTo').innerHTML = esteidData.authCert.getValidTo( language );
		var days = esteidData.authCert.validDays();
		if ( days >= 0 && days <= 105 )
		{
			document.getElementById('authCertWillExpire').style.display = 'block';
			document.getElementById('authCertWillExpire').innerHTML = _( 'labelCertWillExpire' ).replace( /%d/, days );
		} else
			document.getElementById('authCertWillExpire').style.display = 'none';

		document.getElementById('authKeyUsage').innerHTML = esteidData.getAuthUsageCount();

		document.getElementById('signCertValidTo').innerHTML = esteidData.signCert.getValidTo( language );
		days = esteidData.signCert.validDays();
		if ( days >= 0 && days <= 105 )
		{
			document.getElementById('signCertWillExpire').style.display = 'block';
			document.getElementById('signCertWillExpire').innerHTML = _( 'labelCertWillExpire' ).replace( /%d/, days );
		} else
			document.getElementById('signCertWillExpire').style.display = 'none';
		document.getElementById('signKeyUsage').innerHTML = esteidData.getSignUsageCount();

		if ( pin1Retry != 0 )
		{
			document.getElementById('authCertStatus').className=esteidData.authCert.isValid() ? 'statusValid' : 'statusBlocked';
			document.getElementById('authCertStatus').innerHTML=_( esteidData.authCert.isValid() ? 'certValid' : 'certBlocked' );
			document.getElementById('authKeyText').style.display='block';
			document.getElementById('authKeyBlocked').style.display='none';
			document.getElementById('authValidButtons').style.display='block';
			document.getElementById('authBlockedButtons').style.display='none';
		} else {
			document.getElementById('authCertStatus').className='statusBlocked';
			document.getElementById('authCertStatus').innerHTML=_( esteidData.authCert.isValid() ? 'validBlocked' : 'invalidBlocked' );
			document.getElementById('authKeyText').style.display='none';
			document.getElementById('authKeyBlocked').style.display='block';
			document.getElementById('authValidButtons').style.display='none';
			document.getElementById('authBlockedButtons').style.display=(pukRetry == 0 ? 'none' : 'block');
		}
		document.getElementById('authCertValidTo').className=(esteidData.authCert.isValid() ? 'certValid' : 'certBlocked');
		
		if ( pin2Retry != 0 )
		{
			document.getElementById('signCertStatus').className=esteidData.signCert.isValid() ? 'statusValid' : 'statusBlocked';
			document.getElementById('signCertStatus').innerHTML=_( esteidData.signCert.isValid() ? 'certValid' : 'certBlocked' );
			document.getElementById('signKeyText').style.display='block';
			document.getElementById('signKeyBlocked').style.display='none';
			document.getElementById('signValidButtons').style.display='block';
			document.getElementById('signBlockedButtons').style.display='none';
		} else {
			document.getElementById('signCertStatus').className='statusBlocked';
			document.getElementById('signCertStatus').innerHTML=_( esteidData.signCert.isValid() ? 'validBlocked' : 'invalidBlocked' );
			document.getElementById('signKeyText').style.display='none';
			document.getElementById('signKeyBlocked').style.display='block';
			document.getElementById('signValidButtons').style.display='none';
			document.getElementById('signBlockedButtons').style.display=(pukRetry == 0 ? 'none' : 'block');;
		}
		document.getElementById('signCertValidTo').className=(esteidData.signCert.isValid() ? 'certValid' : 'certBlocked');

		//update cert button
		days = esteidData.authCert.validDays();
		if ( ( days <= 105 || esteidData.signCert.validDays() <= 105 ) && pin1Retry > 0 && esteidIsValid )
		{
			document.getElementById('authUpdateDiv').style.display='block';
			var width = 0;
			if ( document.getElementById('authValidButtons').style.display == 'block' )
				width = parseInt(document.defaultView.getComputedStyle(document.getElementById('authValidButtons1'), "").getPropertyValue('width')) + 
						parseInt(document.defaultView.getComputedStyle(document.getElementById('authValidButtons2'), "").getPropertyValue('width')) + 5;
			else if ( document.getElementById('authBlockedButtons').style.display == 'block' )
				width = parseInt(document.defaultView.getComputedStyle(document.getElementById('authBlockedButtons1'), "").getPropertyValue('width'));
			document.getElementById('authUpdateButton').style.width=width + 'px';
		} else
			document.getElementById('authUpdateDiv').style.display='none';

		if(pukRetry == 0)
			setActive('puk',document.getElementById('buttonPUK'));
	} catch ( err ) { }
}

function setActive( content, el )
{
	if ( el != '' )
	{
		if ( typeof el == 'string' )
			el = document.getElementById( el );
		var buttons = document.getElementById('leftMenus').getElementsByTagName('input');
		for( i=0;i<buttons.length;i++)
			buttons[i].className = 'button';
		el.className = 'buttonSelected';
	}
	
	if ( cardManager.getReaderCount() == 0 || !esteidData.canReadCard() )
		return;

	var divs = document.getElementsByTagName('div');
	for( i=0;i<divs.length;i++)
	{
		if ( (typeof divs[i].className == "undefined") || (typeof divs[i].style == "undefined") || divs[i].className != "content" )
			continue;
		if ( divs[i].id.indexOf( content ) )
			divs[i].style.display = 'none';
		else
			divs[i].style.display = 'block';
	}

	switch( content )
	{
		case "cpuk":
			document.getElementById('pukOldPin').value = "";
			document.getElementById('pukNewPin').value = "";
			document.getElementById('pukNewPin2').value = "";
			document.getElementById('pukOldPin').focus();
			break;
		case "pin1":
			document.getElementById('pin1OldPin').value = "";
			document.getElementById('pin1NewPin').value = "";
			document.getElementById('pin1NewPin2').value = "";
			document.getElementById('pin1OldPin').focus();
			break;
		case "ppin1":
			document.getElementById('ppin1OldPin').value = "";
			document.getElementById('ppin1NewPin').value = "";
			document.getElementById('ppin1NewPin2').value = "";
			document.getElementById('ppin1OldPin').focus();
			break;
		case "pin2":
			document.getElementById('pin2OldPin').value = "";
			document.getElementById('pin2NewPin').value = "";
			document.getElementById('pin2NewPin2').value = "";
			document.getElementById('pin2OldPin').focus();
			break;
		case "ppin2":
			document.getElementById('ppin2OldPin').value = "";
			document.getElementById('ppin2NewPin').value = "";
			document.getElementById('ppin2NewPin2').value = "";
			document.getElementById('ppin2OldPin').focus();
			break;
		case "bpin1":
			document.getElementById('bpin1OldPin').value = "";
			document.getElementById('bpin1NewPin').value = "";
			document.getElementById('bpin1NewPin2').value = "";
			document.getElementById('bpin1OldPin').focus();
			break;
		case "bpin2":
			document.getElementById('bpin2OldPin').value = "";
			document.getElementById('bpin2NewPin').value = "";
			document.getElementById('bpin2NewPin2').value = "";
			document.getElementById('bpin2OldPin').focus();
			break;
		case "email":
			try {
				if ( esteidData.authCert.isTempel() )
				{
					document.getElementById('emailsContentCheck').style.display = 'none';
					document.getElementById('emailsContentCheckID').style.display = 'block';
					break;
				}
			} catch ( err ) {}
			if ( !emailsLoaded )
				document.getElementById('emailsContentCheck').style.display = 'block';
			document.getElementById('emailsContentCheckID').style.display = 'none';
			break;
		case "mobile":
			document.getElementById('inputCheckMobile').style.display = 'block';
			try {
				if ( esteidData.authCert.isTempel() )
				{
					document.getElementById('inputCheckMobile').style.display = 'none';
					break;
				}
			} catch ( err ) {}			
			break;
		case "puk":
			if ( esteidData.getPukRetryCount() == 0 )
			{
				document.getElementById('buttonChangePUK2').style.display = 'none';
				document.getElementById('labelPUKBlocked').style.display = 'block';
				document.getElementById('chanePIN4PUK').style.display = 'none';
			} else {
				document.getElementById('buttonChangePUK2').style.display = 'block';
				document.getElementById('labelPUKBlocked').style.display = 'none';
				document.getElementById('chanePIN4PUK').style.display = 'block';
			}
			break;
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

function savePicture()
{ extender.savePicture(); }

function setPicture( img, code )
{
	if ( code != "" )
	{
		var codes = code.split( "|" );
		_alert( 'error', _(codes[0]) + ( typeof codes[1] != "undefined" ? "\n" + codes[1] : "") );
	} else {
		document.getElementById('photo').innerHTML = '<img width="90" height="120" src="' + img + '">';
		document.getElementById('savePhoto').style.display = 'block';
	}
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
	extender.closeLoading();
	document.getElementById('emailsContent').innerHTML = _(msg);
}

function setEmails( code, msg )
{
	extender.closeLoading();
	if ( code == "0" && msg.indexOf( ' - ' ) == -1 )
		code = "20";
	//not activated
	if ( code == "20" )
	{
		document.getElementById('emailAddress').focus();
		document.getElementById('emailsContentActivate').style.display = "block";
	}
	//success
	if ( code != "0" && code != "20" )
		_alert( 'error', _(code) );
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
	var txt = document.getElementById('emailAddress').value;
	if ( txt == "" || txt.indexOf('@') == -1 || txt.indexOf('.') == -1 || txt.indexOf(' ') != -1 )
	{
		_alert( 'warning', _('emailEnter') );
		document.getElementById('emailAddress').focus();
		return;
	}
	extender.showLoading( _('activatingEmail') );
	extender.activateEmail( document.getElementById('emailAddress').value );
	document.getElementById('emailAddress').value = "";
}

function handleError(msg)
{
	if ( msg == "" )
		return;
	if ( msg.indexOf("smart card API error") != -1 || msg.indexOf("No card in specified reader") != -1 )
	{
		extender.closeLoading();
		return;
	}
	if ( msg == "PIN1InvalidRetry" || msg == "PIN1Invalid" )
	{
		msg = "PIN1InvalidRetry";
		var ret = esteidData.getPin1RetryCount( true );
		if ( ret == -1 || ret > 3 )
			return;
		if ( ret == 0 )
		{
			_alert( 'error', _("PIN1Blocked") );
			readCardData();
		} else
			_alert( 'error', _( msg ).replace( /%d/, ret ) );		
		return;
	}
	_alert( 'error', _('errorFound') + _( msg ) )
}

function disableFields( translate )
{
	var divs = document.getElementsByTagName('div');
	for( i=0;i<divs.length;i++ )
	{
		if ( (typeof divs[i].className == "undefined") || ( typeof divs[i].id == "undefined" ) || divs[i].className != "content" )
				continue;
		divs[i].style.display = 'none';
	}

	if ( translate != null )
		return;

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

	document.getElementById('emailsContent').innerHTML = "";
	document.getElementById('photo').innerHTML = '<div id="photoContent" style="padding-top:50px;"><a href="#" onClick="loadPicture();"><trtag trcode="loadPicture">' + _('loadPicture') + '</trtag></a></div>';
	document.getElementById('savePhoto').style.display = 'none';


	try {
		if ( !cardManager.anyCardsInReader() )
		{
			document.getElementById('cardInfoNoCard').style.display='block';
			document.getElementById('cardInfoNoCardText').innerHTML=_( cardManager.getReaderCount() == 0 ? 'noReaders' : 'noCard' );
		}
	} catch( err ) {}
}

function enableFields()
{
	document.getElementById('cardInfo').style.display='block';
	document.getElementById('cardInfoNoCard').style.display='none';
	var buttons = document.getElementById('leftMenus').getElementsByTagName('input');
	var selected = "";
	for( i=0;i<buttons.length;i++ )
	{
		if ( !buttons[i].className.indexOf( "buttonSelected" ) )
		{
			selected = buttons[i].name;
			break;
		}
	}
	if ( selected != "" )
	{
		var divs = document.getElementsByTagName('div');
		for( i=0;i<divs.length;i++ )
		{
			if ( (typeof divs[i].className == "undefined") || ( typeof divs[i].id == "undefined" ) || divs[i].className != "content" )
				continue;
			if ( !divs[i].id.indexOf( selected ) )
				divs[i].style.display = 'block';
		}
	}

	document.getElementById('buttonChangePUK2').style.display = 'block';
	document.getElementById('labelPUKBlocked').style.display = 'none';
	document.getElementById('chanePIN4PUK').style.display = 'block';
}

function checkMobile()
{ extender.getMidStatus(); }

function setMobile( result )
{
	var strings = result.split(";");
	if ( strings.length > 1 )
	{
		setActive('smobile','');
		if ( strings[2] == "Active" )
		{
			document.getElementById('activateMobileButton').style.display = "none";
			document.getElementById('mobileStatus').style.color = "#509b00";
		} else {
			document.getElementById('mobileStatus').style.color = "#e80303";
			document.getElementById('inputActivateMobile').attributes["onclick"].value = "extender.openUrl('" + strings[3] + "');";
			document.getElementById('activateMobileButton').style.display = "block";
		}
		document.getElementById('mobileNumber').innerHTML = strings[0];
		document.getElementById('mobileOperator').innerHTML = strings[1];
		document.getElementById('mobileStatus').innerHTML = _(strings[2]);
	}
}

function updateCert()
{
	extender.showLoading( _('updateCert') );
	if ( !extender.updateCertAllowed() )
	{
		extender.closeLoading();
		return;
	}
	var ok = false;
	try {
	 ok = extender.updateCert();
	} catch( err ){}
	if ( ok )
	{
		extender.closeLoading();
		activeCardId = "";
		cardInserted( cardManager.activeReaderNum() );
	}
	extender.closeLoading();
}

function changePin1()
{ changePin( 1 ); }

function changePin2()
{ changePin( 2 ); }

function changePin( type )
{
	var oldVal=document.getElementById('pin' + type + 'OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		_alert( 'warning', _( 'PIN' + type + 'Enter' ) );
		document.getElementById('pin' + type + 'OldPin').focus();
		return;
	}		
	if ( !eval("esteidData.validatePin" + type + "(oldVal)") )
	{
		ret = eval("esteidData.getPin" + type + "RetryCount() ");
		if ( ret == 0 || ret > 3 )
		{
			document.getElementById('pin' + type + 'OldPin').value = "";
			document.getElementById('pin' + type + 'NewPin').value = "";
			document.getElementById('pin' + type + 'NewPin2').value = "";
			if ( ret == 0 )
			{
				_alert( 'error', _("PIN" + type + "Blocked") );
				readCardData();
				setActive('cert','');
			} else
				_alert( 'error', _("PIN" + type + "ValidateFailed") );
			return;
		}
		_alert( 'warning', _( 'PIN' + type + 'InvalidRetry' ).replace( /%d/, ret ) );
		document.getElementById('pin' + type + 'OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pin' + type + 'NewPin').value;
	var newVal2=document.getElementById('pin' + type + 'NewPin2').value;
	if (newVal==null || newVal == "") 
	{
		_alert( 'warning', _( 'PIN' + type + 'EnterNew' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}
	if ( oldVal == newVal )
	{
		_alert( 'warning', _( 'PIN' + type + 'NewOldSame' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}
	//PIN1 length 4-12, PIN2 5-12
	if ( (type == 1 && (newVal.length<4 || newVal.length > 12) ) || 
		(type == 2 && (newVal.length<5 || newVal.length > 12) ) ) 
	{
		_alert( 'warning', _( 'PIN' + type + 'Length' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}
	//check date of birth and birth year inside pin
	if ( !esteidData.checkPin( newVal ) )
	{
		_alert( 'warning', _( 'PINCheck' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}
	if (newVal2==null || newVal2 == "" )
	{
		_alert( 'warning', _( 'PIN' + type + 'Retry' ) );
		document.getElementById('pin' + type + 'NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		_alert( 'warning', _( 'PIN' + type + 'Different' ) );
		document.getElementById('pin' + type + 'NewPin2').focus();
		return;		
	}
	if (eval("esteidData.changePin" + type + "(newVal, oldVal)"))
	{
		document.getElementById('pin' + type + 'OldPin').value = "";
		document.getElementById('pin' + type + 'NewPin').value = "";
		document.getElementById('pin' + type + 'NewPin2').value = "";
		_alert( 'notice', _( 'PIN' + type + 'Changed' ) );
		setActive('cert','');
	} else
		_alert( 'error', _( 'PIN' + type + 'Unsuccess' ) );
}

function changePin1PUK()
{ changePinPUK( 1 ); }

function changePin2PUK()
{ changePinPUK( 2 ); }

function changePinPUK( type )
{
	var oldVal=document.getElementById('ppin' + type + 'OldPin').value;
	if (oldVal==null || oldVal.length < 4)
	{
		_alert( 'warning', _( 'PUKEnterOld' ) );
		document.getElementById('ppin' + type + 'OldPin').focus();
		return;
	}
	if ( !esteidData.validatePuk(oldVal) )
	{
		ret = esteidData.getPukRetryCount();
		if ( ret == 0 || ret > 3 )
		{
			document.getElementById('ppin' + type + 'OldPin').value = "";
			document.getElementById('ppin' + type + 'NewPin').value = "";
			document.getElementById('ppin' + type + 'NewPin2').value = "";
			if ( ret == 0 )
			{
				_alert( 'error', _("PUKBlocked") );
				readCardData();
				setActive('cert','');
			} else
				_alert( 'error', _("PUKValidateFailed") );
			return;
		}
		_alert( 'warning', _('PUKInvalidRetry').replace( /%d/, ret ) );
		document.getElementById('ppin' + type + 'OldPin').focus();
		return;
	}

	var newVal=document.getElementById('ppin' + type + 'NewPin').value;
	var newVal2=document.getElementById('ppin' + type + 'NewPin2').value;
	if (newVal==null || newVal == "")
	{
		_alert( 'warning', _( 'PIN' + type + 'EnterNew' ) );
		document.getElementById('ppin' + type + 'NewPin').focus();
		return;
	}
	if ( oldVal == newVal )
	{
		_alert( 'warning', _( 'PIN' + type + 'NewOldSame' ) );
		document.getElementById('ppin' + type + 'NewPin').focus();
		return;
	}
	//PIN1 length 4-12, PIN2 5-12
	if ( (type == 1 && (newVal.length<4 || newVal.length > 12) ) ||
		(type == 2 && (newVal.length<5 || newVal.length > 12) ) )
	{
		_alert( 'warning', _( 'PIN' + type + 'Length' ) );
		document.getElementById('ppin' + type + 'NewPin').focus();
		return;
	}
	//check date of birth and birth year inside pin
	if ( !esteidData.checkPin( newVal ) )
	{
		_alert( 'warning', _( 'PINCheck' ) );
		document.getElementById('ppin' + type + 'NewPin').focus();
		return;
	}
	if (newVal2==null || newVal2 == "" )
	{
		_alert( 'warning', _( 'PIN' + type + 'Retry' ) );
		document.getElementById('ppin' + type + 'NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		_alert( 'warning', _( 'PIN' + type + 'Different' ) );
		document.getElementById('ppin' + type + 'NewPin2').focus();
		return;
	}
	if ( eval("esteidData.validatePin" + type + "(newVal)") )
	{
		_alert( 'warning', _( 'PIN' + type + 'NewOldSame' ) );
		document.getElementById('ppin' + type + 'NewPin').focus();
		return;
	}
	if ( eval('esteidData.unblockPin' + type + '(newVal, oldVal)') )
	{
		document.getElementById('ppin' + type + 'OldPin').value = "";
		document.getElementById('ppin' + type + 'NewPin').value = "";
		document.getElementById('ppin' + type + 'NewPin2').value = "";
		_alert( 'notice', _( 'PIN' + type + 'Changed' ) );
		setActive('cert','');
	} else
		_alert( 'error', _( 'PIN' + type + 'Unsuccess' ) );
}

function changePuk()
{
	var oldVal=document.getElementById('pukOldPin').value;
	if (oldVal==null || oldVal == "")
	{
		_alert( 'warning', _('PUKEnterOld') );
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
			_alert( 'error', _("PUKBlocked") );
			readCardData();
			setActive('cert','');
			return;
		}
		_alert( 'warning', _('PUKInvalidRetry').replace( /%d/, ret ) );
		document.getElementById('pukOldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('pukNewPin').value;
	var newVal2=document.getElementById('pukNewPin2').value;
	if (newVal==null || newVal == "") 
	{
		_alert( 'warning', _('PUKEnterNew') );
		document.getElementById('pukNewPin').focus();
		return;
	}
	if ( oldVal == newVal )
	{
		_alert( 'warning', _( 'PUKNewOldSame' ) );
		document.getElementById('pin' + type + 'NewPin').focus();
		return;
	}	
	//PUK length 8-12
	if ( newVal.length<8 || newVal.length > 12 ) 
	{
		_alert( 'warning', _( 'PUKLength' ) );
		document.getElementById('pukNewPin').focus();
		return;
	}		
	
	if (newVal2==null || newVal2 == "")
	{
		_alert( 'warning', _('PUKRetry') );
		document.getElementById('pukNewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		_alert( 'warning', _('PUKDifferent') );
		document.getElementById('pukNewPin2').focus();
		return;		
	}
	if (esteidData.changePuk(newVal, oldVal))
	{
		document.getElementById('pukOldPin').value = "";
		document.getElementById('pukNewPin').value = "";
		document.getElementById('pukNewPin2').value = "";
		_alert( 'notice', _('PUKChanged') );
		setActive('puk','');
	} else
		_alert( 'error', _('PUKUnsuccess') );
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
		_alert( 'warning', _('PUKEnter') );
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
			_alert( 'error', _("PUKBlocked") );
			readCardData();
			setActive('cert','');
			return;
		}
		_alert( 'warning', _("PUKInvalidRetry").replace( /%d/, ret ) );
		document.getElementById('bpin' + type + 'OldPin').focus();
		return;
	}
	
	var newVal=document.getElementById('bpin' + type + 'NewPin').value;
	var newVal2=document.getElementById('bpin' + type + 'NewPin2').value;
	if (newVal==null || newVal == "") 
	{
		_alert( 'warning', _('PIN' + type + 'EnterNew') );
		document.getElementById('bpin' + type + 'NewPin').focus();
		return;
	}		
	//PIN1 length 4-12, PIN2 5-12
	if ( (type == 1 && (newVal.length<4 || newVal.length > 12) ) || 
		(type == 2 && (newVal.length<5 || newVal.length > 12) ) ) 
	{
		_alert( 'warning', _( 'PIN' + type + 'Length' ) );
		document.getElementById('bpin' + type + 'NewPin').focus();
		return;
	}
	//check date of birth and birth year inside pin
	if ( !esteidData.checkPin( newVal ) )
	{
		_alert( 'warning', _( 'PINCheck' ) );
		document.getElementById('bpin' + type + 'NewPin').focus();
		return;
	}
	if (newVal2==null || newVal2 == "")
	{
		_alert( 'warning', _('PIN' + type + 'Retry') );
		document.getElementById('bpin' + type + 'NewPin2').focus();
		return;
	}
	if ( newVal != newVal2 )
	{
		_alert( 'warning', _('PIN' + type + 'Different') );
		document.getElementById('bpin' + type + 'NewPin2').focus();
		return;		
	}
	if (eval('esteidData.unblockPin' + type + '(newVal, oldVal)'))
	{
		document.getElementById('bpin' + type + 'OldPin').value = "";
		document.getElementById('bpin' + type + 'NewPin').value = "";
		document.getElementById('bpin' + type + 'NewPin2').value = "";
		_alert( 'notice', _('PIN' + type + 'UnblockSuccess') );
		readCardData();
		setActive('cert','');
	} else
		_alert( 'error', _('PIN' + type + 'UnblockFailed') );
}

function _alert( type, text )
{ extender.showMessage( type, text ); }
