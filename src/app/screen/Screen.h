#ifndef APP_SCREEN_SCREEN_H
#define APP_SCREEN_SCREEN_H

namespace app::screen {
    class ScreenManager;

    class Screen {
    public:
        virtual ~Screen() = default;

    protected:
        bool isEnabled_ = true;
        ScreenManager *mgr_ = nullptr;

    private:
        friend class ScreenManager;

        void attach(ScreenManager *mgr) { mgr_ = mgr; }

        virtual void render() {}
        virtual void disable() { isEnabled_ = false; }
        virtual void enable() { isEnabled_ = true; }
    };
} // namespace app::screen

#endif // APP_SCREEN_SCREEN_H
