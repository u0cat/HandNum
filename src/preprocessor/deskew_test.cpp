#include <filesystem>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "core/utils/DigitNormalizer.h"

int main() {
    std::filesystem::path imagePath;
    for (const auto &entry: std::filesystem::directory_iterator("data/processed/digits")) {
        if (entry.path().filename().string().rfind("0_", 0) == 0) {
            imagePath = entry.path();
            break;
        }
    }

    if (imagePath.empty()) {
        std::cerr << "Could not find any data/processed/digits/0_*.png\n";
        return 1;
    }

    cv::Mat image = cv::imread(imagePath.string(), cv::IMREAD_GRAYSCALE);
    if (image.empty()) {
        std::cerr << "Could not load " << imagePath << '\n';
        return 1;
    }

    cv::Mat result = core::utils::DigitNormalizer::normalize(image, image.size());
    cv::imshow("Original", image);
    cv::imshow("Deskewed", result);
    cv::waitKey(0);
    return 0;
}
