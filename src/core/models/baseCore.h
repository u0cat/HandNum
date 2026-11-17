#ifndef CORE_MODELS_BASECORE_H
#define CORE_MODELS_BASECORE_H

#include <string>
#include <vector>

#include "models/Dataset.h"

namespace core::models {
    class BaseCore {
    public:
        BaseCore() = default;
        BaseCore(models::Dataset &trainDataset, models::Dataset &testDataset) :
            trainDataset_(&trainDataset), testDataset_(&testDataset) {}
        explicit BaseCore(models::Dataset &trainDataset) : trainDataset_(&trainDataset) {}
        virtual ~BaseCore() = default;

        virtual void train() = 0;
        virtual double test() = 0;
        virtual int predictNum(const cv::Mat &sample) = 0;
        virtual std::vector<int> predictNums(const cv::Mat &samples) = 0;

    protected:
        models::Dataset *trainDataset_ = nullptr;
        models::Dataset *testDataset_ = nullptr;
    };

    class WeightLoadable {
    public:
        virtual ~WeightLoadable() = default;

        virtual bool loadModel(const std::string &modelPath) = 0;
        virtual bool saveModel(const std::string &modelPath) = 0;
    };
} // namespace core::models

#endif // CORE_MODELS_BASECORE_H
