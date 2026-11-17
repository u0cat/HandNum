#include "KNNCore.h"

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

namespace core {
    OpenCVKNNCore::OpenCVKNNCore(models::Dataset &trainDataset, models::Dataset &testDataset) :
        BaseCore(trainDataset, testDataset), knn_(cv::ml::KNearest::create()) {
        trainDataset.data.convertTo(trainDataset.data, CV_32F);
        testDataset.data.convertTo(testDataset.data, CV_32F);
        knn_->setDefaultK(3);
        knn_->setIsClassifier(true);
    }

    OpenCVKNNCore::OpenCVKNNCore(models::Dataset &trainDataset) :
        BaseCore(trainDataset), knn_(cv::ml::KNearest::create()) {
        trainDataset.data.convertTo(trainDataset.data, CV_32F);
        knn_->setDefaultK(3);
        knn_->setIsClassifier(true);
    }

    void OpenCVKNNCore::train() { knn_->train(trainDataset_->data, cv::ml::ROW_SAMPLE, trainDataset_->labels); }

    double OpenCVKNNCore::test() {
        if (testDataset_ == nullptr)
            return 0;

        cv::Mat results;
        knn_->findNearest(testDataset_->data, knn_->getDefaultK(), results);

        int correct = 0;
        for (int i = 0; i < results.rows; ++i) {
            if (results.at<float>(i, 0) == testDataset_->labels[i]) {
                ++correct;
            }
        }
        return static_cast<double>(correct) / testDataset_->data.rows * 100.0;
    }

    int OpenCVKNNCore::predictNum(const cv::Mat &sample) {
        cv::Mat result;
        cv::Mat floatSample;
        sample.convertTo(floatSample, CV_32F);
        knn_->findNearest(floatSample, knn_->getDefaultK(), result);
        return static_cast<int>(result.at<float>(0, 0));
    }

    std::vector<int> OpenCVKNNCore::predictNums(const cv::Mat &samples) {
        std::vector<int> predictions;
        for (int i = 0; i < samples.rows; ++i) {
            predictions.push_back(predictNum(samples.row(i)));
        }
        return predictions;
    }
} // namespace core

namespace core {
    CustomKNNCore::CustomKNNCore(models::Dataset &trainDataset, models::Dataset &testDataset, int k) :
        BaseCore(trainDataset, testDataset), k_(k) {
        trainData_ = trainDataset.data;
        trainData_.convertTo(trainData_, CV_32F);
        trainLabels_ = cv::Mat(trainDataset.labels).reshape(1, static_cast<int>(trainDataset.labels.size()));
    }

    CustomKNNCore::CustomKNNCore(models::Dataset &trainDataset, int k) : BaseCore(trainDataset), k_(k) {
        trainData_ = trainDataset.data;
        trainData_.convertTo(trainData_, CV_32F);
        trainLabels_ = cv::Mat(trainDataset.labels).reshape(1, static_cast<int>(trainDataset.labels.size()));
    }

    void CustomKNNCore::train() {}

    double CustomKNNCore::test() {
        if (testDataset_ == nullptr)
            return 0;
        int correct = 0;
        for (int i = 0; i < testDataset_->data.rows; ++i) {
            int prediction = predictNum(testDataset_->data.row(i));
            if (prediction == testDataset_->labels[i]) {
                ++correct;
            }
        }

        return static_cast<double>(correct) / testDataset_->data.rows * 100.0;
    }

    // 计算距离 -> 排序前k个 -> 计算票数
    int CustomKNNCore::predictNum(const cv::Mat &sample) {
        std::vector<std::pair<double, int>> distances(trainData_.rows);

        cv::parallel_for_(cv::Range(0, trainData_.rows), [&](const cv::Range &range) {
            for (int i = range.start; i < range.end; ++i) {
                distances[i] = {cv::norm(sample, trainData_.row(i), cv::NORM_L2), trainLabels_.at<int>(i, 0)};
            }
        });

        // 排序
        std::partial_sort(
                distances.begin(), distances.begin() + k_, distances.end(),
                [](const std::pair<double, int> &a, const std::pair<double, int> &b) { return a.first < b.first; });

        // 统计
        std::map<int, int> votes;
        for (int i = 0; i < k_; ++i) {
            ++votes[distances[i].second];
        }

        int bestLabel = distances[0].second;
        int bestCount = 0;

        for (int i = 0; i < k_; ++i) {
            const int label = distances[i].second;
            const int count = votes[label];

            if (count > bestCount) {
                bestLabel = label;
                bestCount = count;
            }
        }

        return bestLabel;
    }

    std::vector<int> CustomKNNCore::predictNums(const cv::Mat &samples) {
        std::vector<int> results;
        for (int i = 0; i < samples.rows; ++i) {
            results.push_back(predictNum(samples.row(i)));
        }
        return results;
    }
} // namespace core
