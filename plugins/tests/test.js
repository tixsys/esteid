var names = [ "firstName", "lastName", "middleName", "sex",
              "citizenship", "birthDate", "personalID", "documentID",
              "expiryDate", "placeOfBirth", "issuedDate", "residencePermit",
              "comment1", "comment2", "comment3", "comment4"
];

function inittest() {
    var e = document.getElementById("persData");
    for(var i in names) {
        var r = e.insertRow(e.rows.length);
        var el = document.createElement('span');
	el.id = "s" + names[i];
	r.insertCell(0).innerHTML = names[i];
	r.insertCell(1).appendChild(el);
    }

    // FIXME: Seda jama ei peaks vaja olema!
    if(typeof(esteid) == 'undefined') {
        if(!window.external) return;
        esteid = window.external;
    }

    var e = document.getElementById("eidver");
    try {
        e.innerHTML = esteid.getVersion();
	e.style.background = "green";
    }
    catch(err) {
        e.innerHTML = "Error: " + err.message;
	e.style.background = "red";
    }
    try {
        esteid.addEventListener("cardInsert", cardInserted);
        esteid.addEventListener("cardRemove", cardRemoved);
    }
    catch(err) {
        var e = document.getElementById("cardstatus");
        e.innerHTML = "Error: " + err.message;
        e.style.background = "red";
    }
}

function cardInserted(reader) {
    var e = document.getElementById("cardstatus");
    e.innerHTML = "Sees";
    e.style.background = "green";
}
function cardRemoved(reader) {
    var e = document.getElementById("cardstatus");
    e.innerHTML = "Väljas";
    e.style.background = "yellow";
    clearPersonalData();
}

function readPersonalData() {
    var stat = document.getElementById("pdstatus");
    try {
        for(var i in names) {
            var e = document.getElementById("s" + names[i]);
            e.innerHTML = esteid[names[i]];
        }
        stat.innerHTML = "OK";
        stat.style.background = "";
    }
    catch(err) {
        clearPersonalData();
        stat.innerHTML = "Error: " + err.message;
        stat.style.background = "red";
    }
}

function clearPersonalData() {
    for(var i in names) {
        var e = document.getElementById("s" + names[i]);
        e.innerHTML = "Not Available";
    }
}

function testPersonalData() {
    readPersonalData();
}

function testSign() {
    var e = document.getElementById("signstatus");
    try {
        var certList = esteid.getCertificateList();
        var certArr = certList.split(",");
        var cert= esteid.getCertificateByThumbprint(certArr[1]);
        signedData = esteid.sign("0A0B0C0D",cert);
        //signedData = esteid.sign("0A0B0C0D",0);
        e.innerHTML = "OK: " + signedData;
        e.style.background = "";
    }
    catch(err) {
        e.innerHTML = "Error: " + err.message;
        e.style.background = "red";
    }
}

function runCrashCode(code) {
    var e = document.getElementById("crashstatus");
    try {
        eval(code);
	e.innerHTML += '<span class="red">' + code + " did not throw exception</span>";
    }
    catch(err) {
        e.innerHTML += code + ": " + err.message;
    }
    e.innerHTML += "<br />";
}

function testCrash() {
    var e = document.getElementById("crashstatus");
    e.innerHTML = "";
    runCrashCode('esteid.addEventListener(null, null);');
    runCrashCode('esteid.addEventListener("huinamuina", null);');
    runCrashCode('esteid.addEventListener("", null);');
    runCrashCode('esteid.addEventListener("cardInsert", null);');
    runCrashCode('esteid.addEventListener("cardRemove", "");');
    runCrashCode('esteid.addEventListener("cardRemove", 0);');
    runCrashCode('esteid.getCertificateByThumbprint(null);');
    runCrashCode('esteid.getCertificateByThumbprint(0);');
    runCrashCode('esteid.getCertificateByThumbprint("");');
    runCrashCode('esteid.sign(null, null);');
    runCrashCode('esteid.sign("","");');
    runCrashCode('esteid.sign(0,0);');
    runCrashCode('esteid.sign("",0);');
}

function testWhatever() {
    var certList = signer.getCertificateList();
    var certArr = certList.split(",");
    for(i = 0; i < certArr.length; i++){
       dbgArea.value+= certArr[i] + "\r";
    }
    if (certArr.length > 1 ) {
        var cert= signer.getCertificateByThumbprint(certArr[1]);
        if (cert) {
            dbgArea.value+= cert.CN + "\r" + cert.issuerCN + "\r" ;
            signer.sign("blobofdata",cert);
        }
    }
}
