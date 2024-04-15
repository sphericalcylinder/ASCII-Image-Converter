# Compilation
You must have gtkmm 4.0, pkg-config, a C++ 20 compiler, and ImageMagic installed to compile. No promises on Windows. Run ```make ascii``` and it should spit out an executable named ```ascii``` which you run. I didn't build failsafes for people who don't have these installed so please do or the program will crash. I developed it with Apple clang version 15, I assume it works with GCC, no idea how it works with MSVC.

[gtkmm 4.0](https://www.gtkmm.org/en/index.html) | [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/) | 
[ImageMagick](https://imagemagick.org)

# Documentation
Generate Doxygen documentation with ```doxygen Doxyfile```. It only generates HTML and the main page is index.html.

[Doxygen](https://www.doxygen.nl/index.html)

# Usage
Click Choose File to choose a file. The file you have chosen will appear in the top center of the window. Click Run to start converting the image, which might take a while depending on the image chosen. If it says the image is too large, increase the scale factor (the number in the corner). Once you like your image, you can copy the raw text or save it as an rtf file.
