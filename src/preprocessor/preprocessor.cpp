// Split and label digits.png
#include <filesystem>
#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

int main() {
    std::filesystem::create_directories("data/processed/digits");

    cv::Mat rawImg = cv::imread("data/raw/digits.png");

    if (rawImg.empty()) {
        std::cerr << "Error to load digits.png" << '\n';
        return -1;
    }

    constexpr int kNumWidth = 20;
    constexpr int kNumHeight = 20;
    constexpr int kRowsPerDigit = 5;
    const int numCols = rawImg.cols / kNumWidth;

    // 文件名为 <label>_<index>，index 全局单调递增，避免旧公式 x*y+y 在同行为 0 时互相覆盖
    for (int y = 0; y + kNumHeight <= rawImg.rows; y += kNumHeight) {
        for (int x = 0; x + kNumWidth <= rawImg.cols; x += kNumWidth) {
            cv::Mat cropped = rawImg(cv::Rect(x, y, kNumWidth, kNumHeight));
            const int label = y / (kNumHeight * kRowsPerDigit);
            const int index = (y / kNumHeight) * numCols + x / kNumWidth;
            const std::string path = cv::format("data/processed/digits/%d_%d.png", label, index);
            if (!cv::imwrite(path, cropped)) {
                std::cerr << "Failed to write " << path << '\n';
            }
        }
    }
}
