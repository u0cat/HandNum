#include "FilePicker.h"

#include <algorithm>
#include <cctype>
#include <system_error>

#include "imgui.h"

namespace app::components {
    namespace {
        std::string pathToString(const std::filesystem::path &path) {
            const auto value = path.u8string();
            return std::string(value.begin(), value.end());
        }

        std::string lowercase(std::string value) {
            std::transform(value.begin(), value.end(), value.begin(),
                           [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
            return value;
        }
    } // namespace

    void FilePicker::open(Config config) {
        config_ = std::move(config);
        selectedPath_.clear();
        std::error_code error;
        directory_ = std::filesystem::current_path(error);
        if (error)
            directory_ = ".";
        refresh();
        opened_ = true;
        ImGui::PushID(this);
        ImGui::OpenPopup("FilePicker");
        ImGui::PopID();
    }

    bool FilePicker::accepts(const std::filesystem::path &path) const {
        if (config_.extensions.empty())
            return true;

        const std::string extension = lowercase(pathToString(path.extension()));
        for (auto allowedExtension : config_.extensions) {
            if (!allowedExtension.empty() && allowedExtension.front() != '.')
                allowedExtension.insert(allowedExtension.begin(), '.');
            if (extension == lowercase(std::move(allowedExtension)))
                return true;
        }
        return false;
    }

    void FilePicker::refresh() {
        entries_.clear();
        selectedPath_.clear();
        errorMessage_.clear();

        std::error_code error;
        std::filesystem::directory_iterator iterator(directory_, error);
        if (error) {
            errorMessage_ = config_.directoryErrorPrefix + error.message();
            return;
        }

        const std::filesystem::directory_iterator end;
        while (iterator != end) {
            const auto &entry = *iterator;
            std::error_code entryError;
            const bool isDirectory = entry.is_directory(entryError);
            if (!entryError &&
                (isDirectory || (entry.is_regular_file(entryError) && !entryError && accepts(entry.path()))))
                entries_.push_back({entry.path(), isDirectory});

            iterator.increment(error);
            if (error) {
                errorMessage_ = config_.iterationErrorPrefix + error.message();
                break;
            }
        }

        std::sort(entries_.begin(), entries_.end(), [](const Entry &left, const Entry &right) {
            if (left.isDirectory != right.isDirectory)
                return left.isDirectory;
            return pathToString(left.path.filename()) < pathToString(right.path.filename());
        });

        const std::string directory = pathToString(directory_);
        const std::size_t length = std::min(directory.size(), directoryInput_.size() - 1);
        std::copy_n(directory.begin(), length, directoryInput_.begin());
        directoryInput_[length] = '\0';
    }

    void FilePicker::navigateToInput() {
        std::error_code error;
        const std::filesystem::path requested(directoryInput_.data());
        if (std::filesystem::is_directory(requested, error)) {
            directory_ = requested;
            refresh();
        } else {
            errorMessage_ = error ? error.message() : config_.invalidDirectoryMessage;
        }
    }

    bool FilePicker::draw(std::string &selectedPath) {
        ImGui::PushID(this);
        if (!opened_ || !ImGui::BeginPopupModal("FilePickerPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::PopID();
            return false;
        }

        ImGui::TextUnformatted(config_.title.c_str());
        ImGui::SetNextItemWidth(480.0f);
        if (ImGui::InputText(config_.directoryLabel.c_str(), directoryInput_.data(), directoryInput_.size(),
                             ImGuiInputTextFlags_EnterReturnsTrue))
            navigateToInput();
        ImGui::SameLine();
        if (ImGui::Button(config_.navigateButtonLabel.c_str()))
            navigateToInput();
        if (ImGui::Button(config_.parentDirectoryButtonLabel.c_str())) {
            const auto parent = directory_.parent_path();
            if (!parent.empty()) {
                directory_ = parent;
                refresh();
            }
        }

        if (!config_.filterDescription.empty())
            ImGui::TextWrapped("%s", config_.filterDescription.c_str());

        ImGui::BeginChild("FilePickerList", ImVec2(560.0f, 340.0f), true);
        if (entries_.empty()) {
            ImGui::TextDisabled("%s", config_.noFilesMessage.c_str());
        } else {
            for (std::size_t i = 0; i < entries_.size(); ++i) {
                const auto &entry = entries_[i];
                const std::string label =
                        (entry.isDirectory ? config_.directoryEntryPrefix : "") + pathToString(entry.path.filename());
                ImGui::PushID(static_cast<int>(i));
                if (entry.isDirectory) {
                    if (ImGui::Selectable(label.c_str())) {
                        directory_ = entry.path;
                        refresh();
                        ImGui::PopID();
                        break;
                    }
                } else if (ImGui::Selectable(label.c_str(), selectedPath_ == pathToString(entry.path))) {
                    selectedPath_ = pathToString(entry.path);
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        if (!selectedPath_.empty())
            ImGui::TextWrapped("%s", selectedPath_.c_str());
        if (!errorMessage_.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 140, 140, 255));
            ImGui::TextWrapped("%s", errorMessage_.c_str());
            ImGui::PopStyleColor();
        }

        bool selected = false;
        if (ImGui::Button(config_.openButtonLabel.c_str(), ImVec2(100.0f, 0.0f)) && !selectedPath_.empty()) {
            selectedPath = selectedPath_;
            selected = true;
            opened_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(config_.cancelButtonLabel.c_str(), ImVec2(100.0f, 0.0f))) {
            opened_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
        ImGui::PopID();
        return selected;
    }
} // namespace app::components
