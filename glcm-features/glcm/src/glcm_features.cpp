#include "../headers/glcm_features.h"

/*
Constructor of GLCM_features class.
@params image - successfully loaded image from disk.
@params - windowSize - defining size of swuare window used for GLCM calculations. Should be odd positive number. If not, default size will be used.
*/
GLCM_features::GLCM_features(std::shared_ptr<Image> image, unsigned int windowSize) {
	this->_image = image;
	this->validWindowSize(windowSize);
	this->_greyLevels = this->_image->getImageInfo().grayLevels;
}

/*
Check if given window size is valid - is it positive odd number that fits image sizes.
If not, default window size will be used.
*/
void GLCM_features::validWindowSize(unsigned int windowSize) {
	if (this->checkIfWindowSizeOdd(windowSize) &&
		this->checkIfWindowSizeWithinImageSizes(windowSize)) {
		this->_windowSize = windowSize;
	}
	else {
		std::cerr << "Wrong window size. It has to be positive odd number that fits image sizes.\n";
		this->_windowSize = DEFAULT_WINDOW_SIZE;
	}
}

bool GLCM_features::checkIfWindowSizeOdd(unsigned int windowSize) {
	if (windowSize % 2 == 1) {
		return true;
	}
	return false;
}

bool GLCM_features::checkIfWindowSizeWithinImageSizes(unsigned int windowSize) {
	if (windowSize > 0 && windowSize <= this->_image->getImageInfo().width && windowSize <= this->_image->getImageInfo().height) {
		return true;
	}
	return false;
}

/**
Create image of calculated energy from GLCM with given offset.
@param offset - pair representing offset. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window.
*/
void GLCM_features::energy(std::pair<int, int> offset) {
	std::tuple<int, int, int, int> windowStartingValues = this->setStartingWindowParams();
	this->calcFeatureFromGLCM(offset, ENERGY);
}

/**
Create image of calculated energy from GLCM with given vector of offsets
@param offsets - vector of pairs representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window
*/
void GLCM_features::energy(std::vector<std::pair<int, int>> offsets) {
	this->calcFeatureFromGLCM(offsets, ENERGY);
}

/**
Create image of calculated entropy from GLCM with given offset.
@param offset - pair representing offset. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window.
*/
void GLCM_features::entropy(std::pair<int, int> offset) {
	std::tuple<int, int, int, int> windowStartingValues = this->setStartingWindowParams();
	this->calcFeatureFromGLCM(offset, ENTROPY);
}

/**
Create image of calculated entropy from GLCM with given vector of offsets
@param offsets - vector of pairs representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window
*/
void GLCM_features::entropy(std::vector<std::pair<int, int>> offsets) {
	this->calcFeatureFromGLCM(offsets, ENTROPY);
}

/**
Create image of calculated contrast from GLCM with given offset.
@param offset - pair representing offset. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window.
*/
void GLCM_features::contrast(std::pair<int, int> offset) {
	std::tuple<int, int, int, int> windowStartingValues = this->setStartingWindowParams();
	this->calcFeatureFromGLCM(offset, CONTRAST);
}

/**
Create image of calculated contrast from GLCM with given vector of offsets
@param offsets - vector of pairs representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window
*/
void GLCM_features::contrast(std::vector<std::pair<int, int>> offsets) {
	this->calcFeatureFromGLCM(offsets, CONTRAST);
}

/**
Create image of calculated homogeneity from GLCM with given offset.
@param offset - pair representing offset. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window.
*/
void GLCM_features::homogeneity(std::pair<int, int> offset) {
	std::tuple<int, int, int, int> windowStartingValues = this->setStartingWindowParams();
	this->calcFeatureFromGLCM(offset, HOMOGENEITY);
}

/**
Create image of calculated homogeneity from GLCM with given vector of offsets
@param offsets - vector of pairs representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param meanGLCM - row index of left top element of window
*/
void GLCM_features::homogeneity(std::vector<std::pair<int, int>> offsets) {
	this->calcFeatureFromGLCM(offsets, HOMOGENEITY);
}

void GLCM_features::calcFeatureFromGLCM(std::pair<int, int> offset, FeatureType featureType) {
	std::tuple<int, int, int, int> windowStartingValues = this->setStartingWindowParams();
	int startingRow = std::get<0>(windowStartingValues);
	int maxRow = std::get<1>(windowStartingValues);
	int startingCol = std::get<2>(windowStartingValues);
	int maxCol = std::get<3>(windowStartingValues);
	std::unique_ptr<GLCM> glcm = std::make_unique<GLCM>(this->_image);
	std::unique_ptr<Image> textureFeatureImage = std::make_unique<Image>(glcm->getImage());
	std::string offsetAsString = "_offset(" + std::to_string(offset.first) + "," + std::to_string(offset.second) + ")";
	std::string windowSizeAsString = "_windowSize_" + std::to_string(this->_windowSize);
	std::string featureTypeAsString = this->stringifyFeatureType(featureType);
	std::string grayLevelsAsString = "_grayLevels_" + std::to_string(this->_image->getImageInfo().grayLevelsAmount);
	std::string newImageName = textureFeatureImage->getImageInfo().imageName + featureTypeAsString + offsetAsString + windowSizeAsString + grayLevelsAsString;
	textureFeatureImage->setImageName(newImageName);

	for (int i = startingRow; i < maxRow; i++) {
		for (int j = startingCol; j < maxCol; j++) {
			
			glcm->calculateGLCM(offset, i, j, this->_windowSize, false);

			double result = 0.0;
			switch(featureType) {
				case ENERGY:
					result = this->calcEnergy(glcm);
					break;
				case ENTROPY:
					result = this->calcEntropy(glcm);
					break;
				case CONTRAST:
					result = this->calcContrast(glcm);
					break;
				case HOMOGENEITY:
					result = this->calcHomogeneity(glcm);
					break;
				default:
					throw new BadFeatureType();
					break;
			}

			textureFeatureImage->setPixelValue(i + this->_windowSize / 2, j + this->_windowSize / 2, result);
		}
	}

	//textureFeatureImage->displayImage();
	textureFeatureImage->saveImage();
}

void GLCM_features::calcFeatureFromGLCM(std::vector<std::pair<int, int>> offsets, FeatureType featureType) {
	std::tuple<int, int, int, int> windowStartingValues = this->setStartingWindowParams();
	int startingRow = std::get<0>(windowStartingValues);
	int maxRow = std::get<1>(windowStartingValues);
	int startingCol = std::get<2>(windowStartingValues);
	int maxCol = std::get<3>(windowStartingValues);
	std::unique_ptr<GLCM> glcm = std::make_unique<GLCM>(this->_image);
	std::unique_ptr<Image> textureFeatureImage = std::make_unique<Image>(glcm->getImage());
	std::string offsetAsString = "_mean_offset";
	std::string windowSizeAsString = "_windowSize_" + std::to_string(this->_windowSize);
	std::string featureTypeAsString = this->stringifyFeatureType(featureType);
	std::string grayLevelsAsString = "_grayLevels_" + std::to_string(this->_image->getImageInfo().grayLevelsAmount);
	std::string newImageName = textureFeatureImage->getImageInfo().imageName + featureTypeAsString + offsetAsString + windowSizeAsString + grayLevelsAsString;
	textureFeatureImage->setImageName(newImageName);

	for(int i = startingRow; i < maxRow; i++) {
		for(int j = startingCol; j < maxCol; j++) {

			glcm->calculateMeanGLCM(offsets, i, j, this->_windowSize, false);

			double result = 0.0;
			switch(featureType) {
				case ENERGY:
					result = this->calcEnergy(glcm);
					break;
				case ENTROPY:
					result = this->calcEntropy(glcm);
					break;
				case CONTRAST:
					result = this->calcContrast(glcm);
					break;
				case HOMOGENEITY:
					result = this->calcHomogeneity(glcm);
					break;
				default:
					throw new BadFeatureType();
					break;
			}

			textureFeatureImage->setPixelValue(i + this->_windowSize / 2, j + this->_windowSize / 2, result);
		}
	}

	//textureFeatureImage->displayImage();
	textureFeatureImage->saveImage();
}

std::string GLCM_features::stringifyFeatureType(FeatureType featureType) {
	switch(featureType) {
		case ENERGY:
			return "_energy";
			break;
		case ENTROPY:
			return "_entropy";
			break;
		case CONTRAST:
			return "_contrast";
			break;
		case HOMOGENEITY:
			return "_homogeneity";
			break;
		default:
			throw new BadFeatureType();
			break;
	}
}

double GLCM_features::calcEnergy(std::unique_ptr<GLCM>& glcm) {
	double result = 0.0;
	for(int i = 0; i < glcm->getSize(); i++) {
		for(int j = 0; j < glcm->getSize(); j++) {
			result += std::pow(glcm->getGLCM()[i][j], 2);
		}
	}

	return result;
}

double GLCM_features::calcEntropy(std::unique_ptr<GLCM>& glcm) {
	double result = 0.0;
	for(int i = 0; i < glcm->getSize(); i++) {
		for(int j = 0; j < glcm->getSize(); j++) {
			double glcmValue = glcm->getGLCM()[i][j];
			if(glcmValue != 0.0) {
				result += glcmValue * std::log(glcmValue);
			}
		}
	}

	result *= -1.0;

	return result;
}

double GLCM_features::calcContrast(std::unique_ptr<GLCM>& glcm) {
	double result = 0.0;
	for(int i = 0; i < glcm->getSize(); i++) {
		for(int j = 0; j < glcm->getSize(); j++) {
			result += glcm->getGLCM()[i][j] * std::pow(i - j, 2);
		}
	}

	return result;
}

double GLCM_features::calcHomogeneity(std::unique_ptr<GLCM>& glcm) {
	double result = 0.0;
	for(int i = 0; i < glcm->getSize(); i++) {
		for(int j = 0; j < glcm->getSize(); j++) {
			result += glcm->getGLCM()[i][j] / (1 + std::pow(i - j, 2));
		}
	}

	return result;
}

std::tuple<int, int, int, int> GLCM_features::setStartingWindowParams() {
	int minRow = 0;
	int maxRow = this->_image->getImageInfo().height - this->_windowSize;
	int minCol = 0;
	int maxCol = this->_image->getImageInfo().width - this->_windowSize;

	std::tuple<int, int, int, int> windowStartingValues = std::make_tuple(minRow, maxRow, minCol, maxCol);

	return windowStartingValues;
}
