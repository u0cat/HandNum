#ifndef APP_SCREEN_PREDICTSCREEN_PREDICTSCREEN_H
#define APP_SCREEN_PREDICTSCREEN_PREDICTSCREEN_H

#include <memory>
#include <string>

#include "components/canvas/canvas.h"
#include "core/models/baseCore.h"
#include "screen/Screen.h"

namespace app::screen {
    class PredictScreen : public Screen {
    public:
        PredictScreen() = default;
        ~PredictScreen() = default;

    private:
        components::Canvas canvas_;
        // std::unique_ptr<core::SVMCore> model_;

        std::unique_ptr<core::models::BaseCore> core_;
        int currentCore_ = -1;
        std::unique_ptr<core::models::Dataset> trainDataset_;

        int prediction_ = -1;
        std::string errorMessage_;

        const char *kCoreTypes[3] = {"OpenCV-KNN", "Custom-KNN", "OpenCV-SVM"};
        int selectedCore_ = 2;

        void initCore();
        void predict();
        void render() override;
    };
} // namespace app::screen

#endif // APP_SCREEN_PREDICTSCREEN_PREDICTSCREEN_H
