#include "GtkUI.h"

#include <stdio.h>
#include <iostream>

#include "X11/pininputdialog.h"
#include "X11/PluginWindowX11.h"
#include "X11/whitelistdialog.h"

#define ESTEID_DEBUG printf

#ifndef GLADE_FILE
#define GLADE_FILE "whitelist.ui"
#endif


GtkUI::GtkUI():
    m_dialog_up(false)
{
    ESTEID_DEBUG("GtkUI intialized\n");

    Gtk::Main::init_gtkmm_internals();

    if (loadGladeUI(GLADE_FILE) != 0) {
        ESTEID_DEBUG("GtkUI::GtkUI(): loadGladeUI() failed\n");
    }

    m_refGlade->get_widget_derived("WhitelistDialog", m_whitelistDialog);

    if (m_whitelistDialog) {
        m_whitelistDialog->signal_response().connect( sigc::mem_fun(*this,
                    &GtkUI::on_whitelistdialog_response) );
    }
}


GtkUI::~GtkUI()
{
    ESTEID_DEBUG("~GtkUI()\n");
}


int GtkUI::loadGladeUI(std::string gladeFile)
{
    //Load the GtkBuilder file and instantiate its widgets:
    m_refGlade = Gtk::Builder::create();
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        m_refGlade->add_from_file(gladeFile);
    } catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    } catch(const Gtk::BuilderError& ex) {
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    }
#else
    std::auto_ptr<Glib::Error> error;

    if (!m_refGlade->add_from_file(gladeFile, error)) {
        std::cerr << error->what() << std::endl;
        return 1;
    }
#endif /* !GLIBMM_EXCEPTIONS_ENABLED */

    return 0;
}


std::string GtkUI::PromptForSignPIN(std::string subject,
        std::string docUrl, std::string docHash,
        std::string pageUrl, int pinPadTimeout, bool retry, int tries)
{
    ESTEID_DEBUG("GtkUI::PromptForSignPIN()\n");

    if (m_dialog_up)
        return "";

    std::string pin;

    PinInputDialog dialog(PIN2, subject);
    m_dialog_up = true;
    int rv = dialog.run();
    m_dialog_up = false;

    if (rv == Gtk::RESPONSE_OK) {
        pin = dialog.getPin();
        ESTEID_DEBUG("GtkUI::PromptForSignPIN(): PIN is '%s'\n", pin.c_str());
    } else {
        ESTEID_DEBUG("GtkUI::PromptForSignPIN(): cancelled\n");
    }

    return pin;
}


void GtkUI::ClosePinPrompt()
{
    ESTEID_DEBUG("GtkUI::ClosePinPrompt()\n");
}


void GtkUI::ShowPinBlockedMessage(int pin)
{
    ESTEID_DEBUG("GtkUI::ShowPinBlockedMessage()\n");

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
    ESTEID_DEBUG("GtkUI::ShowSettings()\n");

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


void GtkUI::on_whitelistdialog_response(int response_id)
{
    ESTEID_DEBUG("GtkUI::on_whitelistdialog_response()\n");

    if (response_id == Gtk::RESPONSE_OK) {
        ESTEID_DEBUG("GtkUI: saving whitelist\n");
        m_conf->whitelist = m_whitelistDialog->getWhitelist();
        m_conf->Save();
    }

    ESTEID_DEBUG("GtkUI: hiding whitelist\n");
    m_whitelistDialog->hide();
    m_dialog_up = false;
}
