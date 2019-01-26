#pragma once

#include <EditorLib/ThirdParty/imgui.h>

#include <QWidget>

class QImGui;

class QImGuiSlave : public QWidget
{
    friend class QImGui;

    Q_OBJECT;
public:
    QImGuiSlave(QImGui* owner);
    virtual ~QImGuiSlave();

    void SetDrawList(ImDrawList* srcList);

    void Reset();

    bool ReceivedDraws() { return receivedDraws_; }

protected:
    virtual void focusInEvent(QFocusEvent*) Q_DECL_OVERRIDE;
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

    void PrepareFrame() { receivedDraws_ = false; }
    void UpdatePositioning();

    QPoint paintOffset_;
    QImGui* owner_;
    ImDrawList drawList;
    bool receivedDraws_ = false;
};