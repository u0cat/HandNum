#include "PredictScreen.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "core/KNNCore.h"
#include "core/SVMCore.h"
#include "core/utils/DatasetLoader.h"
#include "core/utils/DigitNormalizer.h"
#include "imgui.h"

namespace app::screen {
    namespace {
        constexpr float kStrokeWidth = 22.0f;
    }

    void PredictScreen::initCore() {
        if (selectedCore_ == currentCore_)
            return;

        auto ensureTrainDataset = [this]() -> core::models::Dataset & {
            if (!trainDataset_) {
                const auto dataset = core::utils::splitDataset(
                        core::utils::loadMnistDataset("data/raw/mnist/train-images-idx3-ubyte",
                                                      "data/raw/mnist/train-labels-idx1-ubyte"),
                        40000);
                trainDataset_ = std::make_unique<core::models::Dataset>(
                        core::utils::DigitNormalizer::normalizeDataset(dataset));
            }
            return *trainDataset_;
        };

        switch (selectedCore_) {
            case 2: core_ = std::make_unique<core::SVMCore>(SVM_MODEL_PATH); break;

            case 0: core_ = std::make_unique<core::OpenCVKNNCore>(ensureTrainDataset()); break;

            case 1: core_ = std::make_unique<core::CustomKNNCore>(ensureTrainDataset(), 3); break;

            default: return;
        }

        if (selectedCore_ == 0 || selectedCore_ == 1)
            core_->train();

        currentCore_ = selectedCore_;
    }

    void PredictScreen::predict() {
        if (canvas_.strokes().empty()) {
            errorMessage_ = "请先写一个数字。";
            prediction_ = -1;
            return;
        }

        try {
            cv::Mat image(280, 280, CV_8UC1, cv::Scalar(0));
            for (const auto &stroke: canvas_.strokes()) {
                const auto &points = stroke.smoothedPoints;
                if (points.empty())
                    continue;

                auto toPixel = [](const components::canvas::InputPoint &point) { return cv::Point(point.x, point.y); };

                if (points.size() == 1) {
                    cv::circle(image, toPixel(points.front()), cvRound(kStrokeWidth / 2.0f), cv::Scalar(255),
                               cv::FILLED, cv::LINE_AA);
                    continue;
                }

                for (std::size_t i = 1; i < points.size(); ++i) {
                    cv::line(image, toPixel(points[i - 1]), toPixel(points[i]), cv::Scalar(255), cvRound(kStrokeWidth),
                             cv::LINE_AA);
                }
                cv::circle(image, toPixel(points.front()), cvRound(kStrokeWidth / 2.0f), cv::Scalar(255), cv::FILLED,
                           cv::LINE_AA);
                cv::circle(image, toPixel(points.back()), cvRound(kStrokeWidth / 2.0f), cv::Scalar(255), cv::FILLED,
                           cv::LINE_AA);
            }

            initCore();
            cv::Mat sample = image;
            if (selectedCore_ == 0 || selectedCore_ == 1) {
                sample = core::utils::DigitNormalizer::normalize(image);
                sample = sample.reshape(1, 1);
            }
            prediction_ = core_->predictNum(sample);
            errorMessage_.clear();
        } catch (const cv::Exception &exception) {
            prediction_ = -1;
            errorMessage_ = std::string("推理失败：") + exception.what();
            std::cout << exception.what();
        } catch (const std::exception &exception) {
            prediction_ = -1;
            errorMessage_ = std::string("推理失败：") + exception.what();
            std::cout << exception.what();
        }
    }

    void PredictScreen::render() {
        ImGui::SetNextWindowSize(ImVec2(1080.0f, 680.0f), ImGuiCond_FirstUseEver);
        if (!ImGui::Begin("Prediction")) {
            ImGui::End();
            return;
        }

        ImGui::TextUnformatted("");
        ImGui::Spacing();

        const float gap = ImGui::GetStyle().ItemSpacing.x;
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const float leftWidth = (available.x - gap) * 0.62f;
        const float rightWidth = available.x - gap - leftWidth;

        ImGui::BeginChild("DrawingPanel", ImVec2(leftWidth, 0.0f), true);
        ImGui::Spacing();

        if (canvas_.draw()) {
            prediction_ = -1;
            errorMessage_.clear();
        }
        if (ImGui::Button("识别", ImVec2(130.0f, 36.0f)))
            predict();
        ImGui::SameLine();
        if (ImGui::Button("清除", ImVec2(100.0f, 36.0f))) {
            canvas_.clear();
            prediction_ = -1;
            errorMessage_.clear();
        }
        ImGui::EndChild();

        ImGui::SameLine(0.0f, gap);
        ImGui::BeginChild("ResultPanel", ImVec2(rightWidth, 0.0f), true);
        ImGui::Combo("模型", &selectedCore_, kCoreTypes, IM_COUNTOF(kCoreTypes));
        ImGui::Spacing();

        if (prediction_ >= 0) {
            ImGui::TextUnformatted("你写的大概是");
            ImGui::Spacing();
            const float digitWidth = ImGui::CalcTextSize("0").x * 5.0f;
            ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetContentRegionAvail().x - digitWidth) * 0.5f));
            ImGui::SetWindowFontScale(5.0f);
            ImGui::Text("%d", prediction_);
            ImGui::SetWindowFontScale(1.0f);
        } else {
            ImGui::TextDisabled("暂无结果");
        }

        if (!errorMessage_.empty()) {
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 120, 120, 255));
            ImGui::TextWrapped("%s", errorMessage_.c_str());
            ImGui::PopStyleColor();
        }
        ImGui::EndChild();

        ImGui::End();
    }
} // namespace app::screen
