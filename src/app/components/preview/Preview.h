#ifndef APP_COMPONENTS_PREVIEW_PREVIEW_H
#define APP_COMPONENTS_PREVIEW_PREVIEW_H

#include <SDL3/SDL.h>

#include <opencv2/core/mat.hpp>

namespace app::components {
    class Preview {
    public:
        Preview(SDL_Renderer *renderer);
        ~Preview();

        Preview(const Preview &) = delete;
        Preview &operator=(const Preview &) = delete;

        bool setFrame(const cv::Mat &frame);
        void clear();
        void draw();

    private:
        SDL_Renderer *renderer_ = nullptr;
        SDL_Texture *texture_ = nullptr;
        int textureWidth_ = 0;
        int textureHeight_ = 0;
        cv::Mat frame_;
        bool frameReady_ = false;

        bool updateTexture();
    };
} // namespace app::components

#endif // APP_COMPONENTS_PREVIEW_PREVIEW_H
