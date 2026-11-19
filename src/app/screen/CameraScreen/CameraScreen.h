#ifndef APP_SCREEN_CAMERASCREEN_CAMERASCREEN_H
#define APP_SCREEN_CAMERASCREEN_CAMERASCREEN_H

#include <opencv2/videoio.hpp>

#include <string>

#include "components/preview/Preview.h"
#include "screen/Screen.h"

namespace app::screen {
    class CameraScreen : public Screen {
    public:
        explicit CameraScreen(SDL_Renderer *renderer);
        ~CameraScreen() override;

    private:
        components::Preview preview_;
        cv::VideoCapture capture_;
        std::string errorMessage_;
        int cameraIndex_ = 0;

        void openCamera();
        void closeCamera();
        void updateCapturedFrame();
        void render() override;
    };
} // namespace app::screen

#endif // APP_SCREEN_CAMERASCREEN_CAMERASCREEN_H
