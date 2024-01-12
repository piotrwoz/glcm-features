#include <iostream>

class BadFeatureType : public std::exception {
public:
    std::string msg() {
        std::string exceptionMessage = "Unknown texture feature type.\n";
        return exceptionMessage;
    }
};