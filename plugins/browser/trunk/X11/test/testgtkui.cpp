#include "testgtkui.h"
#include "pininput.h"
#include "whitelistdialog.h"
#include <gtkmm/messagedialog.h>
#include <iostream>


TestGtkUI::TestGtkUI()
    : m_pinBlockedButton("Show PIN blocked dialog"),
      m_pinInputButton("Show PIN input dialog"),
      m_whitelistButton("Show PIN input dialog")
{
    set_title("Plugin UI test");

    add(m_ButtonBox);

    m_ButtonBox.pack_start(m_pinBlockedButton);
    m_pinBlockedButton.signal_clicked().connect(sigc::mem_fun(*this,
            &TestGtkUI::on_button_pinBlocked_clicked));

    m_ButtonBox.pack_start(m_pinInputButton);
    m_pinInputButton.signal_clicked().connect(sigc::mem_fun(*this,
            &TestGtkUI::on_button_pinInput_clicked) );

    m_ButtonBox.pack_start(m_whitelistButton);
    m_whitelistButton.signal_clicked().connect(sigc::mem_fun(*this,
            &TestGtkUI::on_button_whitelist_clicked) );

    if (loadGladeUI("whitelist.glade") != 0) {
        printf("loadGladeUI() failed\n");
        exit(1);
    }

    m_refGlade->get_widget_derived("WhitelistDialog", m_whitelistDialog);

    show_all_children();
}


TestGtkUI::~TestGtkUI()
{
}


int TestGtkUI::loadGladeUI(std::string gladeFile)
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


void TestGtkUI::on_button_pinBlocked_clicked()
{
    Gtk::MessageDialog dialog(*this, "PIN2 blocked", false, Gtk::MESSAGE_WARNING);
    dialog.set_secondary_text("Please run ID-card Utility to unlock the PIN.");

    dialog.run();
}


void TestGtkUI::on_button_pinInput_clicked()
{
    PinInput dialog(PIN1, "Mari-Liis Männik");

    int result = dialog.run();

    //Handle the response:
    switch (result) {
    case Gtk::RESPONSE_OK:
        std::cout << "X: OK clicked." << std::endl;
        std::cout << "X: PIN is " << dialog.getPin() << std::endl;
        break;
    case Gtk::RESPONSE_CANCEL:
        std::cout << "X: Cancel clicked." << std::endl;
        std::cout << "X: PIN is " << dialog.getPin() << std::endl;
        break;
    case Gtk::RESPONSE_CLOSE:
        std::cout << "X: Close clicked." << std::endl;
        std::cout << "X: PIN is " << dialog.getPin() << std::endl;
        break;
    default:
        std::cout << "X: Unexpected button clicked." << std::endl;
        std::cout << "X: PIN is " << dialog.getPin() << std::endl;
        break;
    }
}


void TestGtkUI::on_button_whitelist_clicked()
{
    m_whitelistDialog->run();
}
