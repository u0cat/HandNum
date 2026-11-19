#include "App.h"

#include <filesystem>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "screen/MainScreen/MainScreen.h"

int main(int, char **) {
    app::App handNum{};

    auto *screens = handNum.getScreenMgr();
    screens->add("main", std::make_unique<app::screen::MainScreen>(handNum.getRenderer()));

    return handNum.run();
}

namespace app {
    App::App() : screenMgr_(std::make_unique<screen::ScreenManager>()) {
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            return;
        }

        window_ = SDL_CreateWindow("Handnum", 1280, 720, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if (!window_) {
            SDL_Quit();
            return;
        }

        renderer_ = SDL_CreateRenderer(window_, nullptr);
        if (!renderer_) {
            SDL_DestroyWindow(window_);
            SDL_Quit();
            return;
        }

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        const char *fontPaths[] = {
                "/usr/share/fonts/noto-cjk/NotoSansCJK-Regular.ttc",
                "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
                "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
                "/usr/share/fonts/google-noto-cjk/NotoSansCJK-Regular.ttc",
                "C:/Windows/Fonts/msyh.ttc",
                "/System/Library/Fonts/PingFang.ttc",
        };
        ImFont *chineseFont = nullptr;
        for (const char *fontPath: fontPaths) {
            if (std::filesystem::exists(fontPath)) {
                chineseFont =
                        io.Fonts->AddFontFromFileTTF(fontPath, 18.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
                if (chineseFont)
                    break;
            }
        }
        if (chineseFont) {
            io.FontDefault = chineseFont;
        } else {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "未找到支持中文的字体");
        }
        ImGui::StyleColorsDark();
        ImGui_ImplSDL3_InitForSDLRenderer(window_, renderer_);
        ImGui_ImplSDLRenderer3_Init(renderer_);
    }

    App::~App() {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        screenMgr_.reset();
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    int App::run() {
        running_ = true;
        while (running_) {
            SDL_Event event = {};
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL3_ProcessEvent(&event);
                if (event.type == SDL_EVENT_QUIT)
                    running_ = false;
                if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window_))
                    running_ = false;
            }

            render();
        }
        return 0;
    }

    screen::ScreenManager *App::getScreenMgr() { return screenMgr_.get(); }

    void App::render() {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        screenMgr_->render();

        ImGui::Render();
        SDL_RenderClear(renderer_);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer_);
        SDL_RenderPresent(renderer_);
    }
} // namespace app
