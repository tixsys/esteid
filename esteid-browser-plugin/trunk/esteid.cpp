/*
 * esteid-browser-plugin - a browser plugin for Estonian EID card
 *
 * Copyright (C) 2010  Estonian Informatics Centre
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

/**********************************************************\

  Auto-generated esteid.cpp

  This file contains the auto-generated main plugin object
  implementation for the esteid project

\**********************************************************/

#include "NpapiTypes.h"
#include "esteidAPI.h"

#include "esteid.h"

logger eidlog("npesteid");

void esteid::StaticInitialize()
{
    // Place one-time initialization stuff here; note that there isn't an absolute guarantee that
    // this will only execute once per process, just a guarantee that it won't execute again until
    // after StaticDeinitialize is called
}

void esteid::StaticDeinitialize()
{
    // Place one-time deinitialization stuff here
}


esteid::esteid()
    : m_esteidAPI()
{
}

esteid::~esteid()
{
}

FB::JSAPI* esteid::createJSAPI()
{
    // m_host is the BrowserHostWrapper
    m_esteidAPI = new esteidAPI(m_host);
    return m_esteidAPI.ptr();
}

bool esteid::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
    return false;
}

bool esteid::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
    return false;
}

bool esteid::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
    return false;
}
bool esteid::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *)
{
    // The window is attached; act appropriately
    return false;
}

bool esteid::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
    // The window is about to be detached; act appropriately
    return false;
}