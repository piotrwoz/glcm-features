#pragma once

#include <string>
#include <vector>

struct ImageInfo {
    std::string imageName;
    std::string extension;
    unsigned int width;
    unsigned int height;
    unsigned int originalGrayLevelsAmount;
    unsigned int grayLevelsAmount;
    std::vector<unsigned int> grayLevels;
};