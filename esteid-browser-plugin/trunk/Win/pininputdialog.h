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

#ifndef ESTEID_PININPUTDIALOG_H
#define ESTEID_PININPUTDIALOG_H

#include <windows.h>
#include <string>
#include <boost/signals.hpp>

#include "Win/basedialog.h"

class PluginSettings;

class PinInputDialog : public BaseDialog
{
public:
    typedef boost::signal<void (bool)>  signal_t;
    typedef boost::signals::connection  connection_t;

public:
    PinInputDialog(HINSTANCE hInst);
    virtual ~PinInputDialog();

    connection_t connect(signal_t::slot_function_type subscriber)
    {
        return m_signalResponse.connect(subscriber);
    }

    void disconnect(connection_t subscriber)
    {
        subscriber.disconnect();
    }

    void showPinBlocked();
    void showWrongPin(bool tries);
    void setSubject(const std::string& subject);
    std::string getPin();
    void clearPin();

    bool doDialog();

private:
    LRESULT on_initdialog(WPARAM wParam);
    LRESULT on_command(WPARAM wParam, LPARAM lParam);
    LRESULT on_message(UINT message, WPARAM wParam, LPARAM lParam);

    std::string m_subject;
    size_t m_minPinLength;
    signal_t m_signalResponse;
};

#endif //ESTEID_PININPUTDIALOG_H