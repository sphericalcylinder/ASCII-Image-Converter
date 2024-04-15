#include <iostream>

#pragma once

/**
 * @file settings.hpp
 * 
*/

/**
 * @brief A struct to hold settings for the application
 *
 * A structure that holds various settings for the application
 * that can be modified.
 * 
 */
struct Settings {
    bool size_limit = true; ///< Whether the output text can be larger than the screen dimensions
    bool dark_mode = false;
    float max_scale_factor = 10.0; ///< The maximum scale factor for the output text
};

inline Settings s; ///< A global instance of the Settings struct
