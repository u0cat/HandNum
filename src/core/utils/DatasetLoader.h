#ifndef CORE_UTILS_DATASETLOADER_H
#define CORE_UTILS_DATASETLOADER_H
#include <string>
#include <vector>

#include <models/Dataset.h>

namespace core::utils {
    models::Dataset loadMnistDataset(const std::string &imagesPath, const std::string &labelsPath);
    models::Dataset loadImgDataset(const std::string &imagesPath, const int width, const int height,
                                   const std::vector<int> &labels);

    models::Dataset splitDataset(const models::Dataset &dataset, int size);
} // namespace core::utils


#endif // CORE_UTILS_DATASETLOADER_H
