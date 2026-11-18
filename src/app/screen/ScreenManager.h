#ifndef APP_SCREEN_MANAGER_H
#define APP_SCREEN_MANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "Screen.h"

namespace app {
    class App;
}

namespace app::screen {
    class ScreenManager {
    public:
        ScreenManager();

        bool add(const std::string &screenName, std::unique_ptr<Screen> screen);
        bool remove(const std::string &screenName);
        
        bool disable(const std::string &screenName);
        bool enable(const std::string &screenName);
        bool isEnabled(const std::string &screenName) const;

        Screen *getScreen(const std::string &screenName);

    private:
        std::unordered_map<std::string, std::unique_ptr<Screen>> screens_;
        std::unordered_map<std::string, bool> screenStates_;
        friend class app::App;
        void render();
    };
} // namespace app::screen
#endif // APP_SCREEN_MANAGER_H
