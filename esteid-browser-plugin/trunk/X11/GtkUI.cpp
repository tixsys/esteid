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

#include "GtkUI.h"

#include <stdio.h>
#include <iostream>

#include "X11/pininputdialog.h"
#include "X11/PluginWindowX11.h"
#include "X11/whitelistdialog.h"
#include "esteidAPI.h"

#include "debug.h"

#ifndef WHITELISTDIALOG_UI
#define WHITELISTDIALOG_UI "whitelistdialog.ui"
#endif


GtkUI::GtkUI(FB::AutoPtr<UICallbacks> cb)
    : PluginUI(cb),
      m_dialog_up(false)
{
    ESTEID_DEBUG("GtkUI intialized");

    Gtk::Main::init_gtkmm_internals();

    Glib::RefPtr<Gtk::Builder> refGlade = Gtk::Builder::create();
    // Load the GtkBuilder file
    try {
        refGlade->add_from_file(WHITELISTDIALOG_UI);
    } catch(const Glib::Error& ex) {
        std::cerr << ex.what() << std::endl;
    }

    refGlade->get_widget_derived("WhitelistDialog", m_whitelistDialog);

    if (m_whitelistDialog) {
        m_whitelistDialog->signal_response().connect( sigc::mem_fun(*this,
                    &GtkUI::on_whitelistdialog_response) );
    }
}


GtkUI::~GtkUI()
{
    ESTEID_DEBUG("~GtkUI()");
}


void GtkUI::PromptForSignPIN(std::string subject,
        std::string docUrl, std::string docHash,
        std::string pageUrl, int pinPadTimeout, bool retry, int tries)
{
    ESTEID_DEBUG("GtkUI::PromptForSignPIN()");

    if (m_dialog_up) {
        // Bring the window to the front
        m_pinInputDialog->present();
        return;
    }

    m_pinInputDialog = new PinInputDialog(PIN2, subject);
    if (m_pinInputDialog) {
        m_pinInputDialog->signal_response().connect( sigc::mem_fun(*this,
                    &GtkUI::on_pininputdialog_response) );
    } else {
        return;
    }

    m_pinInputDialog->show_all();
    m_dialog_up = true;
}

#ifdef SUPPORT_OLD_APIS
void GtkUI::WaitForPinPrompt() {
    if(m_dialog_up) m_pinInputDialog->run();
}
#endif

void GtkUI::ClosePinPrompt()
{
    ESTEID_DEBUG("GtkUI::ClosePinPrompt()");
}


void GtkUI::ShowPinBlockedMessage(int pin)
{
    ESTEID_DEBUG("GtkUI::ShowPinBlockedMessage()");

    if (m_dialog_up)
        return;

    Gtk::MessageDialog dialog("PIN2 blocked", false, Gtk::MESSAGE_WARNING);
    dialog.set_secondary_text("Please run ID-card Utility to unlock the PIN.");
    m_dialog_up = true;
    dialog.run();
    m_dialog_up = false;
}


void GtkUI::ShowSettings(PluginSettings &conf, std::string pageUrl)
{
    ESTEID_DEBUG("GtkUI::ShowSettings()");

    m_conf = &conf;

    if (!m_whitelistDialog)
        return;

    if (m_dialog_up) {
        // Bring the window to the front
        m_whitelistDialog->present();
        return;
    }

    if (pageUrl.length() > 0)
        m_whitelistDialog->setEntryText(pageUrl);
    m_whitelistDialog->addDefaultSites(conf.default_whitelist);
    m_whitelistDialog->addSites(conf.whitelist);

    m_whitelistDialog->show_all();
    m_dialog_up = true;
}


void GtkUI::on_pininputdialog_response(int response_id)
{
    ESTEID_DEBUG("GtkUI::on_pininputdialog_response()");

    std::string pin;

    ESTEID_DEBUG("GtkUI: hiding pinInputDialog");
    m_pinInputDialog->hide();
    m_dialog_up = false;

    if (response_id == Gtk::RESPONSE_OK) {
        pin = m_pinInputDialog->getPin();
        m_callbacks->onPinEntered(pin);
        ESTEID_DEBUG("GtkUI::on_pininputdialog_response(): got PIN");
    } else {
        ESTEID_DEBUG("GtkUI::on_pininputdialog_response(): cancelled");
        m_callbacks->onPinCancelled();
    }
}


void GtkUI::on_whitelistdialog_response(int response_id)
{
    ESTEID_DEBUG("GtkUI::on_whitelistdialog_response()");

    if (response_id == Gtk::RESPONSE_OK) {
        ESTEID_DEBUG("GtkUI: saving whitelist");
        m_conf->whitelist = m_whitelistDialog->getWhitelist();
        m_conf->Save();
    }

    ESTEID_DEBUG("GtkUI: hiding whitelist");
    m_whitelistDialog->hide();
    m_dialog_up = false;
}
