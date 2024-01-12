#include "../headers/image.h"

/**
Load image from disk in grayscale and reduce gray levels to given number.
@param path - path to stored image.
@param grayLevelsAmount - target amount of gray levels in read image.
*/
Image::Image(std::string path, int grayLevelsAmount) {
	this->_path = path;
	this->_imageInfo.imageName = std::filesystem::path(this->_path).filename().replace_extension("").string();
	this->_imageInfo.extension = std::filesystem::path(this->_path).extension().string();

	try {
		this->loadImageInGrayScale();
	}
	catch (ImageNotFoundException ex) {
		std::cerr << ex.msg();
		throw ex;
	}

	this->_imageInfo.width = this->_img.cols;
	this->_imageInfo.height = this->_img.rows;
	if (this->isGrayLevelsAmountCorrect(grayLevelsAmount)) {
		this->_imageInfo.grayLevelsAmount = grayLevelsAmount;
	}
	else {
		std::cerr << "Wrong gray levels amount. It has to be a integer number within (0, 255> range.\n";
		this->_imageInfo.grayLevelsAmount = DEFAULT_GRAY_LEVELS_AMOUNT;
	}

	this->calculateOriginalGrayLevelsAmount();

	try {
		this->reduceGrayLevels();
	}
	catch (BadGrayLevels ex) {
		std::cerr << ex.msg();
		throw ex;
	}
}

bool Image::isGrayLevelsAmountCorrect(int grayLevelsAmount) {
	if (grayLevelsAmount > 0 && grayLevelsAmount < 256) {
		return true;
	}
	return false;
}

void Image::loadImageInGrayScale() {
	this->_img = cv::imread(this->_path, cv::IMREAD_GRAYSCALE);
	if (this->_img.empty()) {
		std::cerr << "Error: Unable to load the image." << std::endl;
		throw new ImageNotFoundException(this->_path);
	}
}

void Image::calculateOriginalGrayLevelsAmount() {
	std::set<uchar>uniqueLevels(this->_img.begin<uchar>(), this->_img.end<uchar>());
	this->_imageInfo.originalGrayLevelsAmount = static_cast<int>(uniqueLevels.size());
}

void Image::reduceGrayLevels() {
	if (!this->isGrayLevelCorrect()) {
		throw new BadGrayLevels();
	}

	int scale = std::ceil(static_cast<double>(MAX_PIXEL_VALUE) / this->_imageInfo.grayLevelsAmount);
	for(int i = 0; i < MAX_PIXEL_VALUE; i += scale) {
		this->_imageInfo.grayLevels.push_back(i);
	}

	for (int i = 0; i < this->_imageInfo.height; ++i) {
		for (int j = 0; j < this->_imageInfo.width; ++j) {
			int originalPixelValue = this->_img.at<uchar>(i, j);
			int newPixelValue = static_cast<int>((originalPixelValue / scale) * scale);
			this->_img.at<uchar>(i, j) = static_cast<uchar>(newPixelValue);
		}
	}	
}

bool Image::isGrayLevelCorrect() {
	if ((this->_imageInfo.grayLevelsAmount < 1 && this->_imageInfo.grayLevelsAmount > 256) || 
		(this->_imageInfo.grayLevelsAmount > this->_imageInfo.originalGrayLevelsAmount)) {
		return false;
	}

	return true;
}

/**
Create image with size and grey levels amount equal to given image.
@param image - existing image.
*/
Image::Image(std::shared_ptr<Image> image) {
	this->_imageInfo.imageName = image->getImageInfo().imageName;
	this->_imageInfo.extension = image->getImageInfo().extension;
	this->_imageInfo.width = image->getImageInfo().width;
	this->_imageInfo.height = image->getImageInfo().height;
	this->_imageInfo.grayLevelsAmount = image->getImageInfo().grayLevelsAmount;
	this->_imageInfo.grayLevels = image->getImageInfo().grayLevels;

	std::string directoryPath = std::filesystem::path(image->getPath()).parent_path().string();
	std::string delimiter = "/";
	directoryPath = directoryPath.erase(directoryPath.rfind(delimiter)) + delimiter + "output/";
	this->_path = directoryPath;

	this->_img = cv::Mat(this->_imageInfo.height, this->_imageInfo.width, CV_8UC1, cv::Scalar(0, 0, 0));
}

