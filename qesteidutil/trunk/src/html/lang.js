var defaultLanguage = "et";
var language = defaultLanguage;

var helpUrl_et = "http://support.sk.ee/";
var helpUrl_en = "http://support.sk.ee/eng/";
var helpUrl_ru = "http://support.sk.ee/ru/";

//code: (est, eng, rus)
var htmlStrings = {
	"linkDiagnostics": new tr( "Diagnostika", "Diagnostics", "" ),
	"linkSettings": new tr( "Seaded", "Settings", "" ),
	"linkHelp": new tr( "Abi", "Help", "" ),

	"personName": new tr( "Nimi", "Name", "" ),
	"personCode": new tr( "Isikukood", "Personal Code", "" ),
	"personBirth": new tr( "Sündinud", "Birth", "" ),
	"personCitizen": new tr( "Kodakondsus", "Citizenship", "" ),
	"personEmail": new tr( "E-post", "E-mail", "" ),

	"labelCardInReaderID": new tr( "Lugejas on ID kaart", "Card in reader", "" ),
	"labelThisIs": new tr( "See on", "This is a", "" ),
	"labelIsValid": new tr( "kehtiv", "valid", "" ),
	"labelDocument": new tr( "dokument", "document", "" ),
	"labelCardValidTill": new tr( "Kaart on kehtiv kuni", "Card is valid till", "" ),
	
	"labelAuthCert": new tr( "Isikutuvastamise sertifikaat", "Authentication certificate", "" ),
	"labelSignCert": new tr( "Allkirjastamise sertifikaat", "Signature certificate", "" ),
	"labelCertIs": new tr( "Sertifikaat on", "Certificate is", "" ),
	"labelCertIsValidTill": new tr( "Sertifikaat kehtib kuni", "Certificate is valid till", "" ),
	"labelAuthUsed": new tr( "ID-kaarti on kasutatud isikutuvastamiseks", "Authentication key has been used", "" ),
	"labelSignUsed": new tr( "ID-kaarti on kasutatud allkirjastamiseks", "Signature key has been used", "" ),
	"labelTimes": new tr( "korda", "times", "" ),
	
	"labelCertBlocked": new tr( "Sertifikaat on blokeeritud.", "Certificate is blocked.", "" ),
	"labelAuthKeyBlocked": new tr( "Selle ID-kaardiga ei ole hetkel võimalik audentida, kuna PIN1 koodi on sisestatud 3 korda valesti.", "", "" ),
	"labelSignKeyBlocked": new tr( "Selle ID-kaardiga ei ole hetkel võimalik anda digitaalallkirja, kuna PIN2 koodi on sisestatud 3 korda valesti.", "", "" ),
	"labelAuthCertBlocked": new tr( "Isikutuvastamise sertifikaat on blokeeritud.", "Authentication certificate is blocked." , "" ),
	"labelSignCertBlocked": new tr( "Allkirjastamise sertifikaat on blokeeritud.", "Signing certificate is blocked.", "" ),
	"labelCertUnblock": new tr( "Sertfikaadi blokeeringu tühistamiseks sisesta kaardi PUK-kood.", "To unblock certificate you have to enter PUK code.", "" ),
	"labelCertUnblock1": new tr( "PUK-koodi leiad ID-kaardi koodide ümbrikus, kui sa pole seda vahepeal muutnud.", "You can find your PUK code inside ID-card codes envelope.", "" ),
	"labelCertUnblock2": new tr( "Kui sa ei tea oma ID-kaardi PUK koodi, külasta teeninduskeskust kus saad kaardile laadida uued sertifikaadid ning saad uue koodiümbriku.", "", "" ),
	
	"labelChangingPIN1": new tr( "PIN1 koodi vahetus", "Change PIN1 code", "" ),
	"labelChangingPIN11": new tr( "PIN1 on kood, mida kasutatakse isikutuvastamise sertifikaadile ligipääsemiseks.", "", "" ),
	"labelChangingPIN12": new tr( "Kui PIN1 koodi sisestatakse kolm korda valesti, siis isikutuvastamise sertifikaat blokeeritakse ning ID-kaarti pole võimalik kasutada isiku tuvastamiseks enne kui blokeering tühistatakse PUK-koodiga.", "", "" ),

	"labelChangingPIN2": new tr( "PIN2 koodi vahetus", "Change PIN2 code", "" ),
	"labelChangingPIN21": new tr( "PIN2 on kood, mida kasutatakse digitaalallkirja andmiseks.", "", "" ),
	"labelChangingPIN22": new tr( "Kui PIN2 koodi sisestatakse kolm korda valesti, siis allkirja sertifikaat blokeeritakse ning ID-kaarti pole võimalik kasutada digitaalallkirja andmiseks enne kui blokeering tühistatakse PUK-koodiga.", "", "" ),
	
	"labelChangingPUK": new tr( "PUK koodi vahetus", "Change PUK code", "" ),
	"labelChangingPUK2": new tr( "Kui peale vahetamist PUK kood läheb meelest ära ja sertifikaat jääb blokeerituks kolme vale PIN1 või PIN2 sisetamise järel, siis ainus võimalus ID-kaart jälle tööle saada on pöörduda teenindusbüroo poole.", "", "" ),
	
	"labelInputPUK": new tr( "PUK-kood on kood, millega saab tühistada sertifikaadi blokeeringu kui PIN1 või PIN2 koodi on 3 korda järjest valesti sisestatud.", "", "" ),
	"labelInputPUK2": new tr( "PUK-kood on kirjas koodiümbrikus, mida väljastatakse koos ID-kaardiga või sertifikaatide vahetamisel.", "", "" ),
	
	"inputCert": new tr( "Sertifikaadid", "Certificates", "" ),
	"inputEmail": new tr( "@eesti.ee e-post", "@eesti.ee e-mail", "" ),
	"inputActivateEmail": new tr( "Aktiveeri @eesti.ee e-post", "Activate @eesti.ee email", "" ),
	"inputCheckEmails": new tr( "Kontrolli @eesti.ee e-posti seadistust", "Check your @eesti.ee email settings", "" ),
	"inputMobile": new tr( "Mobiil-ID", "Mobile-ID", "" ),
	"inputActivateMobile": new tr( "Aktiveeri Mobiil-ID teenus", "Activate Mobile-ID", "" ),
	"inputPUK": new tr( "PUK kood", "PUK code", "" ),
	
	"inputChange": new tr( "Muuda", "Change", "" ),
	"inputCancel": new tr( "Tühista", "Cancel", "" ),
	"inputChangePIN1": new tr( "Muuda PIN1", "Change PIN1", "" ),
	"inputChangePIN2": new tr( "Muuda PIN2", "Change PIN2", "" ),
	"inputChangePUK": new tr( "Muuda PUK", "Change PUK", "" ),
	"inputCertDetails": new tr( "Vaata üksikasju", "View details", "" ),
	"inputUnblock": new tr ( "Tühista blokeering", "Revoke blocking", "" ),
	
	"labelCurrentPIN1": new tr( "Vana PIN1 kood", "Current PIN1 code", "" ),
	"labelNewPIN1": new tr( "Uus PIN1 kood", "New PIN1 code", "" ),
	"labelNewPIN12": new tr( "Uus PIN1 kood uuesti", "Repeat new PIN1 code", "" ),
	"labelCurrentPIN2": new tr( "Vana PIN2 kood", "Current PIN2 code", "" ),
	"labelNewPIN2": new tr( "Uus PIN2 kood", "New PIN2 code", "" ),
	"labelNewPIN22": new tr( "Uus PIN2 kood uuesti", "Repeat new PIN2 code", "" ),
	"labelCurrentPUK": new tr( "Vana PUK kood", "Current PUK code", "" ),
	"labelNewPUK": new tr( "Uus PUK kood", "New PUK code", "" ),
	"labelNewPUK2": new tr( "Uus PUK kood uuesti", "Repeat new PUK code", "" ),
	"labelPUK": new tr( "PUK kood", "PUK code", "" ),
	
	"labelEmailAddress": new tr( "E-posti aadress kuhu suunatakse Teie @eesti.ee kirjad", "Email addres where your @eesti.ee emails will be forwarded", "" ),
	"labelEmailUrl": new tr( "Täiuslikuma ametliku e-posti suunamise häälestamisvahendi leiad portaalist", "", "" ),
	
	"labelMobile": new tr( "Mobiil-ID on võimalus kasutada isikutuvastamiseks ja digitaalallkirja andmiseks ID-kaardi asemel mobiiltelefoni.", "", "" ),
	"labelMobile2": new tr( "Mobiil-ID kasutamiseks on vajalik uus SIM-kaart, mille sa saad omale mobiilsideoperaatori käest. Kui selline kaart on sul juba olemas, tuleb teenus aktiveerida.", "", "" ),
	"labelMobileReadMore": new tr( "Loe täpsemalt id.ee kodulehelt", "", "" ),
	
	"errorFound": new tr( "Tekkis viga: ", "Error occured: ", "" ),
	"loadEmail": new tr( "Laen e-posti seadeid", "Loading e-mail settings", "" ),
	"forwardFailed": new tr( "E-posti suunamise aktiveerimine ebaõnnestus.", "Failed activating e-mail forwards.", "" ),
	"loadFailed": new tr( "E-posti aadresside laadimine ebaõnnestus.", "Failed loading e-mail settings.", "" ),
	"emailEnter": new tr( "E-posti aadress sisestamata!", "Enter e-mail address!", "" ),
	"loadPicture": new tr( "Lae pilt", "Load picture", "" ),
	"savePicture": new tr( "salvesta", "save", "" ),
	"savePicFailed": new tr( "Pildi salvestamine ebaõnnestus!", "Saving picture failed!", "" ),
	"loadPic": new tr( "Laen pilti", "Loading picture", "" ),
	"loadPicFailed": new tr( "Pildi laadimine ebaõnnestus!", "Loading picture failed!", "" ),
	"loadCardData": new tr( "Loen andmeid", "Reading data", "" )
};

//codes from eesti.ee
var eestiStrings = {
	"0":  new tr( "Toiming õnnestus", "Success", "" ),
	"1":  new tr( "ID-kaart pole väljastatud riiklikult tunnustatud sertifitseerija poolt.", "", "" ),
	"2":  new tr( "Sisestati vale PIN-kood, katkestati PIN-koodi sisestamine, tekkisid probleemid sertifikaatidega või puudub ID-kaardi tugi brauseris.","", "" ),
	"3":  new tr( "ID-kaardi sertifikaat ei kehti.", "", "" ),
	"4":  new tr( "Sisemine on lubatud ainult Eesti isikukoodiga.", "", "" ),
	"10": new tr( "Tundmatu viga.", "Unknown error", "" ),
	"11": new tr( "KMA päringu tegemisel tekkis viga.", "" ,"" ),
	"12": new tr( "Äriregistri päringu tegemisel tekkis viga.", "", "" ),
	"20": new tr( "Ühtegi ametliku e-posti suunamist ei leitud.", "", "" ),
	"21": new tr( "Teie e-posti konto on suletud. Avamiseks saatke palun e-kiri aadressil toimetaja@eesti.ee või helistage telefonil 663 0215.", "", "" ),
	"22": new tr( "Vigane e-posti aadress.", "Invalid e-mail address", "" ),
	"23": new tr( "Suunamine on salvestatud, ning Teile on saadetud kiri edasisuunamisaadressi aktiveerimisvõtmega. Suunamine on kasutatav ainult pärast aktiveerimisvõtme sisestamist.", "", "" )
};

var eidStrings = {
	"noCard": new tr( "Ei leitud ühtegi ID-kaarti", "No card found", "" ),
	"noReaders": new tr( "Ühtegi kiipkaardi lugejat pole ühendatud", "No readers found", "" ),
	"certValid": new tr( "kehtiv ja kasutatav", "valid and applicable", "" ),
	"certBlocked": new tr( "kehtetu", "expired", "" ),
	"validBlocked": new tr( "kehtiv kuid blokeeritud", "valid but blocked", "" ),
	
	"PINCheck": new tr( "PIN1 ja PIN2 ei tohi sisaldada sünnikuupäeva ja -aastat", "PIN1 and PIN2 have to be different than date of birth or year of birth", "" ),
	"PIN1Enter": new tr( "Sisesta vana PIN1 kood", "Current PIN1 code", "" ),
	"PIN1Length": new tr( "PIN1 pikkus peab olema 4-12 numbrit", "PIN1 length has to be between 4 and 12", "" ),
	"PIN1InvalidRetry": new tr( "Vale PIN1 kood. Saad veel proovida %d korda.", "Wrong PIN1 code. You can try %d more times.", "" ),
	"PIN1EnterNew": new tr( "Sisesta uus PIN1 kood", "Enter new PIN1 code", "" ),
	"PIN1Retry": new tr( "Korda uut PIN1 koodi", "Retry your new PIN1 code", "" ),
	"PIN1Different": new tr( "Uued PIN1 koodid on erinevad", "New PIN1 codes doesn't match", "" ),
	"PIN1Changed": new tr( "PIN1 kood muudetud!", "PIN1 changed!", "" ),
	"PIN1Unsuccess": new tr( "PIN1 muutmine ebaõnnestus.", "Changing PIN1 failed", "" ),
	"PIN1UnblockFailed": new tr( "Blokeeringu tühistamine ebaõnnestus.\nUus PIN peab erinema eelmisest PINist!", "Unblock failed.\nYour new PIN1 has to be different than current!", "" ),
	"PIN1UnblockSuccess": new tr( "PIN1 kood on muudetud ja sertifikaadi blokeering tühistatud!", "PIN1 changed and you current sertificates blocking has been removed!", "" ),
	"PIN1Blocked": new tr( "PIN1 blokeeritud.", "PIN1 blocked", "" ),

	"PIN2Enter": new tr( "Sisesta vana PIN2 kood", "Current PIN2 code", "" ),
	"PIN2Length": new tr( "PIN2 pikkus peab olema 5-12 numbrit", "PIN2 length has to be between 5 and 12", "" ),
	"PIN2InvalidRetry": new tr( "Vale PIN2 kood. Saad veel proovida %d korda.", "Wrong PIN2 code. You can try %d more times.", "" ),
	"PIN2NewDifferent": new tr( "Uued PIN2 koodid on erinevad.", "New PIN2 codes doesn't match", "" ),
	"PIN2EnterNew": new tr( "Sisesta uus PIN2 kood.", "Enter new PIN2 code", "" ),
	"PIN2Retry": new tr( "Korda uut PIN2 koodi.", "Retry your new PIN2 code", "" ),
	"PIN2Different": new tr( "Uued PIN2 koodid on erinevad", "New PIN2 codes doesn't match", "" ),
	"PIN2Changed": new tr( "PIN2 kood muudetud!", "PIN2 changed!", "" ),
	"PIN2Unsuccess": new tr( "PIN2 muutmine ebaõnnestus.", "Changing PIN2 failed", "" ),
	"PIN2UnblockFailed": new tr( "Blokeeringu tühistamine ebaõnnestus.\nUus PIN peab erinema eelmisest PINist!", "Unblock failed.\nYour new PIN2 has to be different than current!", "" ),
	"PIN2UnblockSuccess": new tr( "PIN2 kood on muudetud ja sertifikaadi blokeering tühistatud!", "PIN2 changed and you current sertificates blocking has been removed!", "" ),
	"PIN2Blocked": new tr( "PIN2 blokeeritud.", "PIN2 blocked", "" ),

	"PUKEnter": new tr( "Sisesta PUK kood.", "Enter PUK code.", "" ),
	"PUKLength": new tr( "PUK koodi pikkus peab olema 8-12 numbrit.", "PUK length has to be between 8 and 12.", "" ),
	"PUKEnterOld": new tr( "Sisesta vana PUK kood.", "Enter current PUK code.", "" ),
	"PUKEnterNew": new tr( "Sisesta uus PUK kood.", "Enter new PUK code.", "" ),
	"PUKRetry": new tr( "Korda uut PUK koodi.", "Retry your new PUK code.", "" ),
	"PUKDifferent": new tr( "Uued PUK koodid on erinevad", "New PUK codes doesn't match", "" ),
	"PUKChanged": new tr( "PUK kood muudetud!", "PUK changed!", "" ),
	"PUKUnsuccess": new tr( "PUK koodi muutmine ebaõnnestus!", "Changing PUK failed!", "" ),
	"PUKInvalidRetry": new tr( "Vale PUK kood. Saad veel proovida 2 korda.", "Wrong PUK code. You can try %d more times.", "" ),
	"PUKBlocked": new tr( "PUK kood blokeeritud.", "PUK blocked", "" )
};

