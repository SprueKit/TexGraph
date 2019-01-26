#include "Repeater.h"

#include <QApplication>
#include <QDataStream>
#include <QDrag>
#include <QLayout>
#include <QMimeData>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QStyle>
#include <QStyleOption.h>
#include <QTimer>
#include <qevent.h>

#include <EditorLib/ScrollAreaWidget.h>

#define SELECTION_COLOR QColor(61, 174, 233)
#define DROP_TARGET_COLOR QColor(255,0,0)
#define ZEBRA_STRIPE_COLOR QColor(40,40,40)
#define SEPERATOR_COLOR QColor(0,0,0)

#define DRAG_DROP_NAME "REPEATER_INTERNAL_DRAG_DATA"

#define TIMER_INTERVAL 300

ScrollAreaWidget* Repeater::WrapInStandardScrollArea(Repeater* repeater)
{
    ScrollAreaWidget* wrapper = new ScrollAreaWidget();
    wrapper->setProperty("dark", true);
    wrapper->GetHiddenWidget()->setProperty("dark", true);
    wrapper->GetLayout()->setContentsMargins(0, 0, 0, 0);
    wrapper->GetLayout()->addWidget(repeater);
    return wrapper;
}

std::vector<IMItemID> Repeater_ExtractMimeIDs(const QMimeData* data)
{
    std::vector<IMItemID> ret;

    QByteArray dataArr = data->data("/data/");
    QDataStream str(&dataArr, QIODevice::ReadOnly);
    unsigned ct = 0;
    str.readRawData((char*)&ct, sizeof(unsigned));

    for (unsigned i = 0; i < ct; ++i)
    {
        IMItemID id;
        str.readRawData((char*)&(id.first), sizeof(int));
        str.readRawData((char*)&id.second, sizeof(void*));
        ret.push_back(id);
    }

    return ret;
}

Repeater::DropType Repeater::DetermineDropType(const QPoint& pt, const QRect& rect)
{
    if (pt.y() < rect.top() + (rect.height() * 0.25f))
        return Repeater::DT_BEFORE;
    else if (pt.y() > rect.bottom() - (rect.height() * 0.25f))
        return Repeater::DT_AFTER;
    return Repeater::DT_ONTO;
}

Repeater::Repeater(QWidget* parent) :
    QWidget(parent),
    defaultTextColor_(255,255,255)
{
    setAcceptDrops(true);
    chevronCollapsed_ = QIcon(":/qss_icons/rc/branch_closed.png");
    chevronExpanded_ = QIcon(":/qss_icons/rc/branch_open.png");

    updateTimer_.setInterval(TIMER_INTERVAL);
    updateTimer_.setSingleShot(false);
    connect(&updateTimer_, &QTimer::timeout, [=]() {
        update();
    });
    updateTimer_.start();
    paintingState_.Reset();

    QFont font("Segoe UI", 10);
    font.setPixelSize(20);
    QFontMetrics metrics(font);

    shortHeight_ = metrics.height();
    lineHeight_ = metrics.height() + metrics.descent();
    for (int i = 0; i < 512; ++i)
        characterWidths_[i] = metrics.width(i);
}

Repeater::~Repeater()
{

}

bool Repeater::IsExpanded(IMItemID who)
{
    auto found = states_.find(who);
    if (found != states_.end())
        return found->second.expanded_;
    return defaultExpanded_;
}

bool Repeater::IsSelected(IMItemID who)
{
    auto found = std::find(selectedItems_.begin(), selectedItems_.end(), who);
    return found != selectedItems_.end();
}

void Repeater::SetSelected(IMItemID who, bool state, bool repaint)
{
    if (!state)
    {
        auto found = std::find(selectedItems_.begin(), selectedItems_.end(), who);
        if (found != selectedItems_.end())
            selectedItems_.erase(found);
    }
    else
    {
        // in single select mode clear the list
        if (isSingleSelect_)
            selectedItems_.clear();
        selectedItems_.push_back(who);
    }

    emit SelectionChanged(selectedItems_);
    if (repaint)
        DoUpdate();
}

void Repeater::SetExpanded(IMItemID who, bool state, bool repaint)
{
    auto found = states_.find(who);
    if (found != states_.end())
        found->second.expanded_ = state;
    else
        states_[who] = ItemState{ state };

    if (repaint)
        DoUpdate();
}

void Repeater::ClearSelected(bool repaint)
{
    selectedItems_.clear();
    if (repaint)
        DoUpdate();
}

void Repeater::paintEvent(QPaintEvent* paintEvent)
{
    // wipe all of the data we can
    ClearCommandRects();
    visualIndex_.clear();
    rects_.clear();

    QPainter painter;
    painter.begin(this);
    QFont font("Segoe UI", 10);
    font.setPixelSize(20);
    QFontMetrics metrics(font);
    painter.setFont(font);

    paintingState_.Reset();
    paintingState_.position_ = QPoint(rowHeaderWidth_, 0);
    paintingState_.painter_ = &painter;

    GenerateUI();

    if (drawHeaderVert_)
    {
        painter.setPen(QPen(SEPERATOR_COLOR, 2));
        painter.drawLine(rowHeaderWidth_, 0, rowHeaderWidth_, height());
    }

    //TODO: support row footers
    if (drawFooterVert_)
    {

    }

    setMinimumHeight(paintingState_.position_.y());
    setMinimumWidth(paintingState_.maxWidth_);

// Troubleshooting code
    //// Draw item rects
    //painter.setPen(QPen(Qt::red, 1));
    //painter.setBrush(Qt::NoBrush);
    //for (auto r : rects_)
    //    painter.drawRect(r.second);

    //// Draw cmd rects
    //painter.setPen(QPen(Qt::red, 1));
    //painter.setBrush(Qt::NoBrush);
    //for (auto r : IMUserInterface::rects_)
    //    painter.drawRect(r.rect_);

    painter.end();

    Q_ASSERT(paintingState_.treeDepth_ == 0);
}

int Repeater::CalculateTextWidth(const QString& text)
{
    int width = 0;
    for (int i = 0; i < text.length(); ++i)
        width += characterWidths_[text[i].unicode()]; //TODO: unicode
    return width;
}

bool Repeater::eventFilter(QObject *object, QEvent *event)
{
    if (object == embeddedEditor_ && event->type() == QEvent::KeyRelease)
    {
        QKeyEvent* evt = static_cast<QKeyEvent*>(event);
    // Exit the embedded editor
        if (evt->key() == Qt::Key::Key_Escape)
        {
            SetEmbeddedEditor(0x0);
            return true;
        }
    // Move the embedded editor downward
        else if (evt->key() == Qt::Key::Key_Up || evt->key() == Qt::Key::Key_Down)
        {
            const bool isDownward = evt->key() == Qt::Key::Key_Down;

            for (unsigned i = 0; i < visualIndex_.size(); ++i)
            {
                if (visualIndex_[i] == selectedItems_[0])
                {
                    if (i > 0 && !isDownward)
                    {
                        selectedItems_.clear();
                        SetSelected(visualIndex_[i - 1], true);
                        SetEmbeddedEditor(EmbeddedEdit(this, visualIndex_[i - 1], rects_[visualIndex_[i-1]]));
                        return true;
                    }
                    else if (i < visualIndex_.size() - 1 && isDownward)
                    {
                        selectedItems_.clear();
                        SetSelected(visualIndex_[i + 1], true);
                        SetEmbeddedEditor(EmbeddedEdit(this, visualIndex_[i + 1], rects_[visualIndex_[i+1]]));
                        return true;
                    }
                }
            }

            return true;
        }
    }
    return false;
}

void Repeater::contextMenuEvent(QContextMenuEvent* evt)
{
    if (selectedItems_.size())
    {
        if (ContextMenu(selectedItems_, mapToGlobal(evt->pos())))
        {
         //   DoUpdate();
        }
    }
}

