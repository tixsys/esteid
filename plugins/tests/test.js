var names = [ "firstName", "lastName", "middleName", "sex",
              "citizenship", "birthDate", "personalID", "documentID",
              "expiryDate", "placeOfBirth", "issuedDate", "residencePermit",
              "comment1", "comment2", "comment3", "comment4"
];

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
        stat.innerHTML = "Error: " + err;
        stat.style.background = "red";
    }
}

function clearPersonalData() {
    for(var i in names) {
        var e = document.getElementById("s" + names[i]);
        e.innerHTML = "Not Available";
    }
}

function inittest() {
    var e = document.getElementById("persData");
    for(var i in names) {
        e.innerHTML += '<tr><td>' + names[i] + '</td><td><span id="s' +
                       names[i] + '"></span></td></tr>';
    }

    clearPersonalData();

    // FIXME: Seda jama ei peaks vaja olema!
    if(!esteid) {
        if(!window.external) return;
        esteid = window.external;
    }

    var e = document.getElementById("eidver");
    try {
        e.innerHTML = esteid.getVersion();
    }
    catch(err) {
        e.innerHTML = "Error: " + err;
    }
    esteid.addEventListener("cardInsert", cardInserted);
    esteid.addEventListener("cardRemove", cardRemoved);
}

function testPersonalData() {
    readPersonalData();
}

function testSign() {
    var e = document.getElementById("signstatus");
    try {
        signedData = esteid.sign("0A0B0C0D",0);
        e.innerHTML = "OK: " + signedData;
        e.style.background = "";
    }
    catch(err) {
        e.innerHTML = "Error: " + err;
        e.style.background = "red";
    }
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
