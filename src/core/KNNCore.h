#ifndef CORE_KNNCORE_H
#define CORE_KNNCORE_H

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

#include "models/baseCore.h"

namespace core {
    class OpenCVKNNCore : public models::BaseCore {
    public:
        OpenCVKNNCore(models::Dataset &trainDataset, models::Dataset &testDataset);
        OpenCVKNNCore(models::Dataset &trainDataset);

        void train() override;
        double test() override;
        int predictNum(const cv::Mat &sample) override;
        std::vector<int> predictNums(const cv::Mat &samples) override;

        void setK(int k) { knn_->setDefaultK(k); }
        int getK() const { return knn_->getDefaultK(); }

    private:
        cv::Ptr<cv::ml::KNearest> knn_;
    };

    class CustomKNNCore : public models::BaseCore {
    public:
        CustomKNNCore(models::Dataset &trainDataset, models::Dataset &testDataset, int k);
        CustomKNNCore(models::Dataset &trainDataset, int k);

        void train() override;
        double test() override;
        int predictNum(const cv::Mat &sample) override;
        std::vector<int> predictNums(const cv::Mat &samples) override;

        void setK(int k) { k_ = k; }
        int getK() const { return k_; }

    private:
        int k_;
        cv::Mat trainData_;
        cv::Mat trainLabels_;
    };
} // namespace core

#endif // CORE_KNNCORE_H
