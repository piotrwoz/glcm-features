#include <iostream>
using namespace std;

class NoOffsets : public std::exception {
public:
    std::string msg() {
        std::string exceptionMessage = "Zero offsets given. Can't calculate mean GLCM\n";
        return exceptionMessage;
    }
};