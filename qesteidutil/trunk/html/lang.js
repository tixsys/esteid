var defaultLanguage = "et";
var language = defaultLanguage;

var helpUrlet = "http://support.sk.ee/";
var helpUrlen = "http://support.sk.ee/eng/";
var helpUrlru = "http://support.sk.ee/ru/";

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

	"cardInReaderID": new tr( "Lugejas on ID kaart", "Card in reader", "" ),
	"thisIs": new tr( "See on", "This is a", "" ),
	"isValid": new tr( "kehtiv", "valid", "" ),
	"document": new tr( "dokument", "document", "" ),
	"cardValidTill": new tr( "Kaart on kehtiv kuni", "Card is valid till", "" ),
	
	"labelAuthCert": new tr( "Isikutuvastamise sertifikaat", "Authentication certificate", "" ),
	"labelSignCert": new tr( "Allkirjastamise sertifikaat", "Signature certificate", "" ),
	"labelCertIs": new tr( "Sertifikaat on", "Certificate is", "" ),
	"labelCertIsValidTill": new tr( "Sertifikaat kehtib kuni", "Certificate is valid till", "" ),
	"labelAuthUsed": new tr( "ID-kaarti on kasutatud isikutuvastamiseks", "Authentication key has been used", "" ),
	"labelSignUsed": new tr( "ID-kaarti on kasutatud allkirjastamiseks", "Signature key has been used", "" ),
	"labelTimes": new tr( "korda", "times", "" ),
	
	"inputCert": new tr( "Sertifikaadid", "Certificates", "" ),
	"inputEmail": new tr( "@eesti.ee e-post", "@eesti.ee e-mail", "" ),
	"inputMobile": new tr( "Mobiil-ID", "Mobile-ID", "" ),
	"inputPUK": new tr( "PUK kood", "PUK code", "" ),
	
	"errorFound": new tr( "Tekkis viga: ", "Error occured: ", "" ),
	"loadEmail": new tr( "Laen e-posti seadeid", "Loading e-mail settings", "" ),
	"forwardFailed": new tr( "E-posti suunamise aktiveerimine ebaõnnestus.", "Failed activating e-mail forwards.", "" ),
	"loadFailed": new tr( "E-posti aadresside laadimine ebaõnnestus.", "Failed loading e-mail settings.", "" ),
	"emailEnter": new tr( "E-posti aadress sisestamata!", "Enter e-mail address!", "" ),
	"loadPicture": new tr( "Lae pilt", "Load picture", "" ),
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
		if ( (typeof trTags[i].getAttribute != "undefined") && trTags[i].getAttribute('code') != null )
			trTags[i].innerHTML = _( trTags[i].getAttribute('code'), trTags[i].innerHTML );

	var iTags = document.getElementsByTagName('input');
	for( var i in iTags )
		if ( iTags[i].type == "button" && (typeof iTags[i].getAttribute != "undefined") && iTags[i].getAttribute('code') != null )
			iTags[i].value = _( iTags[i].getAttribute('code'), iTags[i].value );
}

function openHelp()
{ extender.openUrl( eval( 'helpUrl' + language ) ); }
