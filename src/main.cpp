#include <iostream>

#include "core/SVMCore.h"
#include "core/utils/DatasetLoader.h"

int main() {
    auto trainDataset = core::utils::loadMnistDataset("data/raw/mnist/train-images-idx3-ubyte",
                                                      "data/raw/mnist/train-labels-idx1-ubyte");
    auto testDataset = core::utils::loadMnistDataset("data/raw/mnist/t10k-images-idx3-ubyte",
                                                     "data/raw/mnist/t10k-labels-idx1-ubyte");

    auto trainSubset = core::utils::splitDataset(trainDataset, 40000);
    auto testSubset = core::utils::splitDataset(testDataset, 1000);
    // core::CustomKNNCore model(trainSubset, testSubset, 3);
    // model.train();

    // core::OpenCVKNNCore model(trainSubset, testSubset);
    // model.train();

    core::SVMCore model(trainSubset, testSubset, SVM_MODEL_PATH);
    model.train();
    std::cout << "Accuracy: " << model.test();
    // core::SVMCore model(HANDNUM_MODEL_PATH);

    // std::vector<int> labels;
    // labels.reserve(5000);
    // for (int i = 0; i < 10; ++i)
    //     labels.insert(labels.end(), 500, i);
    // auto imgDataset = core::utils::loadImgDataset("data/raw/digits.png", 20, 20, labels);
    // // imgDataset.data.convertTo(imgDataset.data, CV_32F);
    // auto results = model.predictNums(imgDataset.data);
    // int correct = 0;
    // for (int i = 0; i < results.size(); ++i) {f
    //     if(results[i] == imgDataset.labels[i]) ++correct;
    // }
    // std::cout << "Accuracy: " << static_cast<double>(correct) / imgDataset.data.rows * 100.0;

    return 0;
}
