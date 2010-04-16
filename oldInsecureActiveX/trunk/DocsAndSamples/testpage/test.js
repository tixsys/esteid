var esteid = null;
var bgok = "rgb(50, 255, 50)";
var bgerr = "rgb(255, 50, 50)";

var names = [ "firstName", "lastName", "middleName", "sex",
              "citizenship", "birthDate", "personalID", "documentID",
              "expiryDate", "placeOfBirth", "issuedDate", "residencePermit",
              "comment1", "comment2", "comment3", "comment4"
];

function initpdatatable() {
    var e = document.getElementById("persData");
    for(var i in names) {
        var r = e.insertRow(e.rows.length);
        var el = document.createElement('span');
	el.id = "s" + names[i];
	r.insertCell(0).innerHTML = names[i];
	r.insertCell(1).appendChild(el);
    }
}

var cnames = [ "CN", "issuerCN", "validFrom", "validTo",
              "keyUsage", "serial" //, "isValid"
];


function inittest() {
    esteid = document.getElementById("esteid");

    var e = document.getElementById("eidver");
    try {
        e.innerHTML = esteid.getInfo();
	e.style.background = bgok;
    }
    catch(err) {
        e.innerHTML = "Error: " + err.message;
	e.style.background = bgerr;
    }
}

function readCertificateData(n) {
    var stat = document.getElementById(n + "CertStatus");
    try {
        var cert = null;
        cert = esteid.getSigningCertificate();
        var e = document.getElementById(n + "CertDump");
        // Stupid IE will forget preformatting when assigning innerHTML
        // so we have to add another set of tags here.
        e.innerHTML = "<pre>" + cert + "</pre>";
        stat.innerHTML = "OK";
        stat.style.background = "";
    }
    catch(err) {
        clearCertificateData(n);
        stat.innerHTML = "Error: " + err.message;
        stat.style.background = bgerr;
    }
}

function clearCertificateData(n) {
    var e = document.getElementById(n + "CertDump");
    e.innerHTML = "Not Available";
}

function testSign(n) {
    var e = document.getElementById(n + "CertStatus");
    var hash = document.getElementById("testhash").value;
    var url = "https://id.smartlink.ee/plugin_tests/test.txt";
    try {
        signedData = esteid.getSignedHash(hash, esteid.selectedCertNumber);
        if (signedData) {
            e.innerHTML = "OK: " + signedData;
            e.style.background = "";
        } else {
            e.innerHTML = "Error: User pressed Cancel?";
            e.style.background = bgerr;
        }
    }
    catch(err) {
        e.innerHTML = "Error: " + err.message;
        e.style.background = bgerr;
    }
}
