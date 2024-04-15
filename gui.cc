#include "gui.hpp"
#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include "settings.hpp"

// Most code came from https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/index.html

/**
 * @file gui.cc
 * 
*/

/**
 * @defgroup SignalFunctions
 * A group to hold functions called by signals
 * from the UI.
 *
*/

/// An rtf file header with the correct settings for an ASCII art image
const std::string rtf_header = R"({\rtf1\ansi\ansicpg1252\cocoartf2761
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fnil\fcharset0 Menlo-Regular;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\margl1440\margr1440\vieww17700\viewh9340\viewkind0
\deftab720
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardeftab720\sl144\slmult1\pardirnatural\partightenfactor0

\f0\fs2 \cf0 )";
// I got the header myself, not from another source.

/**
 *
 * The GUI class constructor that initializes
 * all the variables and controls the UI layout
 *
*/
GUI::GUI() : vbox(Gtk::Orientation::VERTICAL), hbox1(Gtk::Orientation::HORIZONTAL, 5),
                    hbox2(Gtk::Orientation::HORIZONTAL, 5), hbox3(Gtk::Orientation::HORIZONTAL),
                    choose_file_button("Choose File"), run_button("Run"), currentfile("No file selected"),
                    scale_factor_adj(Gtk::Adjustment::create(1.0, 1.0, 10.0, 0.5, 3.0, 0.0)),
                    dispatcher(), worker(), worker_thread(nullptr), copy_button("Copy Text"), export_file_button("Export as RTF"),
                    clear_button("Clear"), help_button("Help") {
    
    set_title("ASCII Art");
    set_default_size(500, 300);

    // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-custom-css-names.html
    css_provider = Gtk::CssProvider::create();
    css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/lightstyle.css");
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider,
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    set_child(vbox);

    vbox.append(hbox1);
    vbox.append(hbox2);
    vbox.append(hbox3);

    hbox1.append(choose_file_button);
    choose_file_button.set_margin(5);
    choose_file_button.signal_clicked().connect(sigc::mem_fun(*this, &GUI::on_choose_file_button_clicked));
    choose_file_button.set_tooltip_text("Choose an image file to convert to ASCII art");

    hbox1.append(currentfile);
    currentfile.set_hexpand(true);

    hbox1.append(scale_factor);
    scale_factor.set_adjustment(scale_factor_adj);
    scale_factor.set_digits(1);
    scale_factor.set_margin(5);
    scale_factor.set_size_request(75, -1);
    scale_factor.set_tooltip_text("Scale factor for the emmitted ASCII art");
    scale_factor.signal_value_changed().connect(sigc::mem_fun(*this, &GUI::scale_factor_changed));
    auto entry = scale_factor.get_first_child();
    entry->set_sensitive(false); // Disables manual text entry

    hbox2.append(run_button);
    run_button.set_margin(5);
    run_button.set_hexpand(true);
    run_button.signal_clicked().connect(sigc::mem_fun(*this, &GUI::run_button_clicked));
    

    hbox2.append(progressbar);
    progressbar.set_margin(5);
    progressbar.set_valign(Gtk::Align::CENTER);
    progressbar.set_hexpand(true);
    progressbar.set_pulse_step(0.007);
    dispatcher.connect(sigc::mem_fun(*this, &GUI::on_notification));

    hbox2.append(clear_button);
    clear_button.set_margin(5);
    clear_button.set_hexpand(true);
    clear_button.signal_clicked().connect(sigc::mem_fun(*this, &GUI::clear_button_clicked));

    hbox3.append(copy_button);
    copy_button.set_margin(5);
    copy_button.set_hexpand(true);
    copy_button.set_tooltip_text("Copy the ASCII art to the clipboard");
    copy_button.signal_clicked().connect(sigc::mem_fun(*this, &GUI::copy_button_clicked));

    hbox3.append(export_file_button);
    export_file_button.set_margin(5);
    export_file_button.set_hexpand(true);
    export_file_button.set_tooltip_text("Export the ASCII art as an RTF file");
    export_file_button.signal_clicked().connect(sigc::mem_fun(*this, &GUI::on_export_button_clicked));

    vbox.append(textout);
    textout.set_expand(true);
    textout.set_margin(10);

    vbox.append(help_button);
    help_button.set_margin(5);
    help_button.set_hexpand(true);
    help_button.signal_clicked().connect(sigc::mem_fun(*this, &GUI::help_button_clicked));
    help_window = 0;

    update_buttons();
}


GUI::~GUI() {}

/**
 * @ingroup SignalFunctions
 *
 * Run when the GUI::choose_file_button is clicked and creates a file
 * chooser dialog with filter that filters image files.
 *
*/
void GUI::on_choose_file_button_clicked() {
    // turn off tooltips
    choose_file_button.set_has_tooltip(false);
    scale_factor.set_has_tooltip(false);
    copy_button.set_has_tooltip(false);
    export_file_button.set_has_tooltip(false);

    auto dialog = Gtk::FileDialog::create();

    auto filters = Gio::ListStore<Gtk::FileFilter>::create();

    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Image files");
    filter_text->add_mime_type("image/*");
    filters->append(filter_text);

    dialog->set_filters(filters);

    dialog->open(sigc::bind(sigc::mem_fun(*this, &GUI::on_choose_file_button_finished), dialog));
}


/**
 * @ingroup SignalFunctions
 *
 * Run when the file chooser dialog created by GUI::on_choose_file_button_clicked()
 * is closed and processes the file chosen.
 *
 * @param[in] result a Glib RefPtr to an AsyncResult passed by const reference.
 * @param[in] dialog a Glib RefPtr to the original file dialog that was opened.
 *
*/
void GUI::on_choose_file_button_finished(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog) {
    try {
        auto file = dialog->open_finish(result);

        auto filepath = file->get_path();
        auto name = file->get_basename();
        this->filename = filepath;
        currentfile.set_text(name);
    } catch (const Gtk::DialogError& err) {
        //std::cout << "No file selected" << std::endl;
        // file selection dialog cancelled
    } catch (const Glib::Error& err) {
        std::cout << "Error: " << err.what() << std::endl;
    }
    // turn tooltips back on
    choose_file_button.set_has_tooltip(true);
    scale_factor.set_has_tooltip(true);
    copy_button.set_has_tooltip(true);
    export_file_button.set_has_tooltip(true);
}

/**
 * @ingroup SignalFunctions
 *
 * Run when the GUI::clear_button is clicked and clears the
 * text in GUI::textout.
 *
*/
void GUI::clear_button_clicked() {
    textout.set_text(" ");
    set_default_size(500, 300);
}

/**
 * @ingroup SignalFunctions
 *
 * Sets the value of GUI::sfactor to the current
 * value of GUI::scale_factor.
 *
*/
void GUI::scale_factor_changed() {
    sfactor = scale_factor.get_value();
}

/**
 * @ingroup SignalFunctions
 *
 * Copies text from GUI::textout to the clipboard
 * if the first character isn't '-' (a minus sign)
 *
*/
void GUI::copy_button_clicked() {
    auto clipboard = Gtk::Widget::get_clipboard();
    if (textout.get_text()[0] != '-') {
        clipboard->set_text(textout.get_text());
    }
}

/**
 * @ingroup SignalFunctions
 * 
 * Opens a new HelpWindow and assigns it to GUI::help_window
 */
void GUI::help_button_clicked() {
    // https://stackoverflow.com/questions/15441157/gtkmm-multiple-windows-popup-window
    if (help_window != 0) {
        help_window->present();
        return;
    }

    help_window = new HelpWindow();
    help_window->signal_destroy().connect(sigc::mem_fun(*this, &GUI::on_help_window_close));
    help_window->show();
}


/**
 * @ingroup SignalFunctions
 *
 * Closes the help window and sets GUI::help_window to 0.
 * Also applies changes from the settings
 *
*/
void GUI::on_help_window_close() {
    // https://stackoverflow.com/questions/15441157/gtkmm-multiple-windows-popup-window
    delete help_window;
    help_window = 0;
    this->present();

    //change the scale factor spinbutton range
    double min, max;
    scale_factor.get_range(min, max);
    scale_factor.set_range(min, s.max_scale_factor);

    // https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-custom-css-names.html
    if (s.dark_mode)
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/darkstyle.css");
    else
        css_provider->load_from_path("/Users/jonahposner/Documents/Code/c++/ascii/lightstyle.css");
    Gtk::StyleProvider::add_provider_for_display(get_display(), css_provider,
                                        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
}

/**
 * Takes some text and puts backslashes before
 * newlines or braces, so that it can be put
 * directly into an RTF file.
 *
 * @param[in] text Some text to make RTF compatible
 * @return The original made RTF compatible
 *
*/
std::string GUI::to_rtf(std::string text) {
    std::string out;
    for (char c : text) {
        if (c == '\n' || c == '}' || c == '{') {
            out += "\\";
        }
        out += c;
    }

    return out;
}

/**
 * @ingroup SignalFunctions
 *
 * Creates a save file dialog with a filter
 * for RTF files.
 *
*/
void GUI::on_export_button_clicked() {
    auto dialog = Gtk::FileDialog::create();

    auto filters = Gio::ListStore<Gtk::FileFilter>::create();

    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name("Rich Text Format files");
    filter_text->add_mime_type("text/rtf");
    filters->append(filter_text);

    dialog->set_filters(filters);


    dialog->save(sigc::bind(sigc::mem_fun(*this, &GUI::on_export_button_finished), dialog));
}

/**
 * @ingroup SignalFunctions
 *
 * Run when the file export dialog created by GUI::on_export_button_clicked()
 * is closed saves the text in GUI::textout to an rtf file. The text is converted
 * to rtf format by GUI::to_rtf().
 *
 * @param[in] result a Glib RefPtr to an AsyncResult passed by const reference
 * @param[in] dialog a Glib RefPtr to the original file dialog that was opened
 *
*/
void GUI::on_export_button_finished(const Glib::RefPtr<Gio::AsyncResult>& result, const Glib::RefPtr<Gtk::FileDialog>& dialog) {
    try {
        auto file = dialog->save_finish(result);

        auto filepath = file->get_path();
        
        std::ofstream outtext(filepath, std::fstream::out | std::fstream::trunc);
        outtext << rtf_header << to_rtf(textout.get_text()) << "}" << std::endl;
        outtext.close();
    } catch (const Gtk::DialogError& err) {
        //std::cout << "No file selected" << std::endl;
    } catch (const Glib::Error& err) {
        std::cout << "Error: " << err.what() << std::endl;
    }
}

/**
 * If the GUI::worker_thread is running, disable
 * the GUI::run_button, GUI::copy_button, and
 * GUI::export_file_button buttons so sneaky users
 * can't break my stuff.
 *
*/
void GUI::update_buttons() {
    const bool thread_is_running = worker_thread != nullptr;

    run_button.set_sensitive(!thread_is_running);
    copy_button.set_sensitive(!thread_is_running);
    export_file_button.set_sensitive(!thread_is_running);
}

/**
 * Sets the fraction done of the GUI::progressbar by getting
 * the fraction from GUI::worker.
 *
*/
void GUI::update_progress() {
    double donefrac;
    worker.get_working_data(&donefrac);

    if (donefrac < 1.0) {
        progressbar.set_fraction(donefrac);
    } else {
        progressbar.set_fraction(1.0);
    }
}

/**
 * Calls 'pulse' on the GUI::progressbar to make a
 * bar bounce back and forth.
 *
*/
void GUI::pulse_pbar() {
    progressbar.pulse();
}


/**
 * @ingroup SignalFunctions
 *
 * Calls 'emit' on the GUI::dispatcher which runs the GUI::on_notification()
 * function.
 *
*/
void GUI::notify() {
    dispatcher.emit();
}

/**
 * Called when GUI::dispatcher's emit() function is called and updates
 * the UI by calling GUI::update_progress(). If the GUI::worker is
 * finished, the GUI::worker_thread is joined and text is displayed
 *
*/
void GUI::on_notification() {
    if (worker_thread && worker.has_stopped()) {
        if (worker_thread->joinable()) 
            worker_thread->join();
        delete worker_thread;
        worker_thread = nullptr;
        update_buttons();
        update_progress();
        Glib::ustring text;
        worker.get_final_data(&text);
        std::string temp(text.c_str());
        if (temp[0] == '-') {
            textout.set_markup("<span font_desc='Helvetica 15'>"+temp.substr(1,temp.size())+"</span>");
            set_default_size(500, 300);
        } else {
            textout.set_markup("<span font_desc='Menlo 1.7' line_height='0.4'>"+std::string(g_markup_escape_text(temp.c_str(), temp.size()))+"</span>");
            set_default_size(1, 1);
        }

    }
    update_progress();
}


/**
 * @ingroup SignalFunctions
 *
 * Creates a new thread assigned to GUI::worker_thread and
 * calls Worker::work() inside the new thread
 *
*/
void GUI::run_button_clicked() {
    textout.set_text(""); // clear the textout
    gdk_monitor_get_geometry( // gets the screen dimensions
        gdk_display_get_monitor_at_surface(gdk_display_get_default(), 
        gdk_surface_new_toplevel(gdk_display_get_default())), &this->rect);

    
    if (worker_thread) { 
        std::cout << "worker thread already running" << std::endl;
    } else {
        worker_thread = new std::thread (
            [this] {
                worker.work(this, filename, sfactor, rect.width, rect.height, s);
            }
        );
    }

    update_buttons(); 
}