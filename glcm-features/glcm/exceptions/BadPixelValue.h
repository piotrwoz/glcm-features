#include <iostream>

class BadPixelValue : public std::exception {
public:
    std::string msg() {
        std::string exceptionMessage = "Wrong image's pixel value. Can't find it in the array of existing values\n";
        return exceptionMessage;
    }
};