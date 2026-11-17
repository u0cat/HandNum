#include "DatasetLoader.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <map>
#include <random>
#include <stdexcept>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

namespace core::utils {
    namespace {
        // 端序转换
        // MNIST 的 IDX 文件头部的整数采用大端序存储
        // 而PC通常使用小端序
        uint32_t swap_endian(uint32_t val) {
            val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
            return (val << 16) | (val >> 16);
        }
    } // namespace

    models::Dataset loadMnistDataset(const std::string &imagesPath, const std::string &labelsPath) {
        models::Dataset dataset{};

        // Images
        std::ifstream imageFile(imagesPath, std::ios::binary | std::ios::in);
        if (!imageFile.is_open()) {
            throw std::runtime_error("Failed to open image file: " + imagesPath);
        }

        int32_t magicNumber, numImages, numRows, numCols;

        imageFile.read(reinterpret_cast<char *>(&magicNumber), 4);
        magicNumber = swap_endian(magicNumber);
        if (magicNumber != 2051) {
            throw std::runtime_error("Invalid magic number in image file: " + imagesPath);
        }

        imageFile.read(reinterpret_cast<char *>(&numImages), 4);
        numImages = swap_endian(numImages);

        imageFile.read(reinterpret_cast<char *>(&numRows), 4);
        numRows = swap_endian(numRows);

        imageFile.read(reinterpret_cast<char *>(&numCols), 4);
        numCols = swap_endian(numCols);

        cv::Mat data(numImages, numRows * numCols, CV_8UC1);
        for (int i = 0; i < numImages; ++i) {
            imageFile.read(reinterpret_cast<char *>(data.ptr(i)), numRows * numCols);
        }
        imageFile.close();
        dataset.data = data;

        // labels
        std::ifstream labelFile(labelsPath, std::ios::binary | std::ios::in);
        if (!labelFile.is_open()) {
            throw std::runtime_error("Failed to open label file: " + labelsPath);
        }

        int32_t labelMagicNumber, numLabels;

        labelFile.read(reinterpret_cast<char *>(&labelMagicNumber), 4);
        labelMagicNumber = swap_endian(labelMagicNumber);
        if (labelMagicNumber != 2049) {
            throw std::runtime_error("Invalid magic number in label file: " + labelsPath);
        }

        labelFile.read(reinterpret_cast<char *>(&numLabels), 4);
        numLabels = swap_endian(numLabels);

        std::vector<int> labels(numLabels);
        for (int i = 0; i < numLabels; ++i) {
            labelFile.read(reinterpret_cast<char *>(&labels[i]), 1);
        }
        labelFile.close();
        dataset.labels = labels;

        return dataset;
    }

    // 读图, 缩放成 28x28, 并且与 mnist 的存储方式统一
    models::Dataset loadImgDataset(const std::string &imagesPath, const int width, const int height,
                                   const std::vector<int> &labels) {
        models::Dataset dataset{};

        cv::Mat rawImg = cv::imread(imagesPath, cv::IMREAD_GRAYSCALE);
        for (int y = 0; y + height <= rawImg.rows; y += height) {
            for (int x = 0; x + width <= rawImg.cols; x += width) {
                cv::Mat cropped = rawImg(cv::Rect(x, y, width, height));
                cv::Mat resized;
                cv::resize(cropped, resized, cv::Size(28, 28)); // resize to 28x28
                dataset.data.push_back(resized.reshape(1, 1));  // 展平
            }
        }


        dataset.labels = labels;
        return dataset;
    }

    // 调整数据集大小, 分层抽样选择 num 个样本
    models::Dataset splitDataset(const models::Dataset &dataset, int size) {
        // 分层抽样
        std::map<int, std::vector<int>> labelIndices;
        for (int i = 0; i < static_cast<int>(dataset.labels.size()); ++i) {
            labelIndices[dataset.labels[i]].push_back(i);
        }

        // 打乱
        std::mt19937 rng(std::random_device{}());
        for (auto &[label, indices]: labelIndices) {
            std::shuffle(indices.begin(), indices.end(), rng);
        }

        models::Dataset splitDataset{};
        int samplesPerClass = size / labelIndices.size();
        for (const auto &[label, indices]: labelIndices) {
            for (int i = 0; i < samplesPerClass && i < static_cast<int>(indices.size()); ++i) {
                int idx = indices[i];
                splitDataset.data.push_back(dataset.data.row(idx));
                splitDataset.labels.push_back(label);
            }
        }

        return splitDataset;
    }
} // namespace core::utils
