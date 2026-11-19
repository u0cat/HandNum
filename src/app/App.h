#ifndef APP_APP_H
#define APP_APP_H
#include <SDL3/SDL.h>

#include <memory>

#include "screen/ScreenManager.h"

namespace app {
    class App {
    public:
        App();
        ~App();

        int run();

        screen::ScreenManager *getScreenMgr();
        SDL_Window *getWindow() const { return window_; }
        SDL_Renderer *getRenderer() const { return renderer_; }

    private:
        bool running_ = false;
        std::unique_ptr<screen::ScreenManager> screenMgr_;

        SDL_Renderer *renderer_ = nullptr;
        SDL_Window *window_ = nullptr;

        void render();
    };
} // namespace app

#endif // APP_APP_H
