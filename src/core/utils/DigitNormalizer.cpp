#include "DigitNormalizer.h"

#include <algorithm>
#include <cmath>
#include <opencv2/core.hpp>
#include <opencv2/geometry/2d.hpp>
#include <opencv2/imgproc.hpp>
#include <utility>
#include <vector>

namespace core::utils {
    namespace {
        cv::Mat fitCanvasDigit(const cv::Mat &src, const cv::Size &size) {
            cv::Mat foreground;
            cv::threshold(src, foreground, 16, 255, cv::THRESH_BINARY);

            std::vector<cv::Point> points;
            cv::findNonZero(foreground, points);
            if (points.empty())
                return {};

            const cv::Rect bounds = cv::boundingRect(points);
            const cv::Mat cropped = src(bounds);
            const double scale = 20.0 / std::max(bounds.width, bounds.height);
            const cv::Size fittedSize(std::max(1, cvRound(bounds.width * scale)),
                                      std::max(1, cvRound(bounds.height * scale)));

            cv::Mat resized;
            cv::resize(cropped, resized, fittedSize, 0.0, 0.0, scale < 1.0 ? cv::INTER_AREA : cv::INTER_CUBIC);

            const cv::Moments moments = cv::moments(resized, false);
            if (moments.m00 <= 0.0)
                return {};

            const int x = cvRound(size.width / 2.0 - moments.m10 / moments.m00);
            const int y = cvRound(size.height / 2.0 - moments.m01 / moments.m00);
            cv::Mat fitted(size, CV_8UC1, cv::Scalar(0));
            const cv::Rect destination =
                    cv::Rect(x, y, resized.cols, resized.rows) & cv::Rect(0, 0, size.width, size.height);
            const cv::Rect source(destination.x - x, destination.y - y, destination.width, destination.height);
            resized(source).copyTo(fitted(destination));
            return fitted;
        }

        cv::Mat normalizePreprocess(const cv::Mat &src, const cv::Size &size) {
            cv::Mat resized;
            cv::resize(src, resized, size);

            cv::Mat binary;
            cv::threshold(resized, binary, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
            if (cv::countNonZero(binary) > (binary.rows * binary.cols) / 2)
                cv::bitwise_not(binary, binary);
            return binary;
        }

        std::pair<double, bool> deskewPCA(const cv::Mat &src) {
            std::vector<cv::Point> points;
            cv::findNonZero(src, points);
            if (points.size() < 2)
                return {0.0, false};

            cv::Mat data(static_cast<int>(points.size()), 2, CV_32F);
            for (std::size_t i = 0; i < points.size(); ++i) {
                data.at<float>(static_cast<int>(i), 0) = static_cast<float>(points[i].x);
                data.at<float>(static_cast<int>(i), 1) = static_cast<float>(points[i].y);
            }

            cv::Mat mean;
            cv::Mat eigenvalues;
            cv::Mat eigenvectors;
            cv::PCACompute(data, mean, eigenvectors, eigenvalues);

            double vx = eigenvectors.at<float>(0, 0);
            double vy = eigenvectors.at<float>(0, 1);
            if (vy < 0) {
                vx = -vx;
                vy = -vy;
            }
            return {-std::atan2(vy, vx) * 180.0 / CV_PI, true};
        }
    } // namespace

    cv::Mat DigitNormalizer::normalize(const cv::Mat &src, const cv::Size &size) {
        if (src.empty() || src.rows == 0 || src.cols == 0 || src.channels() != 1)
            return {};

        cv::Mat fitted;
        const cv::Mat *input = &src;
        if (src.cols > size.width || src.rows > size.height) {
            fitted = fitCanvasDigit(src, size);
            if (fitted.empty())
                return {};
            input = &fitted;
        }

        cv::Mat resized;
        cv::resize(*input, resized, size);
        const cv::Mat preprocessed = normalizePreprocess(resized, size);
        const auto [angle, ok] = deskewPCA(preprocessed);

        cv::Mat deskewed;
        if (ok) {
            const cv::Moments moments = cv::moments(preprocessed, true);
            const cv::Point2f center(static_cast<float>(moments.m10 / moments.m00),
                                     static_cast<float>(moments.m01 / moments.m00));
            const cv::Mat rotation = cv::getRotationMatrix2D(center, angle, 1.0);
            cv::warpAffine(resized, deskewed, rotation, size, cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(0));
        } else {
            deskewed = resized;
        }

        cv::Mat normalized;
        deskewed.convertTo(normalized, CV_32FC1, 1.0 / 255.0);
        return normalized;
    }
    models::Dataset DigitNormalizer::normalizeDataset(const models::Dataset &dataset, const cv::Size &size) {
        models::Dataset deskewedDataset;
        for (int i = 0; i < dataset.data.rows; ++i) {
            const cv::Mat normalized = DigitNormalizer::normalize(dataset.data.row(i).reshape(1, 28), size);
            deskewedDataset.data.push_back(normalized.reshape(1, 1));
        }
        deskewedDataset.labels = dataset.labels;
        return deskewedDataset;
    }
} // namespace core::utils
