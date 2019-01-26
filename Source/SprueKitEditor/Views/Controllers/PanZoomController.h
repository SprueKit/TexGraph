#pragma once

#include "../ViewController.h"

namespace SprueEditor
{
    /// Controller for handling a 2D panning/zoom view (like a UV chart or image).
    /// While the camera is 3D this controller only moves the camera along XY or XZ axes ('overhead' setting).
    class PanZoomController : public ViewController
    {
    public:
        /// Construct.
        PanZoomController(ViewManager* manager, ViewBase* view, InputRecorder* input);

        /// Performs keyboard checking for viewport movement.
        virtual void PassiveChecks(const ViewController* currentActive) override;
        /// Will activate if the Middle or Right mouse are held down.
        virtual bool CheckActivation(const ViewController* currentlyActive) override;
        /// Maintains activation if the Middle or Right mouse button are still down.
        virtual bool CheckTermination() override;
        /// Translates mouse movement deltas into viewport movement.
        virtual void Update();

        /// Returns true if the camera moves on Z instead of Y.
        bool IsOverhead() const { return isOverhead_; }
        /// Sets whether the camera moves on the Z (true) or Y axis (false).
        void SetOverhead(bool state) { isOverhead_ = state; }

    private:
        /// If overhead the Z position is changed instead of the Y position.
        bool isOverhead_ = false;
    };

}