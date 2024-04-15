#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ios>
#include <iostream>
#include <fstream>
#include <iterator>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <regex>

#include "gui.hpp"
#include "worker.hpp"
#include "settings.hpp"

/**
 * @file ascii.cpp
 *
*/

/**
 * @mainpage ASCII Art Converter
 *
 * @section intro_sec Introduction
 * 
 * This program is a simple ASCII art converter that takes an image file and
 * converts it to an ASCII art representation. It uses <a href="https://imagemagick.org">
 * ImageMagick </a> to convert the input image and <a href="https://www.gtk.org">Gtk</a>
 * (specifically <a href="https://www.gtkmm.org/en/index.html">gtkmm</a> for C++) for
 * the %GUI.
 *
 * @section usage_sec Usage
 *
 * First, click the 'Choose File' button to select an image file to convert.
 * Once you've done this, you'll see the name of your file to the right of
 * the 'Choose File' button. Whenever the program runs, this is the image
 * that will be converted. You can now click the 'Run' button to try running
 * the program. If it says your image is too large, increase the scale factor
 * number in the top right corner. The higher the scale factor, the smaller
 * the image. For larger images, the conversion process might take some time.
 * You can gauge the progress by looking at the progress bar next to the run
 * button. If a bar is bouncing back and forth, the program is loading the
 * file. When the bar is full, your image should be displayed in the center of
 * the window. A commom problem is that the progress bar is full, but the text
 * is not being displayed. This should only happen for a few seconds and it's
 * the %GUI trying to display the text. Right above the image there are two options,
 * one to copy the text and another to save it to an RTF file. If you click
 * 'Copy Text', the text will be copied to your clipboard for you to paste
 * elsewhere. If you click 'Export as RTF', you will be prompted to save an
 * RTF file containing the image text. If you're not here, you can click the
 * 'Help' button at the bottom of the window to get pretty much these exact
 * same instructions and access Advanced %Settings.
 * 
*/


// `.':_,^=;><+!rc*/z?sLTv)J7|Fi{C}fI31tlu[neoZ5Yxa2EwkP6h9d4VOGbUAKXHm8RD#$Bg0MNWQ%&@

/// An array of ASCII characters from dark to light, each repeated thrice
constexpr char ascii_sub[255]={' ',' ',' ','`','`','`','.','.','.','\'', '\'', '\'', ':', ':', ':','_', '_', '_',',', ',', ',',
'^', '^', '^','=', '=', '=',';', ';', ';','>','>', '>','<', '<', '<','+', '+', '+','!', '!',
'!','r', 'r', 'r','c', 'c', 'c','*', '*', '*','/', '/', '/','z', 'z', 'z','?', '?', '?','s',
's', 's','L', 'L', 'L','T', 'T', 'T','v', 'v','v',')', ')', ')','J', 'J', 'J','7', '7', '7',
'|', '|', '|','F', 'F', 'F','i', 'i', 'i','{','{', '{','C', 'C', 'C', '}', '}','f', 'f',
'f','I', 'I', 'I','3', '3', '3','1', '1', '1','t', 't', 't','l', 'l', 'l','u', 'u', 'u','[',
'[', '[','n', 'n', 'n','e', 'e', 'e','o', 'o','o','Z', 'Z', 'Z','5', '5', '5','Y', 'Y', 'Y',
'x', 'x', 'x','j', 'j','y', 'y','a', 'a', 'a','2', '2', '2','E', 'E', 'E','w', 'w', 
'w','k', 'k', 'k','P', 'P', 'P','6', '6', '6','h', 'h', 'h','9', '9', '9','d', 'd', 'd','4', 
'4', '4','V', 'V', 'V','O', 'O', 'O','G', 'G', 'G','b', 'b', 'b','U', 'U', 'U','A', 'A', 'A',
'K', 'K', 'K','X', 'X', 'X','H', 'H', 'H','m', 'm', 'm','8', '8', '8','R', 'R', 'R','D', 'D', 
'D','#', '#', '#','$', '$', '$','B', 'B', 'B','g', 'g', 'g','0', '0', '0','M', 'M', 'M','N', 
'N', 'N','W', 'W', 'W','Q', 'Q', 'Q','%', '%', '%','&', '&', '&','@', '@', '@'};
// from stackoverflow (https://stackoverflow.com/questions/30097953/ascii-art-sorting-an-array-of-ascii-characters-by-brightness-levels-c-c)

/**
 * @brief Sanitizes the input to prevent command injection
 *
 * Takes a file path and removes any characters that aren't
 * alphanumeric and semicolons.
 *
 * @param[in] input the file path string to sanitize
 * @return the sanitized input
 *
*/
std::string sanitizeInput(std::string input) {
    std::regex pattern("(?![A-Za-z0-9_.]+)(;)"); // matches anything that's not A-Z, a-z, 0-9, _, or . and explicitly matches ;
    
    input = std::regex_replace(input, pattern, "");

    // a regex pattern that matches a space
    std::regex space(" ");
    input = std::regex_replace(input, space, "\\ "); // replace spaces with \ to escape them

    return input;
}

/**
 * @brief Creates a pgm file from an image file
 *
 * Takes a filename, sanitizes it with sanitizeInput(), and
 * uses ImageMagick to convert it to a .pgm file
 *
 * @attention The user must have ImageMagick installed
 *
 * @param[in] filename the name of the file to convert to a pgm file
 *
*/
void create_pgm(std::string filename) {
    std::string sanitized = sanitizeInput(filename);
    
    std::string command = "magick " + sanitized + " -compress none out.pgm";
    
    std::system(command.c_str());
}

/**
 * Reads the pgm file and stores it in a string
 *
 * @param[in,out] image the string to store the pgm file in
 * @param[in,out] gui the GUI object to update the progress bar
 * @param[in,out] mutex the mutex to lock the progress bar
 *
*/
void get_pgm(std::string &image, GUI *gui, std::mutex &mutex) {
    /*std::ifstream inimage("out.ppm");
    std::stringstream file;
    file << inimage.rdbuf();
    inimage.close();
    image = file.str();*/
    // good code

    // bad code, but progressbar!
    std::ifstream inimage("out.pgm");
    std::string line;
    int wait = 0;
    gui->pulse_pbar();

    while (std::getline(inimage, line)) {
        image += line + "\n";
        wait++;
        if (wait == 100) {
            wait = 0;
            gui->pulse_pbar();
        }
    }
}

/**
 * @brief Trims the pgm file to get the width and height of the image,
 * along with the destination width and height
 *
 * Takes a string with the contents a .pgm file and various references
 * and removes the first 3 lines while getting the width and height
 *
 * @param[in] image the string to trim
 * @param[in,out] width the width of the image
 * @param[in,out] height the height of the image
 * @param[in,out] destw the destination width of the image
 * @param[in,out] desth the destination height of the image
 * @param[in] scale_factor the scale factor to scale the image by
 * @return the trimmed string
 *
*/
std::string trim_file(std::string image, int &width, int &height, int &destw, int &desth, float scale_factor) {
    image.erase(0, 3); // erase the magic number

    int space = image.find(' ');
    int newline = image.find('\n');
    if (space != std::string::npos) {
        width = std::stoi(image.substr(0,space));
        height = std::stoi(image.substr(space+1,newline));
    }
    image.erase(0, newline+5); // erase the weird '255' and 2 newlines

    destw = width/scale_factor;
    desth = height/scale_factor;

    return image;
}


std::string filenamecache = "";

/**
 * @brief Parses the pgm file and stores the luminance values in a 2D vector
 * 
 * Takes a string with the contents of a .pgm file and various references
 * and stores the luminance values in a 2D vector
 * 
 * @param[in,out] gui the GUI object to update the progress bar while working
 * @param[in] image the trimmed .pgm file string to parse
 * @param[in] width the width of the image
 * @param[in] height the height of the image
 * @param[in,out] mutex the mutex ??
 * @param[in,out] donefrac the fraction of the progress bar that's filled
 * @param[in] progress_frac the fraction to fill the progress bar by
 * @param[in,out] will_stop a boolean to stop running
 * @param[in,out] stopped a boolean to tell if the program is stopped
 * @return a 2D vector with the luminance values
 *
*/
std::vector<std::vector<int>> parse_file(GUI *gui, std::string image, int width, int height,
                std::mutex &mutex, double &donefrac, double progress_frac, bool &will_stop, bool &stopped) {

    std::vector<std::vector<int>> lum_map = {{}}; // 2D vector to store the luminance values

    std::istringstream iss(image);
    std::string token;
    int heightcount = 0;
    int widthcount = 0;

    while (std::getline(iss, token, ' ')) { // get each number from the pgm file
        if (widthcount == width) { // if the width has been reached
            widthcount = 0; // reset width
            lum_map.push_back(std::vector<int>{}); // add a new row to the image
            heightcount++; // add one to the height
            {
                std::lock_guard<std::mutex> lock(mutex); // lock mutex and check if the program should stop
                if (will_stop) {
                    stopped = true;
                    break;
                }

                donefrac += progress_frac; // update the progress
            }
            gui->notify();
        }
        widthcount++;
        if (token == "\n" || token == " ") { // if the token is a newline or space, ignore it
            continue;
        }
        int num = std::stoi(token);
        num = std::clamp(num, 0, 254); // clamp the number to 254 (the max value for the ascii array)
        lum_map[heightcount].push_back(num); // add the color to the image
    }

    return lum_map;
}


