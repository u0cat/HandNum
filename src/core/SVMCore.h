#ifndef CORE_SVMCORE_H
#define CORE_SVMCORE_H

#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

#include <memory>
#include <string>

#include "models/baseCore.h"

namespace core {
    class SVMCore : public models::BaseCore, public models::WeightLoadable {
    public:
        SVMCore(models::Dataset &trainDataset, models::Dataset &testDataset, const std::string &savePath);
        SVMCore(const std::string &modelPath);

        void train() override;
        double test() override;

        bool loadModel(const std::string &modelPath) override;
        bool saveModel(const std::string &modelPath) override;

        int predictNum(const cv::Mat &sample) override;
        std::vector<int> predictNums(const cv::Mat &samples) override;

    private:
        cv::Ptr<cv::ml::SVM> svm_;
        std::string modelPath_;
        // 归一化后的副本，避免改写调用方的 Dataset（baseCore.h 的指针指向这里）
        std::unique_ptr<models::Dataset> trainDatasetOwned_;
        std::unique_ptr<models::Dataset> testDatasetOwned_;
    };
} // namespace core

#endif // CORE_SVMCORE_H
