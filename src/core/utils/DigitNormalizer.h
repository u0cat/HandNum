#ifndef CORE_UTILS_DIGITNORMALIZER_H
#define CORE_UTILS_DIGITNORMALIZER_H

#include <opencv2/core.hpp>

#include "models/Dataset.h"

namespace core::utils {
    class DigitNormalizer {
    public:
        static cv::Mat normalize(const cv::Mat &src, const cv::Size &size = cv::Size(28, 28));
        static models::Dataset normalizeDataset(const models::Dataset &dataset,
                                                const cv::Size &size = cv::Size(28, 28));
    };
} // namespace core::utils

#endif // CORE_UTILS_DIGITNORMALIZER_H
