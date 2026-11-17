#include "canvas.h"

#include <algorithm>
#include <cmath>

#include "imgui.h"

namespace app::components {
    namespace {
        constexpr float kReferenceSize = 280.0f;
        constexpr float kStrokeWidth = 22.0f;

        ImVec2 toScreen(const canvas::InputPoint &point, const ImVec2 &origin, float side) {
            return ImVec2(origin.x + point.x / (kReferenceSize - 1.0f) * side,
                          origin.y + point.y / (kReferenceSize - 1.0f) * side);
        }

        ImVec2 interpolate(const ImVec2 &p0, const ImVec2 &p1, const ImVec2 &p2, const ImVec2 &p3, float t) {
            const float t2 = t * t;
            const float t3 = t2 * t;
            return ImVec2(
                    0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                            (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3),
                    0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                            (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3));
        }

        void drawStroke(ImDrawList *drawList, const std::vector<canvas::InputPoint> &points, const ImVec2 &origin,
                        float side, float width, ImU32 color) {
            if (points.empty())
                return;

            const float radius = width * 0.5f;
            if (points.size() == 1) {
                drawList->AddCircleFilled(toScreen(points.front(), origin, side), radius, color);
                return;
            }

            const float spacing = std::max(2.0f, radius * 0.45f);
            for (std::size_t i = 0; i + 1 < points.size(); ++i) {
                const ImVec2 p0 = toScreen(points[i == 0 ? i : i - 1], origin, side);
                const ImVec2 p1 = toScreen(points[i], origin, side);
                const ImVec2 p2 = toScreen(points[i + 1], origin, side);
                const ImVec2 p3 = toScreen(points[std::min(i + 2, points.size() - 1)], origin, side);
                const float distance = std::hypot(p2.x - p1.x, p2.y - p1.y);
                const int steps = std::max(1, static_cast<int>(std::ceil(distance / spacing)));
                for (int step = 0; step < steps; ++step) {
                    const ImVec2 point = interpolate(p0, p1, p2, p3, static_cast<float>(step) / steps);
                    drawList->AddCircleFilled(point, radius, color);
                }
            }
            drawList->AddCircleFilled(toScreen(points.back(), origin, side), radius, color);
        }
    } // namespace

    bool Canvas::draw() {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const float side = std::max(1.0f, std::min(available.x, available.y - 58.0f));
        const ImVec2 cursor = ImGui::GetCursorScreenPos();
        const ImVec2 origin(cursor.x + std::max(0.0f, (available.x - side) * 0.5f), cursor.y);
        const ImVec2 bottom(origin.x + side, origin.y + side);

        ImGui::SetCursorScreenPos(origin);
        ImGui::InvisibleButton("DigitCanvas", ImVec2(side, side), ImGuiButtonFlags_MouseButtonLeft);

        ImDrawList *drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(origin, bottom, IM_COL32(13, 18, 30, 255), 8.0f);
        drawList->AddRect(origin, bottom, IM_COL32(71, 85, 105, 255), 8.0f, 0, 1.0f);

        bool changed = false;
        const ImVec2 mouse = ImGui::GetIO().MousePos;
        if (ImGui::IsItemActivated() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            strokes_.push_back({nextStrokeId_++, {}, {}});
            const int x = static_cast<int>(
                    std::lround(std::clamp((mouse.x - origin.x) / side, 0.0f, 1.0f) * (kReferenceSize - 1.0f)));
            const int y = static_cast<int>(
                    std::lround(std::clamp((mouse.y - origin.y) / side, 0.0f, 1.0f) * (kReferenceSize - 1.0f)));
            addPoint(strokes_.back(), x, y);
            changed = true;
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !strokes_.empty()) {
            const int x = static_cast<int>(
                    std::lround(std::clamp((mouse.x - origin.x) / side, 0.0f, 1.0f) * (kReferenceSize - 1.0f)));
            const int y = static_cast<int>(
                    std::lround(std::clamp((mouse.y - origin.y) / side, 0.0f, 1.0f) * (kReferenceSize - 1.0f)));
            auto &stroke = strokes_.back();
            if (stroke.rawPoints.empty() || std::hypot(static_cast<float>(x - stroke.rawPoints.back().x),
                                                       static_cast<float>(y - stroke.rawPoints.back().y)) >= 1.5f) {
                addPoint(stroke, x, y);
                changed = true;
            }
        }

        const float lineWidth = kStrokeWidth * side / kReferenceSize;
        const ImU32 color = IM_COL32(245, 248, 255, 255);
        for (const auto &stroke: strokes_)
            drawStroke(drawList, stroke.smoothedPoints, origin, side, lineWidth, color);

        ImGui::SetCursorScreenPos(ImVec2(origin.x, bottom.y + 12.0f));
        return changed;
    }

    void Canvas::clear() {
        strokes_.clear();
        nextStrokeId_ = 0;
    }

    const std::vector<canvas::Stroke> &Canvas::strokes() const { return strokes_; }

    void Canvas::addPoint(canvas::Stroke &stroke, int x, int y) {
        const double timestamp = ImGui::GetTime();
        stroke.rawPoints.push_back({x, y, timestamp});

        if (stroke.smoothedPoints.empty()) {
            stroke.smoothedPoints.push_back({x, y, timestamp});
            return;
        }

        const auto &previous = stroke.smoothedPoints.back();
        stroke.smoothedPoints.push_back({static_cast<int>(std::lround(previous.x * 0.65 + x * 0.35)),
                                         static_cast<int>(std::lround(previous.y * 0.65 + y * 0.35)), timestamp});
    }
} // namespace app::components