bool Image::isImageSizesCorrect(int width, int height) {
	if (width > 0 && height < 0) {
		return true;
	}
	return false;
}

/**
Create image with size and grey levels amount equal to given image.
@param image - existing image.
*/
void Image::setPixelValue(int x, int y, int value) {
	if (this->isPixelCoordsCorrect(x, y)) {
		this->_img.at<uchar>(x, y) = static_cast<uchar>(value);
	}
	else {
		std::cerr << "Wrong pixel coords to change. Can't do anything.\n";
	}
}

/**
Create image with size and grey levels amount equal to given image.
@param image - existing image.
*/
void Image::setPixelValue(int x, int y, double value) {
	int valueToGrayScale = this->convertDoubleValueToGrayScale(value);
	this->setPixelValue(x, y, valueToGrayScale);
}

bool Image::isPixelCoordsCorrect(int x, int y) {
	if ((x >= 0 && x <= this->_imageInfo.height) && (y >= 0 && y <= this->_imageInfo.width)) {
		return true;
	}
	return false;
}

/**
Save image on disk. Remember to previously set proper image name and path.
*/
void Image::saveImage() {
	bool check = cv::imwrite(this->_path, this->_img);
	if(check) {
		std::cout << "Successfully saved " + this->_imageInfo.imageName + this->_imageInfo.extension << std::endl;
	}
	else {
		std::cerr << "Saving " + this->_imageInfo.imageName + this->_imageInfo.extension + " FAILED!" << std::endl;
	}

}

int Image::convertDoubleValueToGrayScale(double value) {
	int grayScaleValue = value * 255;
	grayScaleValue = this->convertIntValueToGrayLevel(grayScaleValue);

	return grayScaleValue;
}

int Image::convertIntValueToGrayLevel(int value) {
	int nearestGrayScaleValueFromLevels = INT_MAX;
	int smallestDifference = INT_MAX;
	for(auto greyLevel : this->_imageInfo.grayLevels) {
		int difference = std::abs(value - static_cast<int>(greyLevel));
		if(difference < smallestDifference) {
			smallestDifference = difference;
			nearestGrayScaleValueFromLevels = static_cast<int>(greyLevel);
		}
	}

	return nearestGrayScaleValueFromLevels;
}

/**
Display image in dedicate window.
*/
void Image::displayImage() {
	cv::imshow(this->_imageInfo.imageName, this->_img);
	cv::waitKey(0);
	cv::destroyAllWindows();
}

/**
Print pixel values of the image on the console.
*/
void Image::printImage() {
	std::cout << "Image values" << std::endl;
	for (int i = 0; i < this->_imageInfo.height; i++) {
		for (int j = 0; j < this->_imageInfo.width; j++) {
			uchar pixelValue = this->_img.at<uchar>(i, j);
			std::cout << std::setw(3) << static_cast<int>(pixelValue) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

/**
Get struct with basic image data
@return Image data struct.
*/
ImageInfo Image::getImageInfo() {
	return this->_imageInfo;
}

/**
Get image matrix.
@return Image matrix.
*/
cv::Mat Image::getImage() {
	return this->_img;
}

/**
Get image path.
@return Path to the image.
*/
std::string Image::getPath() {
	return this->_path;
}

/**
Set new name of the image. Also update image path.
@param name - new image name.
*/
void Image::setImageName(std::string name) {
	this->_imageInfo.imageName = std::filesystem::path(name).replace_extension("").string();
	this->_path = std::filesystem::path(this->_path).replace_filename(name).string() + this->_imageInfo.extension;
}