/**
 * @brief Does the conversion from image to ASCII
 *
 * Takes a file path to an image and a scale factor and
 * converts the image to ASCII letters.
 *
 * @param[in,out] gui the GUI object to update the progress bar while working
 * @param[in] filename the file path to the image to convert
 * @param[in] scale_factor the scale factor to scale the image by
 * @param[in] swidth the width of the screen
 * @param[in] sheight the height of the screen
 *
*/
void Worker::work(GUI *gui, std::string filename, float scale_factor, int swidth, int sheight, Settings &s) {
    {
        std::lock_guard<std::mutex> lock(mutex); // lock mutex and set variables
        stopped = false;
        donefrac = 0.0;
        message = "";
    }

    if (filename == "") { // if no file is selected, set message and return
        {
            std::lock_guard<std::mutex> lock(mutex);
            message = "-Please select an image";
            stopped = true;
        }
        gui->notify();
        return;
    }
    if (filenamecache != filename) { // generate a new pgm file if the filename has changed
        filenamecache = filename;
        create_pgm(filename);
    }

    std::string image;
    get_pgm(image, gui, mutex); // get the pgm file contents

    int width, height, destw, desth; // image width and height

    image = trim_file(image, width, height, destw, desth, scale_factor);

    if ((destw > swidth-50 || desth > sheight-280) && s.size_limit) { // if the image is too large to display, set message and return
        {
            std::lock_guard<std::mutex> lock(mutex);
            message = "-Image is too large to be displayed on the screen\nTry increasing the scale factor.";
            stopped = true;
        }
        gui->notify();
        return;
    }
    if (destw <= 0 || desth <= 0) { // if the scale factor is invalid, set message and return
        {
            std::lock_guard<std::mutex> lock(mutex);
            message = "-Invalid scale factor.";
            std::cout << scale_factor << std::endl;
            std::cout << destw << " " << desth << std::endl;
            stopped = true;
        }
        gui->notify();
        return;
    }

    if (stopped) { // if the program has been stopped, return
        return;
    }

    double progress_frac = 1.0/height;
    

    std::vector<std::vector<int>> lum_map = {{}}; // 2D vector to store the luminance values

    lum_map = parse_file(gui, image, width, height, mutex, donefrac, progress_frac, will_stop, stopped);

    std::vector<std::vector<int>> scaled_lum_map(desth, std::vector<int>(destw));

    if (scale_factor == 1.0) {
        scaled_lum_map = lum_map;
    } else {
        // uses bilinear interpolation to scale the image
        // https://chao-ji.github.io/jekyll/update/2018/07/19/BilinearResize.html
        int v1, v2, v3, v4;
        int xlow, xhigh, ylow, yhigh;
        int px;
        int xweight, yweight;
        bool stop = false;

        float xratio = (float)(width-1)/(destw-1);
        float yratio = (float)(height-1)/(desth-1);

        if (xratio * (destw-1) > width-1) {
            xratio = (float)(width-2)/(destw-1);
        }
        if (yratio * (desth-1) > height-1) {
            yratio = (float)(height-2)/(desth-1);
        }

        for (int h = 0; h < desth; h++) {
            for (int w = 0; w < destw; w++) {
                xlow = (int)floor(w*xratio);
                xhigh = (int)ceil(w*xratio);

                ylow = (int)floor(h*yratio);
                yhigh = (int)ceil(h*yratio);

                xweight = (xratio * w) - xlow;
                yweight = (yratio * h) - ylow;

                v1 = lum_map[ylow][xlow];
                v2 = lum_map[ylow][xhigh];
                v3 = lum_map[yhigh][xlow];
                v4 = lum_map[yhigh][xhigh];

                px = v1 * (1 - xweight) * (1 - yweight) +
                    v2 * xweight       * (1 - yweight) +
                    v3 * (1 - xweight)       * yweight +
                    v4 * xweight             * yweight;

                scaled_lum_map[h][w] = px;
            }
        }
    }

    char ascii[255];
    std::copy(std::begin(ascii_sub), std::end(ascii_sub), std::begin(ascii));
    // copies the ascii_sub array to the ascii array

    if (!s.dark_mode) {
        std::reverse(std::begin(ascii), std::end(ascii));
    }

    std::ostringstream oss;

    for (auto row : scaled_lum_map) {
        for (auto col : row) {
            oss << ascii[col];
        }
        oss << "\n";
    }
    oss << std::endl;

    {
        std::lock_guard<std::mutex> lock(mutex);
        will_stop = false;
        stopped = true;
        message = oss.str();
    }
    gui->notify();
}


int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create("org.gtkmm.example");

    /// Shows the window and returns when it is closed.
    return app->make_window_and_run<GUI>(argc, argv);
    
    return 0;
}