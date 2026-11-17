#ifndef CORE_MODELS_DATASET_H
#define CORE_MODELS_DATASET_H

#include <opencv2/core.hpp>

#include <vector>

namespace core::models {
    struct Dataset {
        cv::Mat data;
        std::vector<int> labels;
    };
} // namespace core::models

#endif // CORE_MODELS_DATASET_H
