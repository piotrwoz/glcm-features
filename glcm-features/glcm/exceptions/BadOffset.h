#include <iostream>

class BadOffset : public std::exception {
public:
    std::string msg() {
        std::string exceptionMessage = "Wrong offset for GLCM. Offset should be (1, 0), (0, 1), (1, 1) or (-1, 1) consider values as (dx, dy)\n";
        return exceptionMessage;
    }
};