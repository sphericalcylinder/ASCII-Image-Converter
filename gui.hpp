#include "gtkmm/gestureclick.h"
#include "worker.hpp"
#include "extras.hpp"
#include <gtkmm.h>
#include <iostream>
#include <string>
#include "settings.hpp"

#pragma once
// Most code came from https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/index.html
// https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-multithread-example.html

/**
 * @file gui.hpp
 * 
*/

// f6f5f4
/** 
 * @brief A class to control the main window
 * 
 * A class that is the main window for the application
 * and holds all the widgets and signal response functions
 *
*/
class GUI : public Gtk::Window {
    public:
        GUI(); ///< The constructor for the GUI class. 
        virtual ~GUI(); ///< The GUI class destructor 
        void notify(); ///< A public function to signal the GUI::dispatcher 
        void pulse_pbar(); ///< A public function to pulse the GUI::progressbar 
        std::string to_rtf(std::string text); ///< A public function to make text RTF format compatible

    protected:
        void mouse_clicked(int numpresses, double x, double y); ///< A function to disable right clicks
        void run_button_clicked(); ///< A function called when the GUI::run_button is clicked that starts Worker::work() in a new thread
        void clear_button_clicked(); ///< A function called when the GUI::clear_button is clicked that clears the text in GUI::textout
        void scale_factor_changed(); ///< A function called when the GUI::scale_factor is changed 
        void on_choose_file_button_clicked(); ///< A function called when the GUI::choose_file_button button is clicked 

        /// A function called when the file dialog closes
        void on_choose_file_button_finished(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog);
        void on_export_button_clicked(); ///< A function called when the GUI::export_file_button is clicked 

        /// A function called when the export file dialog closes 
        void on_export_button_finished(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog);
        void copy_button_clicked(); ///< A function to copy the text in GUI::textout to the clipboard 

        void help_button_clicked(); ///< A function to open the help window
        void on_help_window_close(); ///< A function to close the help window

        void update_progress(); ///< A function to update the GUI::progressbar when the GUI::worker is running 
        void update_buttons(); ///< A function to enable or disable UI buttons 
        void on_notification(); ///< A function to update the UI and manage the GUI::worker_thread 

        Gtk::Box vbox, hbox1, hbox2, hbox3; ///< Invisible UI box to control layout
        Glib::RefPtr<Gtk::CssProvider> css_provider; ///< A CSS provider to style the UI

        Gtk::Button choose_file_button; ///< A button to choose an image file
        Gtk::Label currentfile; ///< A label to display the name of the current file

        Gtk::SpinButton scale_factor; ///< A 'spinbutton' to control the scale factor from 1-Settings.max_scale_factor
        Glib::RefPtr<Gtk::Adjustment> scale_factor_adj; ///< The adjustment to set the settings for the scale_factor
        float sfactor = 1; ///< A float variable to hold the scale factor value

        Gtk::Button run_button; ///< A button to start the conversion process
        Gtk::Button clear_button; ///< A button to clear the text in GUI::textout
        Gtk::ProgressBar progressbar; ///< A progressbar to display the progress of the conversion process
        Glib::Dispatcher dispatcher; ///< A dispatcher to signal when to update the main UI
        Worker worker; ///< A custom worker class to do work in a seperate thread
        std::thread* worker_thread; ///< The thread that the worker will run in

        std::string filename; ///< The name of the file that's being converted
        std::string text;
        Gtk::Label textout; ///< Where to put the generated ascii art text

        Gtk::Button copy_button, export_file_button; ///< A button to save the generated text
        Gtk::Button help_button; ///< A button to open the help window
        HelpWindow *help_window; ///< A pointer to the help window

        GdkRectangle rect; ///< A rectangle value to hold the display dimenstions
};