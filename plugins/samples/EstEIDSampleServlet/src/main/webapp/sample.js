var esteid;
var xmlHttp;
var userHandler = null;

function ajaxStateHandler() {
    if(xmlHttp.readyState==4) {
        var res;
        if(xmlHttp.status==200) {
            res = xmlHttp.responseText;
            if(userHandler != null) {
                userHandler(res);
            }
        } else {
            res = "Error loading page"+ xmlHttp.status +":"+ xmlHttp.statusText;
            enableSign();
            results(res);
        }
    }
}

function doPostRequest(url, data, callback) {
    userHandler = callback;

    if(window.ActiveXObject) {
        xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
    } else if(window.XMLHttpRequest) {
        xmlHttp = new XMLHttpRequest();
    }
    xmlHttp.onreadystatechange=ajaxStateHandler;
    xmlHttp.open("POST", url, true);
    xmlHttp.setRequestHeader("Content-Type",
            "application/x-www-form-urlencoded; charset=UTF-8");
    xmlHttp.send(data);
}

function sleep(millis) {
    var now = new Date();
    var start = now.getTime();
    do {
        now = new Date();
        var end = now.getTime();
    } while(end - start < millis);
}

function results(msg) {
    document.getElementById("result").innerHTML = msg;
}

function enableSign() {
    document.getElementById("signbutton").disabled = false;
}

function disableSign() {
    document.getElementById("signbutton").disabled = true;
}

function initMe() {
    esteid = document.getElementById("esteid");
    try {
        esteid.getVersion();
    } catch(e) {
        document.getElementById("signcontrols").innerHTML =
            "ID-kaardi tugi puudub";
    }
}

function signButton() {
    disableSign();
    results("Reading Certificate ... ");
    // Schedule certificate read to timer so our button will not freeze
    setTimeout('signMe()',100);
}

function signMe() {
    var pem = null;
    try {
        pem = esteid.signCert.cert;
    } catch(e) {
        results("ERROR reading card");
        enableSign();
    }
    if(pem != null) {
        disableSign();
        results("Preparing document ... ");
        doPostRequest("signservlet?mode=prep", "cert=" + pem, handlePrep);
    }
}

function handlePrep(res)
{
    var hash = res;
    if(hash.substr(0, 6) == "ERROR:") {
        enableSign();
    } else {
        var sigHex = null;
        try {
            sigHex = esteid.sign(hash, "signservlet?mode=sdoc");
        } catch(e) {
            res = "ERROR in signing: " + e;
            enableSign();
        }
        if(sigHex != null) {
            res = "Sending signature back to server ...";
            doPostRequest("signservlet?mode=sign",
                          "sigHex=" + sigHex, handleSign);
        }
    }
    document.getElementById("result").innerHTML=res;
}

function handleSign(res)
{
    results(res);
    enableSign();
}
