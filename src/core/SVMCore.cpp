#include "SVMCore.h"

#include <cmath>

#include "core/utils/DigitNormalizer.h"

namespace core {
    using SVM = cv::ml::SVM;

    SVMCore::SVMCore(models::Dataset &trainDataset, models::Dataset &testDataset, const std::string &savePath) :
        modelPath_(savePath) {
        trainDatasetOwned_ = std::make_unique<models::Dataset>(utils::DigitNormalizer::normalizeDataset(trainDataset));
        testDatasetOwned_ = std::make_unique<models::Dataset>(utils::DigitNormalizer::normalizeDataset(testDataset));
        trainDataset_ = trainDatasetOwned_.get();
        testDataset_ = testDatasetOwned_.get();

        svm_ = SVM::create();
        svm_->setKernel(SVM::RBF);
        svm_->setType(SVM::C_SVC);
        svm_->setC(10);
        svm_->setGamma(0.05);
    }

    SVMCore::SVMCore(const std::string &modelPath) : modelPath_(modelPath) { loadModel(modelPath); }

    void SVMCore::train() {
        if (trainDataset_ == nullptr || trainDataset_->labels.empty())
            return;

        svm_->train(trainDataset_->data, cv::ml::ROW_SAMPLE, trainDataset_->labels);
        svm_->save(modelPath_);
    }

    double SVMCore::test() {
        if (!svm_ || !svm_->isTrained() || testDataset_ == nullptr || testDataset_->data.empty() ||
            testDataset_->labels.empty() || testDataset_->data.rows != static_cast<int>(testDataset_->labels.size()))
            return 0.0;

        cv::Mat results;
        svm_->predict(testDataset_->data, results);

        int correct = 0;
        for (int i = 0; i < results.rows; ++i) {
            if (static_cast<int>(std::lround(results.at<float>(i, 0))) == testDataset_->labels[i])
                ++correct;
        }

        return static_cast<double>(correct) / testDataset_->data.rows * 100.0;
    }

    bool SVMCore::loadModel(const std::string &modelPath) {
        svm_ = SVM::load(modelPath);
        return svm_.get() != nullptr;
    }

    bool SVMCore::saveModel(const std::string &modelPath) {
        if (!svm_ || !svm_->isTrained())
            return false;
        svm_->save(modelPath);
        return true;
    }

    int SVMCore::predictNum(const cv::Mat &sample) {
        if (!svm_ || !svm_->isTrained())
            return -1;

        cv::Mat image = sample;
        if (sample.total() == 28 * 28 && (sample.rows == 1 || sample.cols == 1))
            image = sample.reshape(1, 28);
        cv::Mat normalized = utils::DigitNormalizer::normalize(image, cv::Size(28, 28));
        if (normalized.empty())
            return -1;

        float r = svm_->predict(normalized.reshape(1, 1));
        return static_cast<int>(std::lround(r));
    }

    std::vector<int> SVMCore::predictNums(const cv::Mat &samples) {
        std::vector<int> predictions;
        predictions.reserve(samples.rows);

        for (int i = 0; i < samples.rows; ++i) {
            predictions.push_back(predictNum(samples.row(i)));
        }
        return predictions;
    }
} // namespace core
