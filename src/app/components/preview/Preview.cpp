#include "Preview.h"

#include <algorithm>

#include <opencv2/imgproc.hpp>

#include "imgui.h"

namespace app::components {
    namespace {
        constexpr int kMaxTextureWidth = 1280;
        constexpr int kMaxTextureHeight = 720;
    } // namespace

    Preview::Preview(SDL_Renderer *renderer) : renderer_(renderer) {}

    Preview::~Preview() { clear(); }

    bool Preview::setFrame(const cv::Mat &frame) {
        if (frame.empty() || (frame.channels() != 1 && frame.channels() != 3 && frame.channels() != 4))
            return false;

        frame_ = frame.clone();
        frameReady_ = true;
        return true;
    }

    void Preview::clear() {
        frame_.release();
        frameReady_ = false;
        if (texture_) {
            SDL_DestroyTexture(texture_);
            texture_ = nullptr;
        }
        textureWidth_ = 0;
        textureHeight_ = 0;
    }

    bool Preview::updateTexture() {
        if (!frameReady_ || frame_.empty())
            return false;

        const double scale = std::min({1.0, static_cast<double>(kMaxTextureWidth) / frame_.cols,
                                       static_cast<double>(kMaxTextureHeight) / frame_.rows});
        cv::Mat scaled;
        if (scale < 1.0) {
            cv::resize(frame_, scaled, cv::Size(), scale, scale, cv::INTER_AREA);
            frame_ = scaled;
        }

        if (!texture_ || textureWidth_ != frame_.cols || textureHeight_ != frame_.rows) {
            if (texture_)
                SDL_DestroyTexture(texture_);
            texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_BGR48, SDL_TEXTUREACCESS_STREAMING, frame_.cols,
                                         frame_.rows);
            if (!texture_) {
                textureWidth_ = 0;
                textureHeight_ = 0;
                return false;
            }
            textureWidth_ = frame_.cols;
            textureHeight_ = frame_.rows;
            SDL_SetTextureScaleMode(texture_, SDL_SCALEMODE_LINEAR);
        }

        if (!SDL_UpdateTexture(texture_, nullptr, frame_.data, static_cast<int>(frame_.step)))
            return false;

        frameReady_ = false;
        return true;
    }

    void Preview::draw() {
        if (frameReady_)
            updateTexture();

        const ImVec2 available = ImGui::GetContentRegionAvail();
        if (!texture_) {
            ImGui::TextDisabled("暂无输入");
            return;
        }

        const float imageWidth = static_cast<float>(textureWidth_);
        const float imageHeight = static_cast<float>(textureHeight_);
        if (imageWidth <= 0.0f || imageHeight <= 0.0f)
            return;

        const float scale = std::min(available.x / imageWidth, available.y / imageHeight);
        const ImVec2 size(imageWidth * scale, imageHeight * scale);
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + (available.x - size.x) * 0.5f,
                                   ImGui::GetCursorPosY() + (available.y - size.y) * 0.5f));
        ImGui::Image(ImTextureRef(static_cast<void *>(texture_)), size);
    }
} // namespace app::components
