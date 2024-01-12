#pragma once

#include "../headers/image.h"
#include "../exceptions/BadOffset.h"
#include "../exceptions/BadPixelValue.h"
#include "../exceptions/NoOffsets.h"

#include <iostream>
#include <vector>
#include <utility>
#include <opencv2/opencv.hpp>

class GLCM {
private:
	std::shared_ptr<Image> _image;
	std::shared_ptr<std::shared_ptr<double[]>[]> _glcm;
	std::vector<unsigned int> _greyLevels;
	unsigned int _size;

	void clearGLCM();
	bool checkOffset(std::pair<int, int> offset);
	unsigned int findGreyLevelValueInVector(unsigned int value);
	void makeGLCMHorizontal();
	void normalizeGLCM();
	std::unique_ptr<std::unique_ptr<double[]>[]> createTransponedGLCM();
	void addIntermediateGLCM(std::shared_ptr<std::shared_ptr<double[]>[]> glcm);

public:
	GLCM(std::shared_ptr<Image> image);

	void calculateGLCM(std::pair<int, int> offset, bool horizontal = true);
	void calculateMeanGLCM(std::vector<std::pair<int, int>> offsets, bool horizontal = true);

	void calculateGLCM(std::pair<int, int> offset, int top, int left, int windowSize, bool horizontal = true);
	void calculateMeanGLCM(std::vector<std::pair<int, int>> offsets, int top, int left, int windowSize, bool horizontal = true);
	
	void printGLCM(unsigned int coutPrecision = 0);

	std::shared_ptr<std::shared_ptr<double[]>[]> getGLCM();
	int getSize();
	std::shared_ptr<Image> getImage();
};