void Repeater::mousePressEvent(QMouseEvent* evt)
{
    // Left click
    if (evt->button() == Qt::LeftButton)
    {
        // Command rect checking takes priority
        IMCmdRect cmd;
        if (CheckCommandRects(cmd, evt->pos()))
        {
            if (cmd.commandType_ == IMCMD_ToggleExpansion)
            {
                SetExpanded(cmd.item_, !IsExpanded(cmd.item_));
                //DoUpdate();
            }
            else
            {
                if (ExecuteCommand(cmd))
                {
                 //   DoUpdate();
                }
            }
            return;
        }

        // No commands triggered, check for selection then
        const bool ctrlDown = (evt->modifiers() & Qt::KeyboardModifier::ControlModifier);
        setFocus();

        // Update potential drag start position if ctrl is not down
        if (!ctrlDown)
            dragStart_.reset(new QPoint(evt->pos()));
        else
            dragStart_.reset();

        for (auto r : rects_)
        {
            if (r.second.contains(evt->pos()))
            {
                bool currentState = IsSelected(r.first);
                if (ctrlDown)
                    currentState = !currentState;
                else
                    currentState = true;

                if (!ctrlDown)
                    selectedItems_.clear();

                SetSelected(r.first, currentState);
            }
        }
    }
// If we right-click we'll allow selecting one object via right-mouse
    else if (evt->button() == Qt::RightButton)
    {
        dragStart_.reset();
        dropTarget_.reset();
        IMItemID hitItem;
        if (ItemAt(hitItem, evt->pos(), true))
        {
            // If we hit someone already selected then we leave things as is for multi select
            for (auto current : selectedItems_)
            {
                if (current == hitItem)
                    return;
            }

            // If it's someone not currently selected then we switch everything out
            selectedItems_.clear();
            SetSelected(hitItem, true, false);
        }
    }
}

void Repeater::mouseMoveEvent(QMouseEvent* event)
{
    // Just return if we can't do a drag write now
    if (!dragStart_ || (event->pos() - (*dragStart_)).manhattanLength() < QApplication::startDragDistance())
        return;

    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData();
    QByteArray data;

    unsigned ct = 0;
    for (unsigned i = 0; i < selectedItems_.size(); ++i)
        if (CanDragItem(selectedItems_[i]))
            ++ct;

    if (ct <= 0)
        return;

    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.writeRawData((const char*)&ct, sizeof(unsigned));
    for (unsigned i = 0; i < selectedItems_.size(); ++i)
    {
        if (CanDragItem(selectedItems_[i]))
        {
            stream.writeRawData((const char*)&selectedItems_[i].first, sizeof(int));
            stream.writeRawData((const char*)&selectedItems_[i].second, sizeof(void*));
        }
    }
    
    mimeData->setObjectName(DRAG_DROP_NAME);
    mimeData->setData("/data/", data);
    mimeData->setText("Dragging Repeater Items");
    drag->setMimeData(mimeData);
    
    Qt::DropAction dropAction = drag->exec(Qt::DropAction::MoveAction);
    event->accept();
    ResetDrag();
}

void Repeater::mouseReleaseEvent(QMouseEvent* evt)
{
    //TODO?
    dropTarget_.reset();
    ResetDrag();
}

void Repeater::mouseDoubleClickEvent(QMouseEvent* evt)
{
    for (auto r : rects_)
    {
        if (r.second.contains(evt->pos()))
        {
            selectedItems_.clear();
            SetSelected(r.first, true);
            SetEmbeddedEditor(EmbeddedEdit(this, r.first, r.second));
            evt->accept();
            return;
        }
    }
    selectedItems_.clear();
    emit SelectionChanged(selectedItems_);
    evt->accept();
}

