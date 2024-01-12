#include "../../glcm/headers/glcm_features.h"
#include "../../glcm/headers/glcm.h"
#include "../../glcm/headers/image.h"

#include <filesystem>

int main() {
	std::filesystem::path cwd = std::filesystem::current_path();

	std::vector<int> grayLevels;
	grayLevels.push_back(8);
	grayLevels.push_back(16);
	grayLevels.push_back(24);
	grayLevels.push_back(32);

	std::vector<std::string> paths;
	paths.push_back("../../../../../../images/source/image01.jpg");
	//paths.push_back("../../../../../../images/source/image02.jpg");

	for(auto path : paths) {
		for(auto grayLevel : grayLevels) {
			std::shared_ptr<Image> image = std::make_shared<Image>(path, grayLevel);

			std::vector<std::pair<int, int>> offsets;
			offsets.push_back(std::pair<int, int>(1, 0));
			offsets.push_back(std::pair<int, int>(0, 1));
			offsets.push_back(std::pair<int, int>(1, 1));
			offsets.push_back(std::pair<int, int>(-1, 1));

			std::vector<int> windowSizes;
			windowSizes.push_back(5);
			windowSizes.push_back(7);
			windowSizes.push_back(9);

			for(auto size : windowSizes) {
				std::unique_ptr<GLCM_features> glcmFeatures = std::make_unique<GLCM_features>(image, size);
				glcmFeatures->energy(offsets);
				glcmFeatures->entropy(offsets);
				glcmFeatures->contrast(offsets);
				glcmFeatures->homogeneity(offsets);
			}
		}
	}

	std::cout << "Program has finished. Press 'Enter' to shutdown." << std::endl;
	std::cin.get();
	return 0;
}
