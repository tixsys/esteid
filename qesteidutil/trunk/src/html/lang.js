var defaultLanguage = "et";
var language = defaultLanguage;

var helpUrl_et = "http://support.sk.ee/";
var helpUrl_en = "http://support.sk.ee/eng/";
var helpUrl_ru = "http://support.sk.ee/ru/";

//code: (est, eng, rus)
var htmlStrings = {
	"Active": new tr( "sertifikaadid on aktiivsed ja Mobiil-ID kasutamine on võimalik.", "Active", "" ),
	"Not Active": new tr( "sertifikaadid on aktiveerimata, Mobiil-ID kasutamiseks on vajalik sertifikaatide aktiveerimine.", "", "" ),
	"Suspended": new tr( "sertifikaadid on peatatud, Mobiil-ID kasutamiseks on vajalik peatatuse lõpetamine.", "", "" ),
	"Revoked": new tr( "sertifikaadid on tunnistatud kehtetuks. Mobiil-ID kasutamiseks on vajalik hankida operaatorilt uus Mobiil-ID SIM kaart.", "", "" ),
	"Unknown": new tr( "sertifikaadi olek teadmata.", "", "" ),
	"Expired": new tr( "sertifikaadid on aegunud. Vajalik on operaatorilt uue SIM kaardi hankimine.", "", "" ),
	"mobileNoCert": new tr( "Kasutajal puuduvad Mobiil-ID sertifikaadid!", "", "" ),
	"mobileNotActive": new tr( "Kasutaja ID-kaardi sertifikaadid ei ole aktiivsed, info kuvamine ei ole võimalik!", "", "" ),
	"mobileInternalError": new tr( "Teenuse sisemine viga!", "", "" ),
	"mobileInterfaceNotReady": new tr( "Liides ei ole veel töökorras!", "", "" ),
	
	"linkDiagnostics": new tr( "Diagnostika", "Diagnostics", "Диагностика" ),
	"linkSettings": new tr( "Seaded", "Settings", "Настройки" ),
	"linkHelp": new tr( "Abi", "Help", "Help" ),

	"personName": new tr( "Nimi", "Name", "Имя" ),
	"personDepartment": new tr( "Osakond", "Department", "" ),
	"personCode": new tr( "Isikukood", "Personal Code", "Личный номер" ),
	"regcode": new tr( "Registrikood", "Reg nr", "" ),
	"personBirth": new tr( "Sündinud", "Birth", "День рождения" ),
	"personCitizen": new tr( "Kodakondsus", "Citizenship", "Гражданство" ),
	"personEmail": new tr( "E-post", "E-mail", "Эл. почта" ),

	"labelCardInReaderID": new tr( "Lugejas on ID kaart", "Card in reader", "Карта в считывателе" ),
	"labelThisIs": new tr( "See on", "This is a", "Это" ),
	"labelIsValid": new tr( "kehtiv", "valid", "Это" ),
	"labelDocument": new tr( "dokument", "document", "документ" ),
	"labelCardValidTill": new tr( "Kaart on kehtiv kuni", "Card is valid till", "Карта действительна до" ),
	
	"labelAuthCert": new tr( "Isikutuvastamise sertifikaat", "Authentication certificate", "Идентификационный сертификат" ),
	"labelSignCert": new tr( "Allkirjastamise sertifikaat", "Signature certificate", "Сертификат подписи" ),
	"labelCertIs": new tr( "Sertifikaat on", "Certificate is", "Сертификат подписи" ),
	"labelCertIsValidTill": new tr( "Sertifikaat kehtib kuni", "Certificate is valid till", "Сертификат действителен до" ),
	"labelAuthUsed": new tr( "ID-kaarti on kasutatud isikutuvastamiseks", "Authentication key has been used", "ID-карта была использована для идентификации" ),
	"labelSignUsed": new tr( "ID-kaarti on kasutatud allkirjastamiseks", "Signature key has been used", "ID-карта была использована для подписи" ),
	"labelTimes": new tr( "korda", "times", "раз" ),
	
	"labelCertBlocked": new tr( "Sertifikaat on blokeeritud.", "Certificate is blocked.", "Сертификат заблокирован." ),
	"labelAuthKeyBlocked": new tr( "Selle ID-kaardiga ei ole hetkel võimalik audentida, kuna PIN1 koodi on sisestatud 3 korda valesti.", "", "С данной ID-картой невозможно идентифицироваться, т.к. PIN1 был введён 3 раза неверно." ),
	"labelSignKeyBlocked": new tr( "Selle ID-kaardiga ei ole hetkel võimalik anda digitaalallkirja, kuna PIN2 koodi on sisestatud 3 korda valesti.", "", "С данной ID-картой невозможно создать цифровую подпись, т.к. PIN2 был введён 3 раза неверно." ),
	"labelAuthCertBlocked": new tr( "Isikutuvastamise sertifikaat on blokeeritud.", "Authentication certificate is blocked." , "Идентификационный сертификат заблокирован." ),
	"labelSignCertBlocked": new tr( "Allkirjastamise sertifikaat on blokeeritud.", "Signing certificate is blocked.", "Сертификат подписи заблокирован." ),
	"labelCertUnblock": new tr( "Sertfikaadi blokeeringu tühistamiseks sisesta kaardi PUK-kood.", "To unblock certificate you have to enter PUK code.", "Для разблокировки сертификата введите PUK код." ),
	"labelCertUnblock1": new tr( "PUK-koodi leiad ID-kaardi koodide ümbrikus, kui sa pole seda vahepeal muutnud.", "You can find your PUK code inside ID-card codes envelope.", "PUK код можно найти в конверте кодов." ),
	"labelCertUnblock2": new tr( "Kui sa ei tea oma ID-kaardi PUK koodi, külasta teeninduskeskust kus saad kaardile laadida uued sertifikaadid ning saad uue koodiümbriku.", "", "Если вы не знаете PUK код своей ID- карты, посетите цент обслуживания, где вы сможете получить новые сертификаты и конверт с кодами." ),
	
	"labelChangingPIN1": new tr( "PIN1 koodi vahetus", "Change PIN1 code", "Замена кода PIN1" ),
	"labelChangingPIN11": new tr( "PIN1 on kood, mida kasutatakse isikutuvastamise sertifikaadile ligipääsemiseks.", "", "PIN1 - код, который используют для доступа к сертификатам индентификации личности." ),
	"labelChangingPIN12": new tr( "Kui PIN1 koodi sisestatakse kolm korda valesti, siis isikutuvastamise sertifikaat blokeeritakse ning ID-kaarti pole võimalik kasutada isiku tuvastamiseks enne kui blokeering tühistatakse PUK-koodiga.", "", "Если PIN1 введён 3 раза неверно, тогда блокируется идентификационный сертификат и использовать ID- карту невозможно, пока блокировка не снята PUK кодом." ),

	"labelChangingPIN2": new tr( "PIN2 koodi vahetus", "Change PIN2 code", "Смена кода PIN2" ),
	"labelChangingPIN21": new tr( "PIN2 on kood, mida kasutatakse digitaalallkirja andmiseks.", "", "PIN2 - код, что используется для дигитальной подписи." ),
	"labelChangingPIN22": new tr( "Kui PIN2 koodi sisestatakse kolm korda valesti, siis allkirja sertifikaat blokeeritakse ning ID-kaarti pole võimalik kasutada digitaalallkirja andmiseks enne kui blokeering tühistatakse PUK-koodiga.", "", "Если PIN2 введён 3 раза неверно, тогда блокируется сертификат цифровой подписи и использовать ID- карту для цифровой подписи невозможно, пока  блокировка не снята PUK кодом." ),
	
	"labelChangingPUK": new tr( "PUK koodi vahetus", "Change PUK code", "Смена PUK кода" ),
	"labelChangingPUK2": new tr( "Kui peale vahetamist PUK kood läheb meelest ära ja sertifikaat jääb blokeerituks kolme vale PIN1 või PIN2 sisetamise järel, siis ainus võimalus ID-kaart jälle tööle saada on pöörduda teenindusbüroo poole.", "", "Если после смены PUK код забывается и сетрификат блокируется из-за неверно введённых PIN1 или PIN2, то единственной возможностью восстановить работоспособность ID- карты, это обратиться в бюро обслуживания." ),
	
	"labelInputPUK": new tr( "PUK-kood on kood, millega saab tühistada sertifikaadi blokeeringu kui PIN1 või PIN2 koodi on 3 korda järjest valesti sisestatud.", "", "PUK код, это код, который разблокирует сертификаты, если PIN1 или PIN2 были введены неверно 3 раза подрят." ),
	"labelInputPUK2": new tr( "PUK-kood on kirjas koodiümbrikus, mida väljastatakse koos ID-kaardiga või sertifikaatide vahetamisel.", "", "PUK код записан в конверте с кодами, который выдаётся вместе с ID- картой или при смене сетрификатов." ),
	
	"inputCert": new tr( "Sertifikaadid", "Certificates", "Сертификаты" ),
	"inputEmail": new tr( "@eesti.ee e-post", "@eesti.ee e-mail", "@eesti.ee эл. почта" ),
	"inputActivateEmail": new tr( "Aktiveeri @eesti.ee e-post", "Activate @eesti.ee email", "Активируй @eesti.ee эл. почту" ),
	"inputCheckEmails": new tr( "Kontrolli @eesti.ee e-posti seadistust", "Check your @eesti.ee email settings", "Проверь настройки эл. почты @eesti.ee" ),
	"inputMobile": new tr( "Mobiil-ID", "Mobile-ID", "Mobiil-ID" ),
	"inputActivateMobile": new tr( "Aktiveeri Mobiil-ID teenus", "Activate Mobile-ID", "Активируй услугу Mobiil-ID" ),
	"inputCheckMobile": new tr( "Kontrolli Mobiil-ID staatust", "Check Mobile-ID status", "" ),
	"inputPUK": new tr( "PUK kood", "PUK code", "PUK код" ),
	
	"inputChange": new tr( "Muuda", "Change", "Измени" ),
	"inputCancel": new tr( "Tühista", "Cancel", "Отменить" ),
	"inputChangePIN1": new tr( "Muuda PIN1", "Change PIN1", "Поменять PIN1" ),
	"inputChangePIN2": new tr( "Muuda PIN2", "Change PIN2", "Поменять PIN2" ),
	"inputChangePUK": new tr( "Muuda PUK", "Change PUK", "Поменять PUK" ),
	"inputCertDetails": new tr( "Vaata üksikasju", "View details", "Просмотреть детали" ),
	"inputUnblock": new tr ( "Tühista blokeering", "Revoke blocking", "Отменить блокировку" ),
	
	"labelCurrentPIN1": new tr( "Vana PIN1 kood", "Current PIN1 code", "Старый PIN1 код" ),
	"labelNewPIN1": new tr( "Uus PIN1 kood", "New PIN1 code", "Новый PIN1 код" ),
	"labelNewPIN12": new tr( "Uus PIN1 kood uuesti", "Repeat new PIN1 code", "Новый PIN1 код заново" ),
	"labelCurrentPIN2": new tr( "Vana PIN2 kood", "Current PIN2 code", "Старый PIN2 код" ),
	"labelNewPIN2": new tr( "Uus PIN2 kood", "New PIN2 code", "Новый PIN2 код" ),
	"labelNewPIN22": new tr( "Uus PIN2 kood uuesti", "Repeat new PIN2 code", "Новый PIN2 код заново" ),
	"labelCurrentPUK": new tr( "Vana PUK kood", "Current PUK code", "Старый PUK код" ),
	"labelNewPUK": new tr( "Uus PUK kood", "New PUK code", "Новый PUK код" ),
	"labelNewPUK2": new tr( "Uus PUK kood uuesti", "Repeat new PUK code", "Новый PUK код заново" ),
	"labelPUK": new tr( "PUK kood", "PUK code", "PUK код" ),
	
	"labelEmailAddress": new tr( "E-posti aadress kuhu suunatakse Teie @eesti.ee kirjad", "Email addres where your @eesti.ee emails will be forwarded", "Адрес эл. почты, куда перенаправляют Вашу почту с @eesti.ee" ),
	"labelEmailUrl": new tr( "Täiuslikuma ametliku e-posti suunamise häälestamisvahendi leiad portaalist", "", "Более полную информации о настройке пересылки эл. почты найдёте в портале" ),
	
	"labelMobile": new tr( "Mobiil-ID on võimalus kasutada isikutuvastamiseks ja digitaalallkirja andmiseks ID-kaardi asemel mobiiltelefoni.", "", "Mobiil-ID это возможность идентифицировать личность и давать подпись мобильным телефоном, вместо ID- карты." ),
	"labelMobile2": new tr( "Mobiil-ID kasutamiseks on vajalik uus SIM-kaart, mille sa saad omale mobiilsideoperaatori käest. Kui selline kaart on sul juba olemas, tuleb teenus aktiveerida.", "", "Для использования Mobiil-ID нужна новая SIM- карта, которую можно получить у мобильного оператора. Если такая карта уже есть, то услугу следует активировать." ),
	"labelMobileReadMore": new tr( "Loe täpsemalt id.ee kodulehelt", "", "Более подробно на страничке id.ee" ),
	"mobileNumber": new tr( "Mobiili number", "Mobile number", "" ),
	"mobileOperator": new tr( "Mobiili operaator", "Mobile operator", "" ),
	"mobileStatus": new tr( "Staatus", "Mobile status", "" ),
	
	"errorFound": new tr( "Tekkis viga: ", "Error occured: ", "Возникла ошибка:" ),
	"loadEmail": new tr( "Laen e-posti seadeid", "Loading e-mail settings", "Загружаю настройки эл. почты" ),
	"activatingEmail": new tr( "Aktiveerin e-posti seadeid", "Activating e-mail settings", "настройки эл. почты" ),
	"forwardFailed": new tr( "E-posti suunamise aktiveerimine ebaõnnestus.", "Failed activating e-mail forwards.", "Активация перенаправления с эл. почты провалилась." ),
	"loadFailed": new tr( "E-posti aadresside laadimine ebaõnnestus.", "Failed loading e-mail settings.", "Активация перенаправления с эл. почты провалилась" ),
	"emailEnter": new tr( "E-posti aadress sisestamata!", "Enter e-mail address!", "Введите адрес эл. почты!" ),
	"loadPicture": new tr( "Lae pilt", "Load picture", "Загрузить картинку" ),
	"savePicture": new tr( "salvesta", "save", "сохранить" ),
	"savePicFailed": new tr( "Pildi salvestamine ebaõnnestus!", "Saving picture failed!", "Сохранение картинки неуспешно!" ),
	"loadPic": new tr( "Laen pilti", "Loading picture", "Сохранение картинки!" ),
	"loadPicFailed": new tr( "Pildi laadimine ebaõnnestus!", "Loading picture failed!", "Загрузка картинки неуспешна!" ),
	"loadPicFailed2": new tr( "Pildi laadimine ebaõnnestus - tundmatu pildiformaat!", "Loading picture failed - unknown picture format!", "Загрузка картинки неуспешна- неизвестный формат!" ),
	"loadPicFailed3": new tr( "Pildi laadimine ebaõnnestus - viga salvestamisel!", "Loading picture failed - error saving file!", "Загрузка картинки неуспешна- ошибка при сохранении!" ),
	"loadCardData": new tr( "Loen andmeid", "Reading data", "Данные считваются" )
};

