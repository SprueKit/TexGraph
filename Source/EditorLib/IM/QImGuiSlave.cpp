#include "QImGuiSlave.h"

#include "QImGui.h"

#include <EditorLib/ApplicationCore.h>

#include <QPainter>

#include <Windows.h>

QImGuiSlave::QImGuiSlave(QImGui* owner) :
    QWidget(ApplicationCore::GetMainWindow()),
    owner_(owner)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowSystemMenuHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFocusPolicy(Qt::NoFocus);

    //auto nativeHandle = winId();
    //
    ////SetParent((HWND)nativeHandle, 0);
    //auto currentFlags = GetWindowLong((HWND)nativeHandle, GWL_EXSTYLE);
    //ShowWindow((HWND)nativeHandle, SW_HIDE);
    //currentFlags |= (WS_EX_PALETTEWINDOW | WS_EX_NOACTIVATE) & ~WS_CHILD;
    //SetWindowLong((HWND)nativeHandle, GWL_EXSTYLE, currentFlags);
    ////SetParent((HWND)nativeHandle, 0);
    ////SetParent((HWND)nativeHandle, (HWND)ApplicationCore::GetMainWindow()->winId());
    //ShowWindow((HWND)nativeHandle, SW_SHOW);

    //setMouseTracking(true);
}

QImGuiSlave::~QImGuiSlave()
{

}

void QImGuiSlave::SetDrawList(ImDrawList* srcList)
{
    drawList.Clear();

    drawList._ClipRectStack = srcList->_ClipRectStack;
    drawList.IdxBuffer = srcList->IdxBuffer;
    drawList.VtxBuffer = srcList->VtxBuffer;

    bool setGeom = false;

    // Copy commands
    for (int i = 0; i < srcList->CmdBuffer.size(); ++i)
    {
        auto& cmd = srcList->CmdBuffer[i];

        // Bypass empty objects
        if (cmd.CmdType == IMCMD_Buffer && cmd.ElemCount == 0)
            continue;

        if (cmd.CmdType == IMCMD_Rect && !setGeom)
        {
            auto& rectData = cmd.CmdData.RectData;
            QPoint ownerOffset = owner_->mapToGlobal(QPoint(0, 0));
            QRect r(rectData.Rectangle.x + ownerOffset.x(), rectData.Rectangle.y + ownerOffset.y(), rectData.Rectangle.z - rectData.Rectangle.x, rectData.Rectangle.w - rectData.Rectangle.y);
            QRect geo = geometry();
            if (geo != r)
            {
                setGeometry(r.left(), r.top(), r.width(), r.height());
                setFixedWidth(r.width());
                setFixedHeight(r.height());
                paintOffset_ = QPoint(rectData.Rectangle.x, rectData.Rectangle.y);
            }
            setGeom = true;
        }

        drawList.AddDrawCmd();
        ImDrawCmd& cloneCmd = drawList.CmdBuffer.back();
        cloneCmd = cmd;
        if (cmd.CmdType == IMCMD_Text)
        {
            cloneCmd.CmdData.TextData.Text = new char[cloneCmd.CmdData.TextData.Length + 1];
            memset((void*)cloneCmd.CmdData.TextData.Text, 0, cloneCmd.CmdData.TextData.Length + 1);
            memcpy((void*)cloneCmd.CmdData.TextData.Text, cmd.CmdData.TextData.Text, cmd.CmdData.TextData.Length);
        }
    }
    
    receivedDraws_ = drawList.CmdBuffer.size() > 0;

    update();
}

void QImGuiSlave::Reset()
{
    drawList.Clear();
}

void QImGuiSlave::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);

    if (drawList.CmdBuffer.size())
        QImGui::RenderDrawList(owner_, &painter, &drawList, paintOffset_);

    painter.end();
}

void QImGuiSlave::UpdatePositioning()
{
    for (int i = 0; i < drawList.CmdBuffer.size(); ++i)
    {
        auto& cmd = drawList.CmdBuffer[i];
        if (cmd.CmdType == IMCMD_Rect)
        {
            auto& rectData = cmd.CmdData.RectData;
            QPoint ownerOffset = owner_->mapToGlobal(QPoint(0, 0));
            QRect r(rectData.Rectangle.x + ownerOffset.x(), rectData.Rectangle.y + ownerOffset.y(), rectData.Rectangle.z - rectData.Rectangle.x, rectData.Rectangle.w - rectData.Rectangle.y);
            QRect geo = geometry();
            if (geo != r)
            {
                setGeometry(r.left(), r.top(), r.width(), r.height());
                setFixedWidth(r.width());
                setFixedHeight(r.height());
                paintOffset_ = QPoint(rectData.Rectangle.x, rectData.Rectangle.y);
            }
            return;
        }
    }
}

void QImGuiSlave::focusInEvent(QFocusEvent*)
{
    ApplicationCore::GetMainWindow()->setFocus();
}