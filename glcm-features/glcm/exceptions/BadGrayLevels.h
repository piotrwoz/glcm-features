#include <iostream>

class BadGrayLevels : public std::exception {
public:
    std::string msg() {
        std::string exceptionMessage = "Wrong new gray level. It should be in range <0, 256> and be lower than original gray levels\n";
        return exceptionMessage;
    }
};