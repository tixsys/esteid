/*
 * esteid-browser-plugin - a browser plugin for Estonian EID card
 *
 * Copyright (C) 2010  Smartlink OÜ
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

function appendToBody(e) {
  var b = document.getElementsByTagName("body")[0];
  return b.appendChild(e);
}

function createEstEidObject(id) {
  var e = document.createElement('span');
  e.innerHTML = '<object style="width: 1px; height: 1px;" id="' + id +
                      '" type="application/x-esteid" />';
  e = appendToBody(e);

  return document.getElementById(id);
}

document.XMLSignApplet = createEstEidObject("SEBJavaSigner");