//codes from eesti.ee
var eestiStrings = {
	"0":  new tr( "Toiming õnnestus", "Success", "Выполнение успешно!" ),
	"1":  new tr( "ID-kaart pole väljastatud riiklikult tunnustatud sertifitseerija poolt.", "", "ID- карта не была выдана разрешённым сертифицирующим органом." ),
	"2":  new tr( "Sisestati vale PIN-kood, katkestati PIN-koodi sisestamine, tekkisid probleemid sertifikaatidega või puudub ID-kaardi tugi brauseris.","", "Ввели неверный PIN код, прервали введение PIN кода, возникли проблемы с сертификатами или отсутствует поддержка ID- карты в браузере." ),
	"3":  new tr( "ID-kaardi sertifikaat ei kehti.", "", "Сертификат ID- карты недействителен." ),
	"4":  new tr( "Sisemine on lubatud ainult Eesti isikukoodiga.", "", "Вход разрешён только с эстонским личным кодом." ),
	"10": new tr( "Tundmatu viga.", "Unknown error", "Неизвестная ошибка." ),
	"11": new tr( "KMA päringu tegemisel tekkis viga.", "" ,"В запросе КМА возникла ошибка." ),
	"12": new tr( "Äriregistri päringu tegemisel tekkis viga.", "", "В запросе к Äriregister возникла ошибка." ),
	"20": new tr( "Ühtegi ametliku e-posti suunamist ei leitud.", "", "Не было найдено ни одной официальной пересылки эл. почты." ),
	"21": new tr( "Teie e-posti konto on suletud. Avamiseks saatke palun e-kiri aadressil toimetaja@eesti.ee või helistage telefonil 663 0215.", "", "Ваша учётная запись эл. почты закрыта. Для открытия пошлите письмо на toimetaja@eesti.ee или звоните по телефону 663 0215." ),
	"22": new tr( "Vigane e-posti aadress.", "Invalid e-mail address", "Неверный адрес эл. почты." ),
	"23": new tr( "Suunamine on salvestatud, ning Teile on saadetud kiri edasisuunamisaadressi aktiveerimisvõtmega. Suunamine on kasutatav ainult pärast aktiveerimisvõtme sisestamist.", "", "Переадресация сохранена и Вам послано письмо с ключом активации. Переадресация активна только после введения ключа." )
};

