#include <iostream>
#include <gtkmm.h>
#include "settings.hpp"

#pragma once
// Most code came from https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/index.html

/**
 * @file extras.hpp
 * 
*/

// https://stackoverflow.com/questions/15441157/gtkmm-multiple-windows-popup-window
/**
 * @brief A class to control the settings window
 *
 * A class that is the settings window for the application,
 * which allows users to change certain settings for the application.
 * 
*/
class SettingsWindow : public Gtk::Window {
    public:
        SettingsWindow(); ///< The constructor for the SettingsWindow class
        ~SettingsWindow(); ///< The destructor for the SettingsWindow class

    protected:
        void close_button_clicked(); ///< A function to close the settings window
        void size_limit_toggled(); ///< A function to toggle the size limit setting
        void dark_mode_toggled(); ///< A function to toggle the dark mode setting
        void max_scale_factor_changed(); ///< A function to change the max scale factor setting

        Gtk::Box vbox, hbox; ///< Invisible UI box to control layout
        Glib::RefPtr<Gtk::CssProvider> css_provider; ///< A CSS provider to style the help window
        
        Gtk::Button close_button; ///< A button to close the settings window
        Gtk::Label settings_label; ///< A label that describes the settings window

        Gtk::CheckButton size_limit_button; ///< A button to toggle the size limit setting
        Gtk::CheckButton dark_mode_button; ///< A button to toggle the dark mode setting
        Gtk::Label max_scale_factor_label; ///< A label to describe the max scale factor setting
        Gtk::SpinButton max_scale_factor; ///< A button to change the max scale factor setting
        Glib::RefPtr<Gtk::Adjustment> max_scale_factor_adj; ///< The adjustment to set the settings for the max_scale_factor
};

// https://stackoverflow.com/questions/15441157/gtkmm-multiple-windows-popup-window
/**
 * @brief A class to control the help window
 *
 * A class that is the help window for the application,
 * so that users can understand how to use it. The SettingsWindow
 * window is opened from here.
 * 
*/
class HelpWindow : public Gtk::Window {
    public:
        HelpWindow(); ///< The constructor for the HelpWindow class
        ~HelpWindow(); ///< The destructor for the HelpWindow class

    protected:
        void close_button_clicked(); ///< A function to close the help window
        void settings_button_clicked(); ///< A function to open the settings window
        void on_settings_window_close(); ///< A function to close the settings window

        Gtk::Box vbox, hbox; ///< Invisible UI box to control layout
        Glib::RefPtr<Gtk::CssProvider> css_provider; ///< A CSS provider to style the help window

        Gtk::Button close_button; ///< A button to close the help window
        Gtk::Label help_label; ///< A label with help text

        Gtk::Button settings_button; ///< A button to open the settings window
        SettingsWindow *settings_window; ///< A pointer to the SettingsWindow window
};
