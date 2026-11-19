#include "CameraScreen.h"

#include <opencv2/core.hpp>

#include "imgui.h"

namespace app::screen {
    CameraScreen::CameraScreen(SDL_Renderer *renderer) : preview_(renderer) {}

    CameraScreen::~CameraScreen() { closeCamera(); }

    void CameraScreen::openCamera() {
        closeCamera();
        if (!capture_.open(cameraIndex_)) {
            errorMessage_ = "无法打开摄像头";
            return;
        }
        errorMessage_.clear();
    }

    void CameraScreen::closeCamera() {
        capture_.release();
        preview_.clear();
        errorMessage_.clear();
    }

    void CameraScreen::updateCapturedFrame() {
        if (!capture_.isOpened())
            return;

        cv::Mat frame;
        if (!capture_.read(frame) || frame.empty()) {
            closeCamera();
            errorMessage_ = "无法读取摄像头画面";
            return;
        }
        if (!preview_.setFrame(frame))
            errorMessage_ = "无法显示摄像头画面";
    }

    void CameraScreen::render() {
        ImGui::SetNextWindowSize(ImVec2(1080.0f, 680.0f), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Camera")) {
            ImGui::End();
            return;
        }

        if (!capture_.isOpened()) {
            if (ImGui::Button("打开摄像头"))
                openCamera();
        } else if (ImGui::Button("关闭摄像头")) {
            closeCamera();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90.0f);
        ImGui::InputInt("摄像头ID", &cameraIndex_);

        updateCapturedFrame();

        ImGui::BeginChild("PreviewPanel", ImVec2(0.0f, -ImGui::GetTextLineHeightWithSpacing()), true);
        preview_.draw();
        ImGui::EndChild();

        if (!errorMessage_.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 140, 140, 255));
            ImGui::TextWrapped("%s", errorMessage_.c_str());
            ImGui::PopStyleColor();
        }
        ImGui::End();
    }
} // namespace app::screen
