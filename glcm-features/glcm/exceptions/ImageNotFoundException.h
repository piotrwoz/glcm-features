#include <iostream>

class ImageNotFoundException : public std::exception {
private: 
    std::string _imagePath;
public:
    ImageNotFoundException(std::string imagePath) {
        this->_imagePath = imagePath;
    }
    
    std::string msg() {
        std::string exceptionMessage = "Can't load image file at " + this->_imagePath + "\n";
        return exceptionMessage;
    }
};