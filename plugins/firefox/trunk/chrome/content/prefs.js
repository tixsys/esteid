var params = window.arguments[0];

function onLoad() {
  var elt = document.getElementById('whitelist');
  for(var i = 0; i < params.wl.length; i++)
    elt.appendItem(params.wl[i]);

  if(params.host) {
    var elt = document.getElementById('addhost');
    elt.value = params.host;
  }

  var elt = document.getElementById('log');
  elt.value = params.log;
}

function removeSelected() {
  var elt = document.getElementById('whitelist');
  var i = elt.selectedIndex;
  if(i != -1) elt.removeItemAt(i);
}

function enableDelBtn() {
  var elt = document.getElementById('whitelist');
  var btn = document.getElementById('delbtn');
  if(elt.selectedIndex == -1)
    btn.disabled = true;
  else
    btn.disabled = false;
}

function addHost() {
  var elt = document.getElementById('addhost');
  var wl = document.getElementById('whitelist');
  if(elt.value) wl.appendItem(elt.value);
  elt.value = '';
}

function doOK() {
  var wl = document.getElementById('whitelist');
  params.wl.length = 0;
  
  for(var i = 0; i < wl.itemCount; i++) {
    params.wl.push(wl.getItemAtIndex(i).label);
  }
}
