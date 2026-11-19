#include "ScreenManager.h"

#include <utility>
#include <vector>

namespace app::screen {
    ScreenManager::ScreenManager() {}

    bool ScreenManager::add(const std::string &screenName, std::unique_ptr<Screen> screen) {
        if (auto it = screens_.find(screenName); it == screens_.end()) {
            screen->attach(this);
            screens_[screenName] = std::move(screen);
            screenStates_[screenName] = true;
            return true;
        }
        return false;
    }

    bool ScreenManager::remove(const std::string &screenName) {
        if (screens_.erase(screenName) > 0) {
            screenStates_.erase(screenName);
            return true;
        }
        return false;
    }

    bool ScreenManager::disable(const std::string &screenName) {
        if (screens_.contains(screenName)) {
            screenStates_[screenName] = false;
            return true;
        }
        return false;
    }

    bool ScreenManager::enable(const std::string &screenName) {
        if (screens_.contains(screenName)) {
            screenStates_[screenName] = true;
            return true;
        }
        return false;
    }

    bool ScreenManager::isEnabled(const std::string &screenName) const {
        if (const auto it = screenStates_.find(screenName); it != screenStates_.end())
            return it->second;
        return false;
    }

    Screen *ScreenManager::getScreen(const std::string &screenName) {
        if (auto it = screens_.find(screenName); it != screens_.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    void ScreenManager::render() {
        std::vector<std::pair<std::string, Screen *>> enabled;
        enabled.reserve(screens_.size());

        for (const auto &[name, screen]: screens_)
            if (isEnabled(name))
                enabled.emplace_back(name, screen.get());

        for (const auto &[name, screen]: enabled)
            if (isEnabled(name) && getScreen(name) == screen)
                screen->render();
    }
} // namespace app::screen
