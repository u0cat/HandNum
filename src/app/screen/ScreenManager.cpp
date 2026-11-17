#include "ScreenManager.h"

#include <utility>

namespace app::screen {
    ScreenManager::ScreenManager() {}

    bool ScreenManager::add(const std::string &screenName, std::unique_ptr<Screen> screen) {
        if (auto it = screens_.find(screenName); it == screens_.end()) {
            screen->attach(this);
            screens_[screenName] = std::move(screen);
            return true;
        }
        return false;
    }

    bool ScreenManager::remove(const std::string &screenName) {
        if (screens_.erase(screenName) > 0)
            return true;
        return false;
    }

    bool ScreenManager::disable(const std::string &screenName) {
        if (auto it = screens_.find(screenName); it != screens_.end()) {
            it->second->disable();
            return true;
        }
        return false;
    }

    bool ScreenManager::enable(const std::string &screenName) {
        if (auto it = screens_.find(screenName); it != screens_.end()) {
            it->second->enable();
            return true;
        }
        return false;
    }

    Screen *ScreenManager::getScreen(const std::string &screenName) {
        if (auto it = screens_.find(screenName); it != screens_.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void ScreenManager::render() {
        for (auto &[_, screen]: screens_) {
            screen->render();
        }
    }
} // namespace app::screen
