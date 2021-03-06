/*
 * esteid-browser-plugin - a browser plugin for Estonian EID card
 *
 * Copyright (C) 2010  Estonian Informatics Centre
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

#ifndef CALLBACKAPI_H
#define CALLBACKAPI_H

#include <string>
#include <sstream>
#include "JSAPIAuto.h"
#include "BrowserHost.h"

/** A base class for implementing event listeners.
 * Objects of this type can be attached to DOM events like this:
 *    domObj.callMethod<FB::variant>("addEventListener", 
 *                FB::variant_list_of("click")(CallbackInstance)(false));
 *
 */
class CallbackAPI : public FB::JSAPIAuto
{
public:
    CallbackAPI(FB::BrowserHostPtr host);
    virtual ~CallbackAPI();
    virtual bool handleEvent(const FB::JSObjectPtr& evt);

    /** Override this */
    virtual bool eventHandler() = 0;

    virtual FB::variant Invoke(const std::string& methodName, const std::vector<FB::variant>& args);
    virtual bool HasMethod(const std::string& methodName);
private:
    FB::BrowserHostPtr m_host;
};

#endif //CALLBACKAPI_H
