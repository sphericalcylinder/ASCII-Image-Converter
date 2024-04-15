#include "settings.hpp"
#include <gtkmm.h>
#include <thread>
#include <mutex>

#pragma once

class GUI;

// whole file
// https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-multithread-example.html

/**
 * @file worker.hpp
 * 
*/

/**
 * @brief A class to run in a seperate thread and do work
 *
 * A class that holds variables that can be accessed
 * by both threads.
 *
*/
class Worker {
    public:
        Worker(); ///< The Worker class constructor, initializes variables

        /// A function to do the conversion from image to ASCII
        void work(GUI *gui, std::string filename, float scale_factor, int swidth, int sheight, Settings &s);

        void get_working_data(double *donefrac) const; ///< A function to get data while Worker::work() is running
        void get_final_data(Glib::ustring *message) const; ///< A function to get the resulting text of Worker::work()
        void stop(); ///< A function to stop Worker::work()
        bool has_stopped() const; ///< A const function that returns the value of Worker::stopped

    private:
    
        mutable std::mutex mutex; ///< A mutex, whatever that is

        bool will_stop; ///< A boolean to alert Worker::work() to stop
        bool stopped; ///< A boolean to tell if Worker::work() is stopped
        double donefrac; ///< The fraction of the GUI::progressbar that's filled
        Glib::ustring message; ///< The text that Worker::work() returns
};
