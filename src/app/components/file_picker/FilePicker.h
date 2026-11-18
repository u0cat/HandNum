#ifndef APP_COMPONENTS_FILE_PICKER_FILE_PICKER_H
#define APP_COMPONENTS_FILE_PICKER_FILE_PICKER_H

#include <array>
#include <filesystem>
#include <string>
#include <vector>

namespace app::components {
    class FilePicker {
    public:
        struct Config {
            std::string title = "选择文件";
            std::string directoryLabel = "目录";
            std::string navigateButtonLabel = "前往";
            std::string parentDirectoryButtonLabel = "上一级";
            std::string directoryEntryPrefix = "[目录] ";
            std::string filterDescription;
            std::string noFilesMessage = "此目录下没有符合条件的文件。";
            std::string openButtonLabel = "打开";
            std::string cancelButtonLabel = "取消";
            std::string directoryErrorPrefix = "无法读取目录：";
            std::string iterationErrorPrefix = "读取目录时出错：";
            std::string invalidDirectoryMessage = "输入路径不是一个目录。";
            std::vector<std::string> extensions;
        };

        void open(Config config);
        bool draw(std::string &selectedPath);

    private:
        struct Entry {
            std::filesystem::path path;
            bool isDirectory;
        };

        Config config_;
        std::filesystem::path directory_;
        std::vector<Entry> entries_;
        std::array<char, 1024> directoryInput_{};
        std::string selectedPath_;
        std::string errorMessage_;
        bool opened_ = false;

        bool accepts(const std::filesystem::path &path) const;
        void refresh();
        void navigateToInput();
    };
} // namespace app::components

#endif // APP_COMPONENTS_FILE_PICKER_FILE_PICKER_H
