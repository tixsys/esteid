var defaultLanguage = "et";
var language = defaultLanguage;

//code: (est, eng, rus)
var eestiStrings = {
	"0":  new tr( "Toiming õnnestus", "", "" ),
	"1":  new tr( "ID-kaart pole väljastatud riiklikult tunnustatud sertifitseerija poolt.", "", "" ),
	"2":  new tr( "Sisestati vale PIN-kood, katkestati PIN-koodi sisestamine, tekkisid probleemid sertifikaatidega või puudub ID-kaardi tugi brauseris.","", "" ),
	"3":  new tr( "ID-kaardi sertifikaat ei kehti.", "", "" ),
	"4":  new tr( "Sisemine on lubatud ainult Eesti isikukoodiga.", "", "" ),
	"10": new tr( "Tundmatu viga.", "", "" ),
	"11": new tr( "KMA päringu tegemisel tekkis viga.", "" ,"" ),
	"12": new tr( "Äriregistri päringu tegemisel tekkis viga.", "", "" ),
	"20": new tr( "Ühtegi ametliku e-posti suunamist ei leitud.", "", "" ),
	"21": new tr( "Teie e-posti konto on suletud. Avamiseks saatke palun e-kiri aadressil toimetaja@eesti.ee või helistage telefonil 663 0215.", "", "" ),
	"22": new tr( "Vigane e-posti aadress.", "", "" ),
	"23": new tr( "Suunamine on salvestatud, ning Teile on saadetud kiri edasisuunamisaadressi aktiveerimisvõtmega. Suunamine on kasutatav ainult pärast aktiveerimisvõtme sisestamist.", "", "" ),
	"loadEmail": new tr( "Laen e-posti seadeid", "", "" ),
	"forwardFailed": new tr( "E-posti suunamise aktiveerimine ebaõnnestus.", "", "" ),
	"loadFailed": new tr( "E-posti aadresside laadimine ebaõnnestus.", "", "" ),
	"emailEnter": new tr( "E-posti aadress sisestamata!", "", "" ),
	"loadPic": new tr( "Laen pilti", "", "" ),
	"loadPicFailed": new tr( "Pildi laadimine ebaõnnestus", "", "" )
};

var eidStrings = {
	"errorFound": new tr( "Tekkis viga: ", "", "" ),
	"noCard": new tr( "Ei leitud ühtegi ID-kaarti.", "", "" ),
	"noReaders": new tr( "Ühtegi kiipkaardi lugejat pole ühendatud.", "", "" ),
	"certValid": new tr( "kehtiv ja kasutatav", "", "" ),
	"certBlocked": new tr( "kehtetu", "", "" ),
	"validBlocked": new tr( "kehtiv kuid blokeeritud", "", "" ),
	"PIN1Enter": new tr( "Sisesta vana PIN1 kood", "", "" ),
	"PIN1InvalidRetry": new tr( "Vale PIN1 kood. Saad veel proovida %d korda.", "", "" ),
	"PIN1EnterNew": new tr( "Sisesta uus PIN1 kood", "", "" ),
	"PIN1Retry": new tr( "Korda uut PIN1 koodi", "", "" ),
	"PIN1Different": new tr( "Uued PIN1 koodid on erinevad", "", "" ),
	"PIN1Changed": new tr( "PIN1 kood muudetud!", "", "" ),
	"PIN1Unsuccess": new tr( "PIN1 muutmine ebaõnnestus.", "", "" ),
	"PIN1UnblockFailed": new tr( "Blokeeringu tühistamine ebaõnnestus.", "", "" ),
	"PIN1UnblockSuccess": new tr( "PIN1 kood on muudetud ja sertifikaadi blokeering tühistatud!", "", "" ),

	"PIN2Enter": new tr( "Sisesta vana PIN2 kood", "", "" ),
	"PIN2InvalidRetry": new tr( "Vale PIN2 kood. Saad veel proovida %d korda.", "", "" ),
	"PIN2NewDifferent": new tr( "Uued PIN2 koodid on erinevad.", "", "" ),
	"PIN2EnterNew": new tr( "Sisesta uus PIN2 kood.", "", "" ),
	"PIN2Retry": new tr( "Korda uut PIN2 koodi.", "", "" ),
	"PIN2Different": new tr( "Uued PIN2 koodid on erinevad", "", "" ),
	"PIN2Changed": new tr( "PIN2 kood muudetud!", "", "" ),
	"PIN2Unsuccess": new tr( "PIN2 muutmine ebaõnnestus.", "", "" ),
	"PIN2UnblockFailed": new tr( "Blokeeringu tühistamine ebaõnnestus.", "", "" ),
	"PIN2UnblockSuccess": new tr( "PIN2 kood on muudetud ja sertifikaadi blokeering tühistatud!", "", "" ),

	"PUKEnter": new tr( "Sisesta PUK kood.", "", "" ),
	"PUKEnterOld": new tr( "Sisesta vana PUK kood.", "", "" ),
	"PUKEnterNew": new tr( "Sisesta uus PUK kood.", "", "" ),
	"PUKRetry": new tr( "Korda uut PUK koodi.", "", "" ),
	"PUKDifferent": new tr( "Uued PUK koodid on erinevad", "", "" ),
	"PUKChanged": new tr( "PUK kood muudetud!", "", "" ),
	"PUKUnsuccess": new tr( "PUK koodi muutmine ebaõnnestus.", "", "" ),
	"PUKInvalidRetry": new tr( "Vale PUK kood. Saad veel proovida 2 korda.", "", "" ),
	"PUKLocked": new tr( "PUK on lukus.", "", "" )
};

function tr( est, eng, rus )
{
	this.et = est;
	this.en = eng;
	this.ru = rus;
}

function _( code, defaultString )
{
	if ( typeof eestiStrings[code] != "undefined" )
	{
		if ( eval( "eestiStrings[\"" + code + "\"]." + language ) == "undefined" )
			return eval( "eestiStrings[\"" + code + "\"]." + defaultLanguage );
		return eval( "eestiStrings[\"" + code + "\"]." + language );
	} else if ( typeof eidStrings[code] != "undefined" ) {
		if ( eval( "eidStrings[\"" + code + "\"]." + language ) == "undefined" )
			return eval( "eidStrings[\"" + code + "\"]." + defaultLanguage );
		return eval( "eidStrings[\"" + code + "\"]." + language );
	}
	return (typeof defaultString != "undefined" ) ? defaultString : code;
}

function translateHTML()
{
	var trTags = document.getElementsByTagName('trtag');
	for( var i in trTags )
		if ( (typeof trTags[i].getAttribute != "undefined") && trTags[i].getAttribute('code') != null )
			trTags[i].innerHTML = _( trTags[i].getAttribute('code'), trTags[i].innerHTML );
}