function selectLanguage()
{
	var select = document.getElementById('headerSelect'); 
	language = select.options[select.selectedIndex].value;
	translateHTML();
	readCardData();
	extender.setLanguage( language );
}

function tr( est, eng, rus )
{
	this.et = est;
	this.en = eng;
	this.ru = rus;
}

function _( code, defaultString )
{
	if ( typeof htmlStrings[code] != "undefined" )
	{
		if ( eval( "htmlStrings[\"" + code + "\"]." + language ) == "undefined" || eval( "htmlStrings[\"" + code + "\"]." + language ) == "")
			return eval( "htmlStrings[\"" + code + "\"]." + defaultLanguage );
		return eval( "htmlStrings[\"" + code + "\"]." + language );
	} else if ( typeof eidStrings[code] != "undefined" ) {
		if ( eval( "eidStrings[\"" + code + "\"]." + language ) == "undefined" || eval( "eidStrings[\"" + code + "\"]." + language ) == "" )
			return eval( "eidStrings[\"" + code + "\"]." + defaultLanguage );
		return eval( "eidStrings[\"" + code + "\"]." + language );
	} else if ( typeof eestiStrings[code] != "undefined" ) {
		if ( eval( "eestiStrings[\"" + code + "\"]." + language ) == "undefined" || eval( "eestiStrings[\"" + code + "\"]." + language ) == "")
			return eval( "eestiStrings[\"" + code + "\"]." + defaultLanguage );
		return eval( "eestiStrings[\"" + code + "\"]." + language );
	}
	return (typeof defaultString != "undefined" ) ? defaultString : code;
}

function translateHTML()
{
	var trTags = document.getElementsByTagName('trtag');
	for( var i in trTags )
		if ( (typeof trTags[i].getAttribute != "undefined") && trTags[i].getAttribute('trcode') != null )
			trTags[i].innerHTML = _( trTags[i].getAttribute('trcode'), trTags[i].innerHTML );

	var iTags = document.getElementsByTagName('input');
	for( var i in iTags )
		if ( iTags[i].type == "button" && (typeof iTags[i].getAttribute != "undefined") && iTags[i].getAttribute('trcode') != null )
			iTags[i].value = _( iTags[i].getAttribute('trcode'), iTags[i].value );
}

function openHelp()
{
	if ( typeof eval( 'helpUrl_' + language ) != "undefined" )
		extender.openUrl( eval( 'helpUrl_' + language ) );
	else
		extender.openUrl( eval( 'helpUrl_' + defaultLanguage ) );
}
