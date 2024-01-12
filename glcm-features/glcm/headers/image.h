#pragma once

#define MAX_PIXEL_VALUE 256
#define DEFAULT_GRAY_LEVELS_AMOUNT 8

#include "../headers/image.h"
#include "../headers/imageInfo.h"
#include "../exceptions/ImageNotFoundException.h"
#include "../exceptions/BadGrayLevels.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <filesystem>
#include <set>
#include <opencv2/opencv.hpp>

class Image {
private:
	std::string _path;
	cv::Mat _img;
	ImageInfo _imageInfo;

	bool isGrayLevelsAmountCorrect(int grayLevelsAmount);
	bool isImageSizesCorrect(int width, int height);
	void loadImageInGrayScale();
	void calculateOriginalGrayLevelsAmount();
	void reduceGrayLevels();
	bool isGrayLevelCorrect();
	bool isPixelCoordsCorrect(int i, int j);
	int convertDoubleValueToGrayScale(double value);
	int convertIntValueToGrayLevel(int value);

public:
	Image(std::string path, int grayLevelsAmount);
	Image(std::shared_ptr<Image> image);

	void setPixelValue(int i, int j, int value);
	void setPixelValue(int i, int j, double value);

	void saveImage();

	void displayImage();
	void printImage();

	ImageInfo getImageInfo();
	cv::Mat getImage();
	std::string getPath();

	void setImageName(std::string name);
};
