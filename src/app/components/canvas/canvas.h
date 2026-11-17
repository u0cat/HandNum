#ifndef APP_COMPONENTS_CANVAS_H
#define APP_COMPONENTS_CANVAS_H

#include <vector>

namespace app::components::canvas {
    struct InputPoint {
        int x;
        int y;
        double timestamp;
    };

    struct Stroke {
        int id;
        std::vector<InputPoint> rawPoints;
        std::vector<InputPoint> smoothedPoints;
    };
} // namespace app::components::canvas

namespace app::components {
    class Canvas {
    public:
        bool draw();
        void clear();

        const std::vector<canvas::Stroke> &strokes() const;

    private:
        std::vector<canvas::Stroke> strokes_;
        int nextStrokeId_ = 0;

        void addPoint(canvas::Stroke &stroke, int x, int y);
    };
} // namespace app::components

#endif // APP_COMPONENTS_CANVAS_H
