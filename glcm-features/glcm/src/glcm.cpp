#include "../headers/glcm.h"

GLCM::GLCM(std::shared_ptr<Image> image) {
	this->_image = image;
	this->_greyLevels = this->_image->getImageInfo().grayLevels;
	this->_size = this->_image->getImageInfo().grayLevelsAmount;

	this->_glcm = std::make_shared<std::shared_ptr<double[]>[]>(this->_size);
	for (int i = 0; i < this->_size; ++i) {
		this->_glcm[i] = std::make_shared<double[]>(this->_size);
	}

	this->clearGLCM();
}

void GLCM::clearGLCM() {
	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			this->_glcm[i][j] = 0.0;
		}
	}
}

/**
Calculate GLCM with given offset of whole image
@param offset - pair representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1) or (-1, 1).
@param horizontal - horizontal GLCM means transformation of calculated GLCM into symmetric matrix. For example
		when there is a match with offset is (1, 0), horizontal GLCM also count match with (-1, 0) offset.
*/
void GLCM::calculateGLCM(std::pair<int, int> offset, bool horizontal) {
	if (!this->checkOffset(offset)) {
		throw new BadOffset();
	}

	this->clearGLCM();

	cv::Mat matrix = this->_image->getImage();
	unsigned int width = this->_image->getImageInfo().width;
	unsigned int height = this->_image->getImageInfo().height;
	for (int i = 0; i < height; i++) {
		if (i + offset.second < 0 || i + offset.second >= height) {
			continue;
		}

		for (int j = 0; j < width; j++) {
			if (j + offset.first < 0 || j + offset.first >= width) {
				continue;
			}

			int currentPixelIndex = this->findGreyLevelValueInVector(matrix.at<uchar>(i, j));
			int neighbourPixelIndex = this->findGreyLevelValueInVector(matrix.at<uchar>(i + offset.second, j + offset.first));
			this->_glcm[currentPixelIndex][neighbourPixelIndex] += 1.0;
		}
	}

	if (horizontal) {
		this->makeGLCMHorizontal();
	}
	this->normalizeGLCM();
}

/*
Check if offset is legal. Allowed values are: (1, 0), (0, 1), (1, 1) or (-1, 1).
@param offset - pair representing offsets.
@return true if offset is valid, false when offset is illegal.
*/
bool GLCM::checkOffset(std::pair<int, int> offset) {
	if (
		(offset.first == 1 && offset.second == 0) ||
		(offset.first == 0 && offset.second == 1) ||
		(offset.first == 1 && offset.second == 1) ||
		(offset.first == -1 && offset.second == 1)
		) {
		return true;
	}
	else {
		return false;
	}
}

unsigned int GLCM::findGreyLevelValueInVector(unsigned int value) {
	unsigned int index;
	for (index = 0; index < this->_greyLevels.size(); index++) {
		if (value == this->_greyLevels[index]) {
			return index;
		}
	}

	throw new BadPixelValue();
}

void GLCM::makeGLCMHorizontal() {
	std::unique_ptr<std::unique_ptr<double[]>[]> transponedGLCM = std::move(this->createTransponedGLCM());

	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			this->_glcm[i][j] += transponedGLCM[i][j];
		}
	}
}

std::unique_ptr<std::unique_ptr<double[]>[]> GLCM::createTransponedGLCM() {
	std::unique_ptr<std::unique_ptr<double[]>[]> transponedGLCM = std::make_unique<std::unique_ptr<double[]>[]>(this->_size);
	for (int i = 0; i < this->_size; ++i) {
		transponedGLCM[i] = std::make_unique<double[]>(this->_size);
	}

	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			transponedGLCM[i][j] = this->_glcm[j][i];
		}
	}

	return transponedGLCM;
}

void GLCM::normalizeGLCM() {
	unsigned int sum = 0;
	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			sum += static_cast<unsigned int>(this->_glcm[i][j]);
		}
	}

	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			this->_glcm[i][j] /= (double)sum;
		}
	}
}

/**
Calculate GLCM with given vector of offsets in a square part of original image
@param offsets - vector of pairs representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param horizontal - horizontal GLCM means transformation of calculated GLCM into symmetric matrix. For example
		when there is a match with offset is (1, 0), horizontal GLCM also count match with (-1, 0) offset.
*/
void GLCM::calculateMeanGLCM(std::vector<std::pair<int, int>> offsets, bool horizontal) {
	if (offsets.empty()) {
		throw new NoOffsets();
	}

	for (auto offset : offsets) {
		if (!this->checkOffset(offset)) {
			throw new BadOffset();
		}
	}

	this->clearGLCM();
	for (auto offset : offsets) {
		std::unique_ptr<GLCM> glcm = std::make_unique<GLCM>(this->_image);
		glcm->calculateGLCM(offset, horizontal);

		std::shared_ptr<std::shared_ptr<double[]>[]> matrix = glcm->getGLCM();
		this->addIntermediateGLCM(matrix);
	}

	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			this->_glcm[i][j] /= static_cast<double>(offsets.size());
		}
	}
}

void GLCM::addIntermediateGLCM(std::shared_ptr<std::shared_ptr<double[]>[]> glcm) {
	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			this->_glcm[i][j] += glcm[i][j];
		}
	}
}

/**
Calculate GLCM with given offset in a square part of original image
@param offset - pair representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1) or (-1, 1).
@param top - row index of left top element of window
@param left - column index of left top element of window
@param windowSize - window size defining scope of image to calculate. Should be odd.
@param horizontal - horizontal GLCM means transformation of calculated GLCM into symmetric matrix. For example
		when there is a match with offset is (1, 0), horizontal GLCM also count match with (-1, 0) offset.
*/
void GLCM::calculateGLCM(std::pair<int, int> offset, int startingRow, int startingCol, int windowSize, bool horizontal) {
	if (!this->checkOffset(offset)) {
		throw new BadOffset();
	}

	this->clearGLCM();

	cv::Mat matrix = this->_image->getImage();
	unsigned int width = this->_image->getImageInfo().width;
	unsigned int height = this->_image->getImageInfo().height;
	for (int i = startingRow; i < startingRow + windowSize; i++) {
		// do not go outside matrix
		if (i + offset.second < 0 || i + offset.second >= startingRow + windowSize) {
			continue;
		}

		for (int j = startingCol; j < startingCol + windowSize; j++) {
			// do not go outside matrix
			if (j + offset.first < 0 || j + offset.first >= startingCol + windowSize) {
				continue;
			}

			int currentPixelIndex = this->findGreyLevelValueInVector(matrix.at<uchar>(i, j));
			int neighbourPixelIndex = this->findGreyLevelValueInVector(matrix.at<uchar>(i + offset.second, j + offset.first));
			this->_glcm[currentPixelIndex][neighbourPixelIndex] += 1.0;
		}
	}

	if (horizontal) {
		this->makeGLCMHorizontal();
	}
	this->normalizeGLCM();
}

/**
Calculate GLCM with given vector of offsets in a square part of original image
@param offsets - vector of pairs representing offsets. Allowed values are: (1, 0), (0, 1), (1, 1), (-1, 1).
@param top - row index of left top element of window
@param left - column index of left top element of window
@param windowSize - window size defining scope of image to calculate. Should be odd.
@param horizontal - horizontal GLCM means transformation of calculated GLCM into symmetric matrix. For example
		when there is a match with offset is (1, 0), horizontal GLCM also count match with (-1, 0) offset.
*/
void GLCM::calculateMeanGLCM(std::vector<std::pair<int, int>> offsets, int top, int left, int windowSize, bool horizontal) {
	if (offsets.empty()) {
		throw new NoOffsets();
	}

	for (auto offset : offsets) {
		if (!this->checkOffset(offset)) {
			throw new BadOffset();
		}
	}

	this->clearGLCM();
	for (auto offset : offsets) {
		std::unique_ptr<GLCM> glcm = std::make_unique<GLCM>(this->_image);
		glcm->calculateGLCM(offset, top, left, windowSize, horizontal);

		std::shared_ptr<std::shared_ptr<double[]>[]> matrix = glcm->getGLCM();
		this->addIntermediateGLCM(matrix);
	}

	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			this->_glcm[i][j] /= static_cast<double>(offsets.size());
		}
	}
}

/**
Print GLCM on console
@param coutPrecision - precision for printing floating-point values of GLCM
*/
void GLCM::printGLCM(unsigned int coutPrecision) {
	unsigned int maxCoutPrecision = 5;
	if (coutPrecision > maxCoutPrecision) {
		coutPrecision = maxCoutPrecision;
	}

	for (int i = 0; i < this->_size; i++) {
		for (int j = 0; j < this->_size; j++) {
			std::cout << std::fixed << std::setprecision(coutPrecision) << this->_glcm[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

std::shared_ptr<std::shared_ptr<double[]>[]> GLCM::getGLCM() {
	return this->_glcm;
}

int GLCM::getSize() {
	return this->_size;
}

std::shared_ptr<Image> GLCM::getImage() {
	return this->_image;
}
