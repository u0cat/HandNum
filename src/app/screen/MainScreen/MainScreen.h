#ifndef APP_SCREEN_MAINSCREEN_MAINSCREEN_H
#define APP_SCREEN_MAINSCREEN_MAINSCREEN_H

#include <SDL3/SDL.h>

#include "screen/Screen.h"

namespace app::screen {
    class MainScreen : public Screen {
    public:
        explicit MainScreen(SDL_Renderer *renderer) : renderer_(renderer) {}

    private:
        SDL_Renderer *renderer_ = nullptr;

        void render() override;
    };
} // namespace app::screen

#endif // APP_SCREEN_MAINSCREEN_MAINSCREEN_H