var eidStrings = {
	"noCard": new tr( "Ei leitud ühtegi ID-kaarti", "No card found", "Не найдена ID- карта" ),
	"noReaders": new tr( "Ühtegi kiipkaardi lugejat pole ühendatud", "No readers found", "Не обнаружено считывающее устройство" ),
	"certValid": new tr( "kehtiv ja kasutatav", "valid and applicable", "действительно и пригодно" ),
	"certBlocked": new tr( "kehtetu", "expired", "недействительно" ),
	"validBlocked": new tr( "kehtiv kuid blokeeritud", "valid but blocked", "действительно, но заблокировано" ),
	
	"PINCheck": new tr( "PIN1 ja PIN2 ei tohi sisaldada sünnikuupäeva ja -aastat", "PIN1 and PIN2 have to be different than date of birth or year of birth", "PIN1 и PIN2 не должны содержать дату рождения" ),
	"PIN1Enter": new tr( "Sisesta vana PIN1 kood", "Current PIN1 code", "Введите старый PIN1 код" ),
	"PIN1Length": new tr( "PIN1 pikkus peab olema 4-12 numbrit", "PIN1 length has to be between 4 and 12", "Длина PIN1 должна быть 4-12 номера" ),
	"PIN1InvalidRetry": new tr( "Vale PIN1 kood. Saad veel proovida %d korda.", "Wrong PIN1 code. You can try %d more times.", "Неверный PIN1 код. Попыток ещё: %d" ),
	"PIN1EnterNew": new tr( "Sisesta uus PIN1 kood", "Enter new PIN1 code", "Неверный PIN1 код. Попыток ещё" ),
	"PIN1Retry": new tr( "Korda uut PIN1 koodi", "Retry your new PIN1 code", "Повторите новый PIN1 код" ),
	"PIN1Different": new tr( "Uued PIN1 koodid on erinevad", "New PIN1 codes doesn't match", "Новые PIN1 коды не сходятся" ),
	"PIN1Changed": new tr( "PIN1 kood muudetud!", "PIN1 changed!", "PIN1 код изменён!" ),
	"PIN1Unsuccess": new tr( "PIN1 muutmine ebaõnnestus.", "Changing PIN1 failed", "Смена PIN1 кода неудачна." ),
	"PIN1UnblockFailed": new tr( "Blokeeringu tühistamine ebaõnnestus.\nUus PIN peab erinema eelmisest PINist!", "Unblock failed.\nYour new PIN1 has to be different than current!", "Снятие блокировки неуспешно.\nНовый PIN должен отличаться от старого!" ),
	"PIN1UnblockSuccess": new tr( "PIN1 kood on muudetud ja sertifikaadi blokeering tühistatud!", "PIN1 changed and you current sertificates blocking has been removed!", "PIN1 код изменён и сертификат разблокирован!" ),
	"PIN1Blocked": new tr( "PIN1 blokeeritud.", "PIN1 blocked", "PIN1 заблокирован." ),

	"PIN2Enter": new tr( "Sisesta vana PIN2 kood", "Current PIN2 code", "Введите старый PIN2 код" ),
	"PIN2Length": new tr( "PIN2 pikkus peab olema 5-12 numbrit", "PIN2 length has to be between 5 and 12", "Длина PIN2 должна быть 5-12 номера" ),
	"PIN2InvalidRetry": new tr( "Vale PIN2 kood. Saad veel proovida %d korda.", "Wrong PIN2 code. You can try %d more times.", "Неверный PIN2 код. Попыток ещё: %d" ),
	"PIN2NewDifferent": new tr( "Uued PIN2 koodid on erinevad.", "New PIN2 codes doesn't match", "Новые PIN2 коды не сходятся" ),
	"PIN2EnterNew": new tr( "Sisesta uus PIN2 kood.", "Enter new PIN2 code", "Неверный PIN2 код. Попыток ещё" ),
	"PIN2Retry": new tr( "Korda uut PIN2 koodi.", "Retry your new PIN2 code", "Повторите новый PIN2 код" ),
	"PIN2Different": new tr( "Uued PIN2 koodid on erinevad", "New PIN2 codes doesn't match", "Новые PIN2 коды не сходятся" ),
	"PIN2Changed": new tr( "PIN2 kood muudetud!", "PIN2 changed!", "PIN2 код изменён!" ),
	"PIN2Unsuccess": new tr( "PIN2 muutmine ebaõnnestus.", "Changing PIN2 failed", "Смена PIN2 кода неудачна." ),
	"PIN2UnblockFailed": new tr( "Blokeeringu tühistamine ebaõnnestus.\nUus PIN peab erinema eelmisest PINist!", "Unblock failed.\nYour new PIN2 has to be different than current!", "Снятие блокировки неуспешно.\nНовый PIN должен отличаться от старого!" ),
	"PIN2UnblockSuccess": new tr( "PIN2 kood on muudetud ja sertifikaadi blokeering tühistatud!", "PIN2 changed and you current sertificates blocking has been removed!", "PIN2 код изменён и сертификат разблокирован!" ),
	"PIN2Blocked": new tr( "PIN2 blokeeritud.", "PIN2 blocked", "PIN2 заблокирован." ),

	"PUKEnter": new tr( "Sisesta PUK kood.", "Enter PUK code.", "Введите PUK код" ),
	"PUKLength": new tr( "PUK koodi pikkus peab olema 8-12 numbrit.", "PUK length has to be between 8 and 12.", "Длина PUK должна быть 8-12 номера" ),
	"PUKEnterOld": new tr( "Sisesta vana PUK kood.", "Enter current PUK code.", "Введите старый PUK код" ),
	"PUKEnterNew": new tr( "Sisesta uus PUK kood.", "Enter new PUK code.", "Неверный PUK код. Попыток ещё" ),
	"PUKRetry": new tr( "Korda uut PUK koodi.", "Retry your new PUK code.", "Повторите новый PUK код" ),
	"PUKDifferent": new tr( "Uued PUK koodid on erinevad", "New PUK codes doesn't match", "Новые PUK коды не сходятся" ),
	"PUKChanged": new tr( "PUK kood muudetud!", "PUK changed!", "PUK код изменён!" ),
	"PUKUnsuccess": new tr( "PUK koodi muutmine ebaõnnestus!", "Changing PUK failed!", "Смена PUK кода неудачна!" ),
	"PUKInvalidRetry": new tr( "Vale PUK kood. Saad veel proovida 2 korda.", "Wrong PUK code. You can try %d more times.", "Неверный PUK код. Попыток ещё: %d" ),
	"PUKBlocked": new tr( "PUK kood blokeeritud.", "PUK blocked", "PUK заблокирован." )
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
	var trTags = document.getElementsByTagName('trTag');
	for( i=0;i<trTags.length;i++)
	{
		if ( (typeof trTags[i].getAttribute != "undefined") && trTags[i].getAttribute('trcode') != null )
			trTags[i].innerHTML = _( trTags[i].getAttribute('trcode'), trTags[i].innerHTML );
	}
	var iTags = document.getElementsByTagName('input');
	for( i=0;i<iTags.length;i++)
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
