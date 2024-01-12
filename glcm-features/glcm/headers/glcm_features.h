#pragma once

#define DEFAULT_WINDOW_SIZE 7

#include "../headers/glcm.h"
#include "../headers/image.h"
#include "../exceptions/badFeatureType.h"

#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <opencv2/opencv.hpp>

enum FeatureType {
	ENERGY,
	ENTROPY,
	CONTRAST,
	HOMOGENEITY
};

class GLCM_features {
private:
	std::shared_ptr<Image> _image;
	std::vector<unsigned int> _greyLevels;
	unsigned int _windowSize;

	void validWindowSize(unsigned int windowSize);
	bool checkIfWindowSizeOdd(unsigned int windowSize);
	bool checkIfWindowSizeWithinImageSizes(unsigned int windowSize);
	std::tuple<int, int, int, int> setStartingWindowParams();
	void calcFeatureFromGLCM(std::pair<int, int> offset, FeatureType featureType);
	void calcFeatureFromGLCM(std::vector<std::pair<int, int>> offsets, FeatureType featureType);
	double calcEnergy(std::unique_ptr<GLCM>& glcm);
	double calcEntropy(std::unique_ptr<GLCM>& glcm);
	double calcContrast(std::unique_ptr<GLCM>& glcm);
	double calcHomogeneity(std::unique_ptr<GLCM>& glcm);
	std::string createFeatureTypeImageName(FeatureType featureType);
	std::string stringifyFeatureType(FeatureType featureType);

public:
	GLCM_features(std::shared_ptr<Image> image, unsigned int windowSize = DEFAULT_WINDOW_SIZE);

	void energy(std::pair<int, int> offset);
	void energy(std::vector<std::pair<int, int>> offsets);
	void entropy(std::pair<int, int> offset);
	void entropy(std::vector<std::pair<int, int>> offsets);
	void contrast(std::pair<int, int> offset);
	void contrast(std::vector<std::pair<int, int>> offsets);
	void homogeneity(std::pair<int, int> offset);
	void homogeneity(std::vector<std::pair<int, int>> offsets);
};
