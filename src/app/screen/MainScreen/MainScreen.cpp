#include "MainScreen.h"

#include <memory>

#include "imgui.h"
#include "screen/CameraScreen/CameraScreen.h"
#include "screen/PredictScreen/PredictScreen.h"
#include "screen/ScreenManager.h"

namespace app::screen {
    void MainScreen::render() {
        ImGui::SetNextWindowSize(ImVec2(360.0f, 160.0f), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("HandNum")) {
            ImGui::End();
            return;
        }

        if (mgr_) {
            const bool cameraEnabled = mgr_->isEnabled("camera");
            if (ImGui::Button(cameraEnabled ? "关闭摄像头" : "打开摄像头")) {
                if (mgr_->getScreen("camera")) {
                    if (cameraEnabled)
                        mgr_->disable("camera");
                    else
                        mgr_->enable("camera");
                } else {
                    mgr_->add("camera", std::make_unique<CameraScreen>(renderer_));
                }
            }

            const bool predictEnabled = mgr_->isEnabled("predict");
            if (ImGui::Button(predictEnabled ? "关闭画板" : "打开画板")) {
                if (mgr_->getScreen("predict")) {
                    if (predictEnabled)
                        mgr_->disable("predict");
                    else
                        mgr_->enable("predict");
                } else {
                    mgr_->add("predict", std::make_unique<PredictScreen>());
                }
            }
        }

        ImGui::End();
    }
} // namespace app::screen
