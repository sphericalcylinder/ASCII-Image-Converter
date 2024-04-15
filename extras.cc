#include "extras.hpp"
#include "gtkmm/enums.h"
#include "settings.hpp"

/**
 * @file extras.cc
 * 
*/

/**
 *
 * The constructor for the SettingsWindow class. It sets up the window's
 * layout and widgets, and connects the signals to the appropriate functions.
 * 
*/
SettingsWindow::SettingsWindow() : vbox(Gtk::Orientation::VERTICAL), hbox(Gtk::Orientation::HORIZONTAL), close_button("Close"),
    max_scale_factor_adj(Gtk::Adjustment::create(10.0, 1.0, 100.0, 1.0, 5.0, 0.0)),
        max_scale_factor_label("Max Scale Factor:"), size_limit_button("Image Size Restricted\nBy Screen (Dangerous)"),
        dark_mode_button("Dark Mode") {


    set_title("Settings");
    set_default_size(300, 200);
    set_resizable(false);
    
    // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-custom-css-names.html 
    css_provider = Gtk::CssProvider::create();
    if (s.dark_mode)
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/darkstyle.css");
    else
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/lightstyle.css");
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider,
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_child(vbox);

    vbox.append(settings_label);
    settings_label.set_markup("<span line-height='1.5' size='large'>Here you can change the settings for the application.</span>");
    settings_label.set_wrap(true);

    vbox.append(hbox);
    hbox.set_hexpand(true);

    hbox.append(max_scale_factor_label);
    max_scale_factor_label.set_margin(5);

    hbox.append(max_scale_factor);
    max_scale_factor.set_adjustment(max_scale_factor_adj);
    max_scale_factor.set_hexpand(true);
    max_scale_factor.set_text("Max Scale Factor");
    max_scale_factor.set_digits(1);
    max_scale_factor.signal_value_changed().connect(sigc::mem_fun(*this, &SettingsWindow::max_scale_factor_changed));

    vbox.append(size_limit_button);
    size_limit_button.set_active(s.size_limit);
    size_limit_button.signal_toggled().connect(sigc::mem_fun(*this, &SettingsWindow::size_limit_toggled));

    vbox.append(dark_mode_button);
    dark_mode_button.set_active(s.dark_mode);
    dark_mode_button.signal_toggled().connect(sigc::mem_fun(*this, &SettingsWindow::dark_mode_toggled));

};

SettingsWindow::~SettingsWindow() {}


/**
 * @ingroup SignalFunctions
 *
 * Runs when the max_scale_factor spin button
 * is changed. It then updates the settings with
 * the new value.
 *
*/
void SettingsWindow::max_scale_factor_changed() {
    s.max_scale_factor = max_scale_factor.get_value();
}

/**
 * @ingroup SignalFunctions
 *
 * Runs when the size_limit_button is toggled.
 * It then updates the settings with the new value.
 *
*/
void SettingsWindow::size_limit_toggled() {
    s.size_limit = size_limit_button.get_active();
}

/**
 * @ingroup SignalFunctions
 * 
 * Runs when the dark_mode_button is toggled.
 * It then updates the settings with the new value.
*/
void SettingsWindow::dark_mode_toggled() {
    s.dark_mode = dark_mode_button.get_active();

    // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-custom-css-names.html
    if (s.dark_mode)
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/darkstyle.css");
    else
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/lightstyle.css");
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider,
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

/**
 * @ingroup SignalFunctions
 *
 * Runs when the close_button is clicked. It closes
 * the settings window.
 *
*/
void SettingsWindow::close_button_clicked() {
    this->close();
}


/**
 * 
 * The HelpWindow class constructor that initializes
 * all the variables and controls the UI layout
 * 
*/
HelpWindow::HelpWindow() : close_button("Close"), settings_button("Settings"),
                            vbox(Gtk::Orientation::VERTICAL), hbox(Gtk::Orientation::HORIZONTAL) {
    set_title("Help");
    set_default_size(600, 200);
    
    // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-custom-css-names.html
    css_provider = Gtk::CssProvider::create();
    if (s.dark_mode)
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/darkstyle.css");
    else
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/lightstyle.css");
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider,
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_child(vbox);
    vbox.append(hbox);

    hbox.append(close_button);
    close_button.set_margin(5);
    close_button.set_hexpand(true);
    close_button.signal_clicked().connect(sigc::mem_fun(*this, &HelpWindow::close_button_clicked));

    hbox.append(settings_button);
    settings_button.set_margin(5);
    settings_button.set_hexpand(true);
    settings_button.signal_clicked().connect(sigc::mem_fun(*this, &HelpWindow::settings_button_clicked));
    settings_window = 0;

    vbox.append(help_label);
    help_label.set_markup("<span line-height='1.5' size='large'> This application converts an image to an ASCII art representation, \
using the brightness of each pixel to determine the character to use.\n\nTo get started, select an image \
file to convert by pressing the 'Choose File' button. The current file will be displayed in the label \
below the button.\n\nYou can adjust the scale factor by changing the number to the right of the file name. \
The higher the scale factor, the smaller the image.\n\nNow, click 'Run' to start the conversion process. \
Depending on the size of the image and the scale factor, this process may take a while. The progress bar \
will show you how far along the process is.\n\nOnce the process is complete, you can either copy the raw text \
to the clipboard, or export it to an RTF file. You can also clear the text if you want to start over.\n\n\
You can click the settings button to change the settings for the application, or click the close button to \
go back to the main window. Enjoy!</span>");

    help_label.set_wrap(true);
    help_label.set_margin(10);

}

HelpWindow::~HelpWindow() {
    // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-custom-css-names.html
    if (s.dark_mode) {
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/darkstyle.css");
    } else {
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/lightstyle.css");
    }
}

/**
 * @ingroup SignalFunctions
 *
 * Runs when the close_button is clicked. It closes
 * the help window.
 *
*/
void HelpWindow::close_button_clicked() {
    this->close();
}

/**
 * @ingroup SignalFunctions
 *
 * Runs when the settings_button is clicked. It opens
 * the settings window.
 *
*/
void HelpWindow::settings_button_clicked() {
    if (settings_window != 0) {
        settings_window->present();
        return;
    }


    settings_window = new SettingsWindow();
    settings_window->signal_destroy().connect(sigc::mem_fun(*this, &HelpWindow::on_settings_window_close));
    settings_window->show();
}

/**
 * @ingroup SignalFunctions
 *
 * Runs when the settings_window is closed and
 * deletes the settings_window pointer.
 *
*/
void HelpWindow::on_settings_window_close() {
    delete settings_window;
    settings_window = 0;
    // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-custom-css-names.html
    if (s.dark_mode) {
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/darkstyle.css");
    } else {
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/lightstyle.css");
    }
    queue_draw();
    this->present();
}