#pragma once

#include <EditorLib/editorlib_global.h>

#include <vector>

#include <QRect>
#include <QStyle>

/// ItemID's are used to identify things in collections, the integer provides a safe key for purely functional leaves, while the pointer should suffice for others
/// possibly using the integral ID as a type identifier.
typedef std::pair<int, void*> IMItemID;

/// Enumerates the available commands for IMCmdRect's to utilize.
/// Custom commands can begin with ICMD_UserCmdStart id and do not need to be globally unique,
/// only unique to the IM-style widget producing them.
enum IMCommands
{
    IMCMD_Invalid = 0,
    IMCMD_ToggleExpansion = -1,
};

/// A hotspot that contains something we can interact with via clicking.
struct EDITORLIB_EXPORT IMCmdRect {
    IMItemID item_;
    int commandType_;
    QRect rect_;

    IMCmdRect() { }
    IMCmdRect(IMItemID item, int cmd, QRect rect) : item_(item), commandType_(cmd), rect_(rect) { }
};

class EDITORLIB_EXPORT IMUserInterface {
public:
    virtual void PushCommandRect(const IMCmdRect& rect) { rects_.push_back(rect); }
    virtual void ClearCommandRects() { rects_.clear(); }
    virtual bool CheckCommandRects(IMCmdRect& holder, const QPoint& pos) {
        for (unsigned i = 0; i < rects_.size(); ++i)
        {
            if (rects_[i].rect_.contains(pos))
            {
                holder = rects_[i];
                return true;
            }
        }
        return false;
    }

protected:
    std::vector<IMCmdRect> rects_;
};