void Repeater::keyReleaseEvent(QKeyEvent* evt)
{
// Return key can activate embedded editor
    if (evt->key() == Qt::Key::Key_Return)
    {
        if (selectedItems_.size() == 1)
        {
            auto item = selectedItems_[0];
            SetEmbeddedEditor(EmbeddedEdit(this, item, rects_[item]));
            evt->accept();
            return;
        }
        evt->accept();
        return;
    }
// Arrow keys move selection up and down
    else if (evt->key() == Qt::Key::Key_Up || evt->key() == Qt::Key::Key_Down)
    {
        const bool isDownward = evt->key() == Qt::Key::Key_Down;

        // nothing to do if no selection
        if (selectedItems_.empty()) { evt->accept(); return; }

        for (unsigned i = 0; i < visualIndex_.size(); ++i)
        {
            if (visualIndex_[i] == selectedItems_[0])
            {
                if (i > 0 && !isDownward)
                {
                    selectedItems_.clear();
                    SetSelected(visualIndex_[i - 1], true);
                    evt->accept();
                    return;
                }
                else if (i < visualIndex_.size() - 1 && isDownward)
                {
                    selectedItems_.clear();
                    SetSelected(visualIndex_[i + 1], true);
                    evt->accept();
                    DoUpdate();
                    return;
                }
            }
        }
    }
// Left and right arrows can toggle expansion states
    else if (evt->key() == Qt::Key::Key_Left || evt->key() == Qt::Key::Key_Right)
    {
        const bool isCollapse = evt->key() == Qt::Key::Key_Left;
        if (selectedItems_.size() == 1)
        {
            SetExpanded(selectedItems_[0], !isCollapse, true);
            evt->accept();
            return;
        }
    }
// Spacebar toggles
    else if (evt->key() == Qt::Key::Key_Space)
    {
        if (selectedItems_.size() == 1)
        {
            SetExpanded(selectedItems_[0], !IsExpanded(selectedItems_[0]));
            evt->accept();
            return;
        }
    }
    else if (evt->key() == Qt::Key::Key_Delete || evt->key() == Qt::Key::Key_Backspace)
    {
        bool anyDeleted = false;
        for (unsigned i = 0; i < selectedItems_.size(); ++i)
        {
            if (DeleteItem(selectedItems_[i]))
            {
                ForgetItem(selectedItems_[i]);
                selectedItems_.erase(selectedItems_.begin() + i);
                --i;
                anyDeleted = true;
            }
        }
        if (anyDeleted)
        {
            emit SelectionChanged(selectedItems_);
            DoUpdate();
        }
    }
}

void Repeater::dragEnterEvent(QDragEnterEvent* evt)
{
    dropTarget_.reset();
    if (evt->mimeData()->objectName().compare(DRAG_DROP_NAME) == 0)
    {
        IMItemID dropOnto;
        QRect itemRect;
        if (ItemAt(dropOnto, evt->pos(), true, &itemRect))
        {
            auto draggingItems = Repeater_ExtractMimeIDs(evt->mimeData());
            auto dropNature = DetermineDropType(evt->pos(), itemRect);
            if (!CanDoDrop(draggingItems, dropOnto, dropNature))
            {
                evt->acceptProposedAction();
                evt->setDropAction(Qt::DropAction::IgnoreAction);
                return;
            }
            evt->setDropAction(Qt::DropAction::MoveAction);
            evt->acceptProposedAction();
            return;
        }
    }
    evt->acceptProposedAction();
    evt->setDropAction(Qt::DropAction::IgnoreAction);
}

void Repeater::dragLeaveEvent(QDragLeaveEvent* evt)
{
    dropTarget_.reset();
}

void Repeater::dragMoveEvent(QDragMoveEvent* evt)
{
    dropTarget_.reset();
    if (evt->mimeData()->objectName().compare(DRAG_DROP_NAME) == 0)
    {
        IMItemID dropOnto;
        QRect itemRect;
        if (ItemAt(dropOnto, evt->pos(), true, &itemRect))
        {
            // Verify that we can do this
            auto draggingItems = Repeater_ExtractMimeIDs(evt->mimeData());
            auto dropNature = DetermineDropType(evt->pos(), itemRect);
            if (!CanDoDrop(draggingItems, dropOnto, dropNature))
            {
                evt->acceptProposedAction();
                evt->setDropAction(Qt::DropAction::IgnoreAction);
                return;
            }
            evt->setDropAction(Qt::DropAction::MoveAction);
            evt->acceptProposedAction();
            return;
        }
    }
    
    evt->acceptProposedAction();
    evt->setDropAction(Qt::DropAction::IgnoreAction);
}

bool Repeater::CanDoDrop(std::vector<IMItemID>& IMItemIDs, IMItemID whom, DropType nature)
{
    for (auto item : IMItemIDs)
    {
        if (item.first == whom.first && item.second == whom.second)
        {
            //??DoUpdate();
            return false;
        }
    }
    bool nope = false;
    switch (nature)
    {
    case DT_ONTO: {
        for (auto item : IMItemIDs)
            if (!CanDropOn(item, whom))
                nope = true;
        } break;
    case DT_BEFORE: {
        for (auto item : IMItemIDs)
            if (!CanDropBefore(item, whom))
                nope = true;
        } break;
    case DT_AFTER: {
        for (auto item : IMItemIDs)
            if (!CanDropAfter(item, whom))
                nope = true;
        } break;
    }
    if (nope)
    {
        //DoUpdate();
        return false;
    }

    dropTarget_.reset(new DropTarget(whom, nature));
    //??DoUpdate();
    return true;
}

void Repeater::dropEvent(QDropEvent* evt)
{
    dropTarget_.reset();
    if (evt->mimeData()->objectName().compare(DRAG_DROP_NAME) == 0)
    {
        IMItemID dropOnto;
        QRect itemRect;
        if (ItemAt(dropOnto, evt->pos(), true, &itemRect))
        {
            // Verify that we can do this
            auto draggingItems = Repeater_ExtractMimeIDs(evt->mimeData());
            auto dropNature = DetermineDropType(evt->pos(), itemRect);
            if (CanDoDrop(draggingItems, dropOnto, dropNature))
            {
                switch (dropNature)
                {
                case DT_ONTO: {
                    for (auto item : draggingItems)
                        DoDropOn(item, dropOnto);
                } break;
                case DT_BEFORE: {
                    for (auto item : draggingItems)
                        DoDropBefore(item, dropOnto);
                } break;
                case DT_AFTER: {
                    for (auto item : draggingItems)
                        DoDropAfter(item, dropOnto);
                } break;
                }
            }
        }
    }
    dropTarget_.reset();
    evt->acceptProposedAction();
    //DoUpdate();
}

void Repeater::SaveStates(void* key)
{
    SavedState st;
    st.selection_ = selectedItems_;
    st.expansionState_ = this->states_;
    savedStates_[key] = st;
}

void Repeater::RestoreStates(void* key)
{
    auto found = savedStates_.find(key);
    if (found != savedStates_.end())
    {
        states_ = found->second.expansionState_;
        selectedItems_ = found->second.selection_;
    }
    else
    {
        states_.clear();
        selectedItems_.clear();
    }

    emit SelectionChanged(selectedItems_);
    DoUpdate();
}

void Repeater::DestroyState(void* key)
{
    auto found = savedStates_.find(key);
    if (found != savedStates_.end())
        savedStates_.erase(found);
}

bool Repeater::ItemAt(IMItemID& id, const QPoint& pt, bool wholeRow, QRect* outRect)
{
    // potentially extensive looping, deal with each special case of wholeRow instead of "if" testing in the loop
    if (wholeRow)
    {
        for (auto it : visualIndex_)
        {
            auto rect = rects_[it];
            if (rect.top() < pt.y() && rect.bottom() > pt.y())
            {
                id = it;
                if (outRect)
                    *outRect = rect;
                return true;
            }
        }
    }
    else
    {
        for (auto it : visualIndex_)
        {
            auto rect = rects_[it];
            if (rect.contains(pt))
            {
                id = it;
                if (outRect)
                    *outRect = rect;
                return true;
            }
        }
    }
    return false;
}

void Repeater::DoUpdate()
{
    updateTimer_.start(TIMER_INTERVAL);
    update();
}

void Repeater::SetFilterString(const QString& text)
{
    QString txt = text.trimmed();
    if (text.isEmpty())
    {
        filterText_.clear();
        DoUpdate();
    }
    else if (!text.isEmpty())
        SetFilters(text.split(' '));
}

void Repeater::SetFilters(const QStringList& terms)
{
    filterText_ = terms;
    DoUpdate();
}

void Repeater::ForgetItem(IMItemID id)
{
    auto found = states_.find(id);
    if (found != states_.end())
        states_.erase(found);
    auto otherFound = rects_.find(id);
    if (otherFound != rects_.end())
        rects_.erase(otherFound);
}

void Repeater::SetEmbeddedEditor(QWidget* widget)
{
    if (embeddedEditor_)
    {
        embeddedEditor_->deleteLater();
        embeddedEditor_ = 0x0;
    }
    if (widget)
        widget->installEventFilter(this);
    embeddedEditor_ = widget;
}

bool Repeater::TreeNode(const IMItemID& id, const char* text, QPixmap* icon, bool isLeaf, QColor textColor)
{
    int leftLead = 0;

    QPainter* painter = paintingState_.painter_;

    visualIndex_.push_back(id);

    bool expanded = (IsExpanded(id) || (paintingState_.treeDepth_ == 0 && !rootsCanCollapse_)) || !drawAsTree_;
    expanded = isLeaf ? false : expanded;

    QPoint paintPoint = paintingState_.position_;

    // always account for icon space if drawing as a tree
    const float leadSize = 20;
    if (drawAsTree_ && (rootsCanCollapse_ || paintingState_.treeDepth_ > 0))
    {
        paintPoint.setX(paintPoint.x() + leadSize);
        leftLead += leadSize;
    }

    // If zebra-striped then we draw the zebra stripes first
    if (zebraStripe_ && visualIndex_.size() % 2)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(ZEBRA_STRIPE_COLOR));
        painter->drawRect(0, paintPoint.y(), width(), lineHeight_);
    }

    // Draw selection background color or zebra-striping
    if (IsSelected(id))
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(SELECTION_COLOR)); //TODO: move color to constant somewhere
        painter->drawRect(paintPoint.x(), paintPoint.y(), width(), lineHeight_);
    }

    // draw expand/collapse icon if we have any children, but only if we're not the root item
    if (drawAsTree_ && !isLeaf && (paintingState_.treeDepth_ > 0 || rootsCanCollapse_))
    {
        auto& icon = expanded ? chevronExpanded_ : chevronCollapsed_;
        float iconY = paintingState_.position_.y() + (lineHeight_ / 2.0f) - (9.0f / 2.0f);
        auto newIcon = icon.pixmap(16, 16).scaledToHeight(16);
        painter->drawPixmap(QRect(paintingState_.position_.x(), iconY - 8, 32, 32), newIcon);
        //icon.paint(painter, QRect(paintingState_.position_.x(), iconY, 32, 32));
        PushCommandRect({ id, IMCMD_ToggleExpansion, QRect(paintingState_.position_.x() - 5, paintingState_.position_.y(), 28, lineHeight_) });
    }

    painter->setPen(QPen(textColor, 1));

    int pixmapWidth = icon ? 32 + 4 /*spacing*/ : 0;
    const int itemWidth = CalculateTextWidth(text) + pixmapWidth;
    const int itemRight = paintPoint.x() + itemWidth;

// Actual tree item contents
    // Draw our icon if we have one
    if (icon)
    {
        auto pix = icon->scaledToHeight(24);
        painter->drawPixmap(paintPoint.x(), paintPoint.y()+4, 24, 24, pix);
    }

    // Draw our text
    painter->setBrush(Qt::NoBrush);
    painter->drawText(paintPoint.x() + pixmapWidth, paintPoint.y() + shortHeight_, text);
    paintingState_.maxWidth_ = std::max(paintingState_.maxWidth_, itemRight);

    // Drop indicators
    if (dropTarget_ && dropTarget_->dropNature_ == DT_BEFORE && dropTarget_->dropTarget_ == id)
    {
        // line above us
        painter->setBrush(QBrush(DROP_TARGET_COLOR));
        painter->setPen(QPen(QBrush(DROP_TARGET_COLOR), 2));
        painter->drawLine(paintPoint.x(), paintingState_.position_.y(), itemRight, paintingState_.position_.y());
        painter->drawLine(paintPoint.x(), paintingState_.position_.y(), paintPoint.x() - 5, paintingState_.position_.y() + 4);
        painter->drawLine(itemRight, paintingState_.position_.y(), itemRight + 5, paintingState_.position_.y() + 4);
    }
    else if (dropTarget_ && dropTarget_->dropNature_ == DT_ONTO && dropTarget_->dropTarget_ == id)
    {
        // rect ontop of us
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QBrush(DROP_TARGET_COLOR), 2));
        painter->drawRoundRect(paintPoint.x(), paintPoint.y(), itemWidth, lineHeight_, 15, 15);
    }

    // Store our rect and move the position downward
    int bonus = 0;
    rects_[id] = QRect(paintPoint, QSize(itemWidth, lineHeight_));
    paintingState_.position_.setY(paintingState_.position_.y() + lineHeight_);

    // Line seperators on items
    if (drawHorizontalLines_)
    {
        painter->setBrush(QBrush(SEPERATOR_COLOR));
        painter->setPen(QPen(QBrush(SEPERATOR_COLOR), 1));
        painter->drawLine(0, paintingState_.position_.y(), INT_MAX, paintingState_.position_.y());
        paintingState_.position_.setY(paintingState_.position_.y() + 1);
    }

    // Drop indicator for drop after us
    if (dropTarget_ && dropTarget_->dropNature_ == DT_AFTER && dropTarget_->dropTarget_ == id)
    {
        painter->setBrush(QBrush(DROP_TARGET_COLOR));
        painter->setPen(QPen(QBrush(DROP_TARGET_COLOR), 2));
        const int yCoord = paintingState_.position_.y() - (drawHorizontalLines_ ? 1 : 0);
        painter->drawLine(paintPoint.x(), yCoord, itemRight, yCoord);
        painter->drawLine(paintPoint.x(), yCoord, paintPoint.x() - 5, yCoord - 4);
        painter->drawLine(itemRight, yCoord, itemRight + 5, yCoord - 4);
    }

    if (expanded)
    {
        // move indent forward
        paintingState_.position_ += QPoint(GetIndentSize(), 0);
        paintingState_.treeDepth_ += 1;
    }

    return expanded;
}

void Repeater::TreePop()
{
    // When drawing as a tree it's necessary to do this,
    // don't need to account for roots because roots begin at "0" (fake zero at least)
    float leadSize = 20;
    if (!drawAsTree_)
        leadSize = 0;

    paintingState_.treeDepth_ -= 1;
    // back off from our indent
    paintingState_.position_ -= QPoint(GetIndentSize(), 0);
}

bool Repeater::HeaderButton(const IMItemID& id, int index, QPixmap* pixmap, const char* tipText)
{
    QPoint paintPoint(0 + pixmap->width() * index, paintingState_.position_.y());
    QRect buttonRect(paintPoint, QSize(pixmap->width(), pixmap->height()));
    bool containsClick = false;
    if (buttonRect.contains(paintingState_.mousePosition_))
        containsClick = true;
    paintingState_.painter_->drawPixmap(buttonRect, *pixmap);

    PushCommandRect({ id, index, buttonRect });
    return containsClick;
}