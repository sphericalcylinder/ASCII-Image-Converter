#include "worker.hpp"
#include "gui.hpp"
#include <mutex>

/**
 * @file worker.cc
 * 
*/


// whole file:
// https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/sec-multithread-example.html

/***/
Worker::Worker() :
    mutex(),
    will_stop(false),
    stopped(true),
    donefrac(0.0),
    message()
{}

/**
 * Sets the fraction of the amount that the 
 * progressbar is filled
 *
 * @param[in,out] donefrac a pointer to the progressbar fraction
 *
*/
void Worker::get_working_data(double *donefrac) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (donefrac)
        *donefrac = this->donefrac;
}

/**
 * Sets the final message when the Worker::work()
 * function is finished.
 *
 * @param[in,out] message a pointer to the final message
 *
*/
void Worker::get_final_data(Glib::ustring *message) const {
    std::lock_guard<std::mutex> lock(mutex);
    if (message)
        *message = this->message;
}

/**
 * Stops Worker::work() by setting the Worker::will_stop
 * variable to true.
 *
*/
void Worker::stop() {
    std::lock_guard<std::mutex> lock(mutex);
    will_stop = true;
}

/**
 * Gets the value of Worker::stopped and returns it
 *
 * @return the value of Worker::stopped
 *
*/
bool Worker::has_stopped() const {
    std::lock_guard<std::mutex> lock(mutex);
    return stopped;
}

