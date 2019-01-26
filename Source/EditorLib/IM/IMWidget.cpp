#include "IMWidget.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QImage>
#include <QItemDelegate>
#include <QListWidget>
#include <QPainter>
#include <QTooltip>

#include <qevent.h>

#include <ctype.h>

// have to undefine slots, no tail/prefix naming in nuklear so macro collision - just fix in nuklear later
#include <EditorLib/ThirdParty/Nuklear/nuklear_header.h>

Qt::CursorShape TranslateNKCursor(enum nk_style_cursor curse)
{
    switch (curse)
    {
    case NK_CURSOR_TEXT:
        return Qt::CursorShape::IBeamCursor;
    case NK_CURSOR_RESIZE_HORIZONTAL:
        return Qt::CursorShape::SizeHorCursor;
    case NK_CURSOR_RESIZE_VERTICAL:
        return Qt::CursorShape::SizeVerCursor;
    case NK_CURSOR_RESIZE_TOP_LEFT_DOWN_RIGHT:
        return Qt::CursorShape::SizeAllCursor;
    case NK_CURSOR_MOVE:
        return Qt::CursorShape::DragMoveCursor;
    }
    return Qt::CursorShape::ArrowCursor;
}

IMWidget::IMWidget(int updateRate, QWidget* parent) :
    QWidget(parent),
    matchSizes_(true),
    font_("Arial", 10), // cruft
    metrics_(QFont("Arial", 10)) // cruft
{
    //setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setMouseTracking(true);
    nkFont_ = new nk_user_font;
    nkFont_->userdata = nk_handle_ptr(this);// &metrics_);
    nkFont_->height = (metrics_.height()); //baseline to cap-height
    nkFont_->width = nkGetTextWidth;
    
    for (int i = 0; i < 256; ++i)
        precomputedCharacterWidths_[i] = metrics_.width(i);

    context_ = new nk_context;
    nk_init_default(context_, nkFont_);
    context_->clip.userdata = nk_handle_ptr(0x0);
    context_->clip.copy = nkCopy;
    context_->clip.paste = nkPaste;
    context_->style.window.fixed_background = nk_style_item_hide();
    context_->hotspotHandle = nk_handle_ptr(this);
    context_->hotspotFunction = nkHotspot;
    
    // prepare the auto-update timer
    connect(&repaintTimer_, &QTimer::timeout, [=]() { update(); });
    repaintTimer_.setSingleShot(false);
    if (updateRate > 0)
        repaintTimer_.start(updateRate);

    QAction* cop = 0x0, *pst = 0x0, *cut = 0x0;
    addAction(cop = new QAction("Copy"));
    addAction(pst = new QAction("Paste"));
    addAction(cut = new QAction("Cut"));
    cop->setShortcut(QKeySequence::Copy);
    cut->setShortcut(QKeySequence::Cut);
    pst->setShortcut(QKeySequence::Paste);
}

IMWidget::IMWidget(GUI_CALLBACK callback, int updateRate, QWidget* parent) :
    IMWidget(updateRate, parent)
{
    guiCallback_ = callback;
}

IMWidget::~IMWidget()
{
    FlushComboCache();
    delete nkFont_;
    delete context_;
}

void IMWidget::SetAutoUpdate(int ms)
{
    if (ms == 0)
        repaintTimer_.stop();
    else
        repaintTimer_.start(ms);
}

void IMWidget::cut()
{
    PushRecord({ 0, 0, NK_KEY_CUT, true });
    setFocus(Qt::FocusReason::MouseFocusReason);
    update();
}

void IMWidget::copy()
{
    PushRecord({ 0, 0, NK_KEY_COPY, true });
    setFocus(Qt::FocusReason::MouseFocusReason);
    update();
}

void IMWidget::paste()
{
    PushRecord({ 0, 0, NK_KEY_PASTE, true });
    setFocus(Qt::FocusReason::MouseFocusReason);
    update();
}

void IMWidget::undo()
{
    PushRecord({ 0, 0, NK_KEY_TEXT_UNDO, true });
    setFocus(Qt::FocusReason::MouseFocusReason);
    update();
}

void IMWidget::redo()
{
    PushRecord({ 0, 0, NK_KEY_TEXT_REDO, true });
    setFocus(Qt::FocusReason::MouseFocusReason);
    update();
}

void IMWidget::BuildStyles(nk_context* ctx)
{
    ctx->style.window.spacing.x = 8;
    ctx->style.window.spacing.y = 8;
    //ctx->style.text.color.r = ctx->style.text.color.g = ctx->style.text.color.b = 255;
    //ctx->style.edit.text_normal = { 0,0,0,255 };
    //ctx->style.edit.normal.data.color = {255,255,255,255};
}

bool IMWidget::focusNextPrevChild(bool next)
{
    if (next)
        tabDir_ = 1;
    else
        tabDir_ = -1;
    return true;
}

void IMWidget::paintEvent(QPaintEvent* event)
{
    PrepareComboCache();
    if (!stylesInitialized_)
    {
        BuildStyles(context_);
        stylesInitialized_ = true;
    }
    hotSpots_.clear();

    int dpiX = logicalDpiX();
    int dpiY = logicalDpiY();
    int pDpiX = physicalDpiX();
    int pDpiY = physicalDpiY();
    //int width = widthMM();
    //int height = heightMM();

    QPainter painter;
    painter.begin(this);
    //std::auto_ptr<QFont> tFont(new QFont((font_, &painter));
    //painter.setFont(font_);

    nk_input_begin(context_);

    nk_input_key(context_, NK_KEY_CTRL, QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ControlModifier));
    nk_input_key(context_, NK_KEY_SHIFT, QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier));
    bool shiftDown = QApplication::keyboardModifiers().testFlag(Qt::KeyboardModifier::ShiftModifier);

    for (unsigned i = 0; i < keysDown_.size(); ++i)
    {
        if (keysDown_[i].charCode_ != 0 && keysDown_[i].state_)
        {
            if (isalpha(keysDown_[i].charCode_))
            {
                if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ShiftModifier)
                    nk_input_char(context_, toupper(keysDown_[i].charCode_));
                else
                    nk_input_char(context_, tolower(keysDown_[i].charCode_));
            }
            else
                nk_input_char(context_, keysDown_[i].charCode_);

            keysDown_[i].state_ = false;
        }
        else if (keysDown_[i].nkKey_)
        {
            if (keysDown_[i].nkKey_ != NK_KEY_NONE)
            {
                nk_input_key(context_, keysDown_[i].nkKey_, keysDown_[i].state_);
            }
            keysDown_[i].state_ = false;
        }
    }

    context_->delta_time_seconds = GetDelta();
    QPoint localMousePos = mapFromGlobal(QCursor::pos());
    nk_input_button(context_, nk_buttons::NK_BUTTON_LEFT, localMousePos.x(), localMousePos.y(), QApplication::mouseButtons().testFlag(Qt::LeftButton));
    nk_input_button(context_, nk_buttons::NK_BUTTON_RIGHT, localMousePos.x(), localMousePos.y(), QApplication::mouseButtons().testFlag(Qt::RightButton));
    nk_input_button(context_, nk_buttons::NK_BUTTON_MIDDLE, localMousePos.x(), localMousePos.y(), QApplication::mouseButtons().testFlag(Qt::MiddleButton));
    nk_input_motion(context_, localMousePos.x(), localMousePos.y());// mouseDelta_.x(), mouseDelta_.y());
    nk_input_scroll(context_, nk_vec2(0, lastWheelDelta_));

    nk_input_end(context_);

    // whipe out all of them
    for (int i = 0; i < keysDown_.size(); ++i)
        if (keysDown_[i].nkKey_)
            nk_input_key(context_, keysDown_[i].nkKey_, false);
    context_->input.mouse.scroll_delta.y = 0;

    context_->cursor_style = NK_CURSOR_ARROW;

    nk_begin(context_, "Demo", nk_rect(0, 0, width(), height()), NK_WINDOW_NO_SCROLLBAR);
    GenerateUI(context_, &painter);

    if (matchSizes_)
    {
        struct nk_rect nuklearSize = nk_widget_bounds(context_);
        if ((int)(nuklearSize.y) != minimumHeight())
            setMinimumHeight(nuklearSize.y);
    }

    nk_end(context_);

    //setCursor(QCursor(TranslateNKCursor(context_->cursor_style)));

    Render(&painter);
    lastWheelDelta_ = 0;

    painter.end();

    CleanComboCache();
}

void IMWidget::wheelEvent(QWheelEvent* event)
{
    QWidget::wheelEvent(event);
    lastWheelDelta_ = event->delta();
    update();
}

void IMWidget::mouseMoveEvent(QMouseEvent* event)
{
    QWidget::mouseMoveEvent(event);
    event->accept();

    bool hit = false;
    bool hasTip = false;
    for (auto hs : hotSpots_)
    {
        if (hs.rect_.contains(event->pos()))
        {
            setCursor(QCursor(hs.cursor_));
            if (hs.repaint_)
                update();
            hit = true;

            // If we have a tooltip then use it
            if (hasTip = hs.tip_)
                QToolTip::showText(mapToGlobal(event->pos()), hs.tip_);
        }
    }
    if (!hit)
        setCursor(QCursor(Qt::CursorShape::ArrowCursor));

    // Clear any existing tooltip so that they don't "stick" around
    if (!hasTip)
        QToolTip::hideText();
}

void IMWidget::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
    event->accept();
    KeyRecord rec = MakeKeyRecord(event, true);
    //auto found = std::find_if(keysDown_.begin(), keysDown_.end(), [&rec](const KeyRecord& rhs) { return rhs.qKey_ == rec.qKey_ && rhs.nkKey_ == rec.nkKey_; });
    rec.state_ = true;
    PushRecord(rec);
    //if (found == keysDown_.end())
    //    keysDown_.push_back(rec);
    //else
    //    found->state_ = true;
    update();
}

void IMWidget::keyReleaseEvent(QKeyEvent* event)
{
    QWidget::keyReleaseEvent(event);
    event->accept();
    KeyRecord rec = MakeKeyRecord(event, false);
    //auto found = std::find_if(keysDown_.begin(), keysDown_.end(), [&rec](const KeyRecord& rhs) { return rhs.qKey_ == rec.qKey_ && rhs.nkKey_ == rec.nkKey_; });
    //if (found != keysDown_.end())
    //{
    //    switch (rec.nkKey_)
    //    {
    //    //case NK_KEY_PASTE:
    //    //case NK_KEY_COPY:
    //    //case NK_KEY_CUT:
    //    //    found->state_ = true;
    //    //    break;
    //    default:
    //        found->state_ = false;
    //    }
    //}
    //else
    //{
        switch (rec.nkKey_)
        {
        case NK_KEY_PASTE:
        case NK_KEY_COPY:
        case NK_KEY_CUT:
        case NK_KEY_TAB:
            rec.state_ = true;
            break;
        default:
            rec.state_ = false;
        }
        PushRecord(rec);
    //    keysDown_.push_back(rec);
    //}
    update();
}

void IMWidget::PushRecord(const KeyRecord& rec)
{
    auto found = std::find_if(keysDown_.begin(), keysDown_.end(), [&rec](const KeyRecord& rhs) { return rhs.qKey_ == rec.qKey_ && rhs.nkKey_ == rec.nkKey_; });
    if (found != keysDown_.end())
        found->state_ = rec.state_;
    else
        keysDown_.push_back(rec);
}

IMWidget::KeyRecord IMWidget::MakeKeyRecord(QKeyEvent* event, bool isDown)
{
    static QString allowedChars = " ()[]{}.,<>?'\"\\/!@#$%^&*()-_=+|:;`~";

    bool ctrlDown = event->modifiers().testFlag(Qt::KeyboardModifier::ControlModifier);
    bool shiftDown = event->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier);

    switch (event->key())
    {
    case Qt::Key_Shift:
    case Qt::Key_CapsLock:
        return  { 0, 0, NK_KEY_NONE };
    default:
        break;
    }

    if (event->key() > 0 && event->key() < 255 && (isalpha(event->key()) || allowedChars.contains((QChar)event->key())))
    {
        if (ctrlDown)
        {
            if (event->key() == 'V')
                return{ 0, 0, NK_KEY_PASTE };
            else if (event->key() == 'C')
                return{ 0, 0, NK_KEY_COPY };
            else if (event->key() == 'X')
                return{ event->key(), 0, NK_KEY_CUT };
            else if (event->key() == 'A')
                return{ 0, 0, NK_KEY_TEXT_SELECT_ALL };
        }

        return{ event->key(), (char)event->key(), nk_keys::NK_KEY_NONE };
    }
    else if (event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9)
        return{ event->key(), (char)event->key(), nk_keys::NK_KEY_NONE };
    else if (event->key() == Qt::Key_Period)
        return{ event->key(), (char)event->key(), nk_keys::NK_KEY_NONE };
    else if (event->key() == Qt::Key_Left)
    {
        if (ctrlDown)
            return{ 0, (char)0, NK_KEY_TEXT_WORD_LEFT };
        else
            return{ 0, (char)0, NK_KEY_LEFT };
    }
    else if (event->key() == Qt::Key_Right)
    {
        if (ctrlDown)
            return{ event->key(), (char)0, NK_KEY_TEXT_WORD_RIGHT };
        else
            return{ event->key(), (char)0, NK_KEY_RIGHT };
    }
    else if (event->key() == Qt::Key_Up)
        return { 0, (char)0, NK_KEY_UP };
    else if (event->key() == Qt::Key_Down)
        return{ 0, (char)0, NK_KEY_DOWN };
    else if (event->key() == Qt::Key_Delete)
        return{ 0, (char)0, NK_KEY_DEL };
    else if (event->key() == Qt::Key_Backspace)
        return{ 0, (char)0, NK_KEY_BACKSPACE};
    else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
        return{ 0, (char)0, NK_KEY_ENTER };
    else if (event->key() == Qt::Key_PageUp)
        return { 0, (char)0, NK_KEY_SCROLL_UP};
    else if (event->key() == Qt::Key_PageDown)
        return { 0, (char)0, NK_KEY_SCROLL_DOWN };
    else if (event->key() == Qt::Key_Home)
        if (ctrlDown)
            return{ 0, (char)0, NK_KEY_TEXT_START };
        else
            return { 0, (char)0, NK_KEY_TEXT_LINE_START };
    else if (event->key() == Qt::Key_End)
        if (ctrlDown)
            return{ 0, (char)0, NK_KEY_TEXT_END };
        else
            return { 0, (char)0, NK_KEY_TEXT_LINE_END };
    else if (event->key() == Qt::Key_Tab)
            return { 0, (char)0, NK_KEY_TAB};
    else if (event->key() == Qt::Key_Insert)
        return{ 0, (char)0, NK_KEY_TEXT_INSERT_MODE };
    //else if (event->key() == Qt::Key_Control)
    //    return{ event->key(), (char)0, NK_KEY_CTRL };
    return { 0, 0, NK_KEY_NONE };
}

float IMWidget::GetDelta()
{
    float lastTime = timer_.elapsed() / 1000.01f;
    timer_.restart();
    return lastTime;
}

void IMWidget::GenerateUI(nk_context* ctx, QPainter* painter)
{
    if (guiCallback_)
    {
        guiCallback_(this, ctx, painter);
        return;
    }

    // Confirmation code: if this functions then everything 'should' be right
    enum { EASY, HARD };
    static int op = EASY;
    static int property = 20;
    static float fProp = 1.0f;
    static QString testString = "Roger";

    nk_layout_row_dynamic(ctx, 30, 1);
    if (nk_button_label(ctx, "Run Button"))
        fprintf(stdout, "button pressed\n");
    nk_layout_row_dynamic(ctx, 30, 2);
    if (nk_option_label(ctx, "Easy", op == EASY)) 
        op = EASY;
    if (nk_option_label(ctx, "Hard", op == HARD)) 
        op = HARD;
    nk_layout_row_dynamic(ctx, 22, 1);
    nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1, NK_PROP_COLOR_PERCENT, { 255,255,255,255 });
    nk_label(context_, "Have a Label", NK_TEXT_ALIGN_LEFT);

    if (nk_tree_push(ctx, NK_TREE_TAB, "Test Group", NK_MINIMIZED))
    {
        nk_label(context_, "My Label", NK_TEXT_ALIGN_LEFT);
        nk_property_float(ctx, "Delta", 0.0f, &fProp, 100.0f, 0.25f, 1.0f, NK_PROP_COLOR_PERCENT, { 255,255,255,255 });
        nk_layout_row_dynamic(ctx, 70, 1);            
        nk_tree_pop(context_);
    }

    nk_layout_row_dynamic(ctx, 90, 2);
    nk_label(ctx, "Label", NK_TEXT_ALIGN_LEFT);
        
    if (nk_group_begin(ctx, "Subgroup", NK_WINDOW_NO_SCROLLBAR))
    {
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "Item A", NK_TEXT_ALIGN_LEFT);
        nk_label(ctx, "Item C", NK_TEXT_ALIGN_LEFT);
        nk_label(ctx, "Item B", NK_TEXT_ALIGN_LEFT);
        nk_group_end(ctx);
    }
}

static void nuklear_interpolate_color(struct nk_color c1, struct nk_color c2, struct nk_color *result, float fraction) {
    float r = c1.r + (c2.r - c1.r) * fraction;
    float g = c1.g + (c2.g - c1.g) * fraction;
    float b = c1.b + (c2.b - c1.b) * fraction;
    float a = c1.a + (c2.a - c1.a) * fraction;
    result->r = (nk_byte)NK_CLAMP(0, r, 255);
    result->g = (nk_byte)NK_CLAMP(0, g, 255);
    result->b = (nk_byte)NK_CLAMP(0, b, 255);
    result->a = (nk_byte)NK_CLAMP(0, a, 255);
}

void IMWidget::Render(QPainter* painter)
{
    int cmdsProcessed = 0;
    // reset sequence;
    tabSequence_ = 0;
    tabDir_ = 0;

#define GET_COLOR(SRC) QColor(SRC->color.r, SRC->color.g, SRC->color.b, SRC->color.a)
    const nk_command* cmd = 0x0;
    nk_foreach(cmd, context_)
    {
        ++cmdsProcessed;
        switch (cmd->type)
        {
        case NK_COMMAND_NOP: break;
        case NK_COMMAND_SCISSOR: {
            const nk_command_scissor* s = (const nk_command_scissor*)cmd;
            painter->setClipRect(s->x, s->y, s->w, s->h, Qt::ReplaceClip);
            painter->setClipping(true);
        } break;
        case NK_COMMAND_LINE: {
            const nk_command_line* l = (const nk_command_line*)cmd;
            painter->setPen(QPen(QColor(l->color.r, l->color.g, l->color.b, l->color.a), l->line_thickness));
            painter->setBrush(Qt::NoBrush);
            painter->drawLine(l->begin.x, l->begin.y, l->end.x, l->end.y);
        } break;
        case NK_COMMAND_RECT: {
            const nk_command_rect *r = (const nk_command_rect *)cmd;
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(QColor(r->color.r, r->color.g, r->color.b, r->color.a), r->line_thickness));
            if (r->rounding)
                painter->drawRoundedRect(r->x, r->y, r->w, r->h, r->rounding, r->rounding);
            else
                painter->drawRect(r->x, r->y, r->w, r->h);
        } break;
        case NK_COMMAND_RECT_FILLED: {
            const nk_command_rect_filled* r = (const nk_command_rect_filled*)cmd;
            painter->setBrush(QColor(r->color.r, r->color.g, r->color.b, r->color.a));
            painter->setPen(Qt::NoPen);
            if (r->rounding)
                painter->drawRoundedRect(r->x, r->y, r->w, r->h, r->rounding, r->rounding);
            else
                painter->drawRect(r->x, r->y, r->w, r->h);
        } break;
        case NK_COMMAND_CIRCLE: {
            const nk_command_circle* c = (const nk_command_circle *)cmd;
            painter->setBrush(Qt::NoBrush);
            painter->setPen(QPen(QColor(c->color.r, c->color.g, c->color.b, c->color.a), c->line_thickness));
            painter->drawEllipse(c->x, c->y, c->w, c->h);
        } break;
        case NK_COMMAND_CIRCLE_FILLED: {
            const nk_command_circle_filled* c = (const nk_command_circle_filled *)cmd;
            painter->setBrush(QColor(c->color.r, c->color.g, c->color.b, c->color.a));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(c->x, c->y, c->w, c->h);
        } break;
        case NK_COMMAND_TRIANGLE: {
            const struct nk_command_triangle*t = (const struct nk_command_triangle*)cmd;
            painter->setPen(QPen(GET_COLOR(t), t->line_thickness));
            painter->setBrush(Qt::NoBrush);
            std::vector<QPoint> pts = { QPoint(t->a.x, t->a.y), QPoint(t->b.x, t->b.y), QPoint(t->c.x, t->c.y) };
            painter->drawPolygon(pts.data(), pts.size());
        } break;
        case NK_COMMAND_TRIANGLE_FILLED: {
            const struct nk_command_triangle_filled* t = (const struct nk_command_triangle_filled*)cmd;
            painter->setPen(Qt::NoPen);
            painter->setBrush(GET_COLOR(t));
            std::vector<QPoint> pts = { QPoint(t->a.x, t->a.y), QPoint(t->b.x, t->b.y), QPoint(t->c.x, t->c.y) };
            painter->drawPolygon(pts.data(), pts.size());
        } break;
        case NK_COMMAND_POLYGON: {
            const struct nk_command_polygon *p = (const struct nk_command_polygon*)cmd;
            painter->setPen(QPen(GET_COLOR(p), p->line_thickness));
            painter->setBrush(Qt::NoBrush);
            std::vector<QPoint> pts(p->point_count);
            for (unsigned idx = 0; idx < p->point_count; ++idx)
                pts[idx] = QPoint(p->points[idx].x, p->points[idx].y);
            painter->drawPolygon(pts.data(), pts.size());
        } break;
        case NK_COMMAND_POLYGON_FILLED: {
            const struct nk_command_polygon_filled*p = (const struct nk_command_polygon_filled*)cmd;
            painter->setPen(Qt::NoPen);
            painter->setBrush(GET_COLOR(p));
            std::vector<QPoint> pts(p->point_count);
            for (unsigned idx = 0; idx < p->point_count; ++idx)
                pts[idx] = QPoint(p->points[idx].x, p->points[idx].y);
            painter->drawPolygon(pts.data(), pts.size());
        } break;
        case NK_COMMAND_POLYLINE: {
            const struct nk_command_polyline *p = (const struct nk_command_polyline *)cmd;
            painter->setPen(QPen(GET_COLOR(p), p->line_thickness));
            painter->setBrush(Qt::NoBrush);
            std::vector<QPoint> pts(p->point_count);
            for (unsigned idx = 0; idx < p->point_count; ++idx)
                pts[idx] = QPoint(p->points[idx].x, p->points[idx].y);
            painter->drawPolyline(pts.data(), pts.size());
        } break;
        case NK_COMMAND_TEXT: {
            const nk_command_text* t = (const struct nk_command_text*)cmd;
            painter->setBrush(Qt::NoBrush);// QColor(t->background.r, t->background.g, t->background.b, t->background.a));
            painter->setPen(QPen(QColor(t->foreground.r, t->foreground.g, t->foreground.b, 255), 1.0));
            QString txt = (const char*)t->string;
            txt.chop(txt.length() - t->length);
            painter->drawText(t->x, t->y, t->w + metrics_.averageCharWidth(), INT_MAX, 0, txt);
            //DEBUG code:
            //painter->setPen(QPen(Qt::red, 1.0));
            //painter->drawRect(t->x, t->y, t->w + metrics_.averageCharWidth(), t->h);
        } break;
        case NK_COMMAND_CURVE: {
            const struct nk_command_curve *q = (const struct nk_command_curve *)cmd;
            painter->setPen(QPen(GET_COLOR(q), q->line_thickness));
            painter->setBrush(Qt::NoBrush);
            QPainterPath path(QPoint(q->begin.x, q->begin.y));
            path.cubicTo(q->ctrl[0].x, q->ctrl[0].y, q->ctrl[1].x, q->ctrl[1].y, q->end.x, q->end.y);
            painter->drawPath(path);
        } break;
        case NK_COMMAND_RECT_MULTI_COLOR: {
            const struct nk_command_rect_multi_color* r = (const nk_command_rect_multi_color*)cmd;
            painter->setPen(Qt::NoPen);

            QImage img = QImage(r->w, r->h, QImage::Format_RGB32);
            struct nk_color X1, X2, Y;
            for (int y = 0; y < r->h; ++y)
            {
                float fraction_y = ((float)y) / r->h;
                for (int x = 0; x < r->w; ++x)
                {
                    float fraction_x = ((float)x) / r->w;
                    nuklear_interpolate_color(r->left, r->top, &X1, fraction_x);
                    nuklear_interpolate_color(r->right, r->bottom, &X2, fraction_x);
                    nuklear_interpolate_color(X1, X2, &Y, fraction_y);
                    img.setPixelColor(x, y, QColor(Y.r, Y.g, Y.b, Y.a));
                }
            }
            painter->drawImage(r->x, r->y, img);
        } break;
        case NK_COMMAND_IMAGE: {
            const nk_command_image* i = (const nk_command_image*)cmd;
            if (void* data = i->img.handle.ptr)
            {
                auto iconFound = iconCache_.find((const char*)data);
                if (iconFound != iconCache_.end())
                    painter->drawPixmap(QPoint(i->x, i->y), iconFound->second.pixmap(QSize(i->w, i->h)));
                else {
                    auto imageFound = imageCache_.find((const char*)data);
                    if (imageFound != imageCache_.end())
                        painter->drawImage(QPoint(i->x, i->y), imageFound->second);
                }
            }
        } break;
        case NK_COMMAND_ARC: {
            const nk_command_arc* a = (const nk_command_arc*)cmd;
            painter->setPen(QPen(GET_COLOR(a), a->line_thickness));
            painter->setBrush(Qt::NoBrush);
            painter->drawArc(a->cx - a->r, a->cy - a->r, a->cx + a->r, a->cy + a->r, a->a[0], a->a[1]);
        } break;
        case NK_COMMAND_ARC_FILLED: {
            const nk_command_arc_filled* a = (const nk_command_arc_filled*)cmd;
            painter->setPen(Qt::NoPen);
            painter->setBrush(GET_COLOR(a));
            painter->drawArc(a->cx - a->r, a->cy - a->r, a->cx + a->r, a->cy + a->r, a->a[0], a->a[1]);
        } break;
        default: break;
        }
    }

    nk_clear(context_);
}

float IMWidget::nkGetTextWidth(nk_handle handle, float height, const char *text, int len)
{
    int width = 0;
    for (int i = 0; i < len; ++i)
        width += ((IMWidget*)handle.ptr)->precomputedCharacterWidths_[text[i]];
    return width;
    //WARNING: using QFontMetrics for this was incredibly slow and resulted in Nuklear using 80% of application CPU time
    //Code left in place as a minefield-marker
    //return ((QFontMetrics*)handle.ptr)->averageCharWidth() * len;// width(text, len);
}

void IMWidget::nkPaste(nk_handle, nk_text_edit* editor)
{
    QString text = QApplication::clipboard()->text();
    auto data = text.toUtf8();
    nk_textedit_paste(editor, data.constData(), data.length());
}

void IMWidget::nkCopy(nk_handle, const char* text, int len)
{
    QString txt(text);
    txt.chop(txt.length() - len);
    QApplication::clipboard()->setText(txt);
}

void IMWidget::nkHotspot(nk_handle handle, struct nk_hotspot hs)
{
    IMWidget* widget = ((IMWidget*)handle.ptr);
    HotSpot hot;
    hot.rect_ = QRect(hs.rect.x, hs.rect.y, hs.rect.w, hs.rect.h);
    hot.cursor_ = TranslateNKCursor(hs.cursor_style);
    hot.repaint_ = hs.flags;
    widget->hotSpots_.push_back(hot);
}

void IMWidget::ProcessShiftTab()
{
    if (tabDir_ != -1)
        return;
    auto win = context_->current;
    if (tabSequence_ == (lastActiveWidgetTabIndex_ - 1) || (tabSequence_ == 0 && lastActiveWidgetTabIndex_ == 1))
    {
        //TODO: investigate why Nuklear's sequences are off by one?
        nk_edit_focus(context_, NK_EDIT_ALWAYS_INSERT_MODE);
        // Compensate for Nuklear's bad sequencing
        win->edit.name = std::max<unsigned>(0, win->edit.seq - 1);
        tabDir_ = 0;
    }
}

void IMWidget::ProcessTab()
{
    if (tabDir_ != 1)
        return;
    auto win = context_->current;

    if (tabSequence_ == lastActiveWidgetTabIndex_)
    {
        nk_edit_focus(context_, NK_EDIT_ALWAYS_INSERT_MODE);
        tabDir_ = 0;
    }
}

bool IMWidget::EditString(const char* name, std::string& txt)
{
    static char data[1024];
    memset(&data[0], 0, sizeof(char) * 1024);
    strcpy(data, txt.c_str());
    int len = txt.length();

    if (nk_edit_string_zero_terminated(context_, NK_EDIT_FIELD, data, 1023, 0) == 1)
        lastActiveWidgetTabIndex_ = tabSequence_;
    
    ProcessShiftTab();
    ProcessTab();
    ++tabSequence_;

    if (strcmp(data, txt.c_str()) != 0)
    {
        txt = data;
        return true;
    }
    return false;
}

bool IMWidget::EditText(const char* name, std::string& txt)
{
    static char data[4096*8];
    memset(&data[0], 0, sizeof(char) * 4096*8);
    strcpy(data, txt.c_str());
    int len = txt.length();
    nk_edit_string_zero_terminated(context_, NK_EDIT_BOX, data, 4096*8-1, 0);
    if (memcmp(data, txt.c_str(), txt.length()) != 0)
    {
        txt = data;
        return true;
    }
    return false;
}
bool IMWidget::EditInt(const char* name, int& value, int min, int max, int increment)
{
    int ref = value;
    nk_property_int(context_, name, min, &ref, max, increment, increment * 0.1f, NK_PROP_COLOR_OFF, { 255,255,255,255 });
    if (ref != value)
    {
        value = ref;
        return true;
    }
    return false;
}

bool IMWidget::EditInt(const char* name, int& value, int min, int max, int increment, nk_color col)
{
    int ref = value;
    nk_property_int(context_, name, min, &ref, max, increment, increment * 0.1f, NK_PROP_COLOR_FULL, col);
    if (ref != value)
    {
        value = ref;
        return true;
    }
    return false;
}

bool IMWidget::EditFloat(const char* name, float& value, float min, float max, float increment)
{
    float ref = value;
    nk_property_float(context_, name, min, &ref, max, increment, increment * 0.1f, NK_PROP_COLOR_OFF, { 255,255,255,255 });
    if (ref != value)
    {
        value = ref;
        return true;
    }
    return false;
}

bool IMWidget::EditFloat(const char* name, float& value, float min, float max, float increment, nk_color col)
{
    float ref = value;
    
    // check for forward tab
    int forceOn = 0;
    if (tabSequence_ == lastActiveWidgetTabIndex_ + 1 && tabDir_ == 1)
    {
        forceOn = 1;
        tabDir_ = 0;
    }
    else if (tabSequence_ == lastActiveWidgetTabIndex_ - 1 && tabDir_ == -1)
    {
        forceOn = 1;
        tabDir_ = 0;
    }

    if (nk_property_float(context_, name, min, &ref, max, increment, increment * 0.1f, NK_PROP_COLOR_FULL, col, forceOn))
        lastActiveWidgetTabIndex_ = tabSequence_;
    
    ++tabSequence_;

    if (ref != value)
    {
        value = ref;
        return true;
    }
    return false;
}

bool IMWidget::EditByte(const char* name, unsigned char& value)
{
    int ref = value;
    nk_property_int(context_, name, 0, &ref, 255, 1, 1 * 0.1f, NK_PROP_COLOR_OFF, { 255,255,255,255 });
    if (ref != value)
    {
        value = ref;
        return true;
    }
    return false;
}

bool IMWidget::EditBool(const char* name, bool& value)
{
    int ref = value;
    nk_checkbox_label(context_, name, &ref);
    if (ref != value)
    {
        value = ref;
        return true;
    }
    return false;
}

bool IMWidget::ButtonText(const char* name)
{
    return nk_button_label(context_, name) != 0;
}

bool IMWidget::ButtonText(const char* name, const char* tip)
{
    PushTooltip(tip);
    return nk_button_label(context_, name) != 0;
}

bool IMWidget::ButtonImage(const char* name, QIcon icon)
{
    struct nk_image img;
    img.handle.ptr = (void*)name;
    auto iconSize = icon.actualSize(QSize(INT_MAX, INT_MAX));
    img.w = iconSize.width();
    img.h = iconSize.height();
    iconCache_[name] = icon;
    return nk_button_image(context_, img);
}

bool IMWidget::ButtonImage(const char* name, const char* tip, QIcon icon)
{
    PushTooltip(tip);
    return ButtonImage(name, icon);
}

bool IMWidget::ButtonImage(const char* name, QImage image)
{
    struct nk_image img;
    img.handle.ptr = (void*)name;
    img.w = image.width();
    img.h = image.height();
    imageCache_[name] = image;
    return nk_button_image(context_, img);
}

bool IMWidget::ButtonImage(const char* name, const char* tip, QImage image)
{
    PushTooltip(tip);
    return ButtonImage(name, image);
}

bool IMWidget::EditBitfield(const char* name, unsigned& bits)
{
    nk_layout_row_template_begin(context_, 18);
    for (unsigned i = 0; i < 17; ++i)
    {
        if (i == 8)
            nk_layout_row_template_push_static(context_, 4);
        else
            nk_layout_row_template_push_static(context_, 18);
    }
    nk_layout_row_template_end(context_);

    unsigned refVal = bits;
    for (unsigned i = 0; i < 32; ++i)
    {
        if (i == 8 || i == 24)
            EmptySpace();
        nk_checkbox_flags_label(context_, "", &bits, 1 << (i));
    }
    return refVal != bits;;
}

bool IMWidget::ComboBox(const char* name, const char** items, int& selectedIndex)
{
    auto record = GetComboBoxRecord(name);
    if (record->combo_->count() == 0)
    {
        record->combo_->blockSignals(true);
        for (int i = 0;; ++i)
        {
            if (items[i] == 0x0)
                break;
            record->combo_->addItem(items[i]);
        }
        record->combo_->setCurrentIndex(selectedIndex);
        record->combo_->blockSignals(false);
    }
    
    record->combo_->blockSignals(true);
    int curIndex = record->combo_->currentIndex();
    if (selectedIndex != curIndex && !record->justChanged_)
        record->combo_->setCurrentIndex(selectedIndex);
    record->combo_->blockSignals(false);

    QTWidget(record->combo_);

    if (record->justChanged_)
    {
        selectedIndex = record->index_;
        return true;
    }

    return false;
}

bool IMWidget::ComboBox(const char* name, const char** items, int itemCount, int& selectedIndex)
{
    return false;
}

bool IMWidget::ComboBox(const char* name, const char** items, QIcon** icons, int& selectedIndex)
{
    return false;
}

bool IMWidget::ComboBox(const char* name, const char** items, QIcon** icons, int itemCount, int& selectedIndex)
{
    return false;
}

void IMWidget::EmptySpace()
{
    struct nk_rect r = nk_rect(0, 0, 0, 0);
    nk_widget(&r, context_);
}

void IMWidget::QTWidget(QWidget* widget)
{
    struct nk_rect r = nk_rect(0, 0, 0, 0);
    nk_widget(&r, context_);
    if (!widget->isVisible())
        widget->setVisible(true);

    if (widget->pos().x() != r.x || widget->pos().y() != r.y)
        //widget->setGeometry(r.x, r.y, r.w, r.h);
        widget->move(r.x, r.y);
    if (widget->size().width() != r.w || widget->size().height() != r.h)
        widget->setFixedSize(r.w, r.h);
}

QRect IMWidget::GetNextWidgetArea()
{
    struct nk_rect r = nk_widget_bounds(context_);
    return QRect(r.x, r.y, r.w, r.h);
}

void IMWidget::PushTooltip(const char* tip)
{
    HotSpot hs;
    hs.rect_ = GetNextWidgetArea();
    hs.tip_ = tip;
    hs.repaint_ = false;
    hs.cursor_ = Qt::CursorShape::ArrowCursor;
    hotSpots_.push_back(hs);
}

IMWidget::ComboBoxRecord* IMWidget::GetComboBoxRecord(const char* name)
{
    auto found = comboBoxCache_.find(name);
    if (found != comboBoxCache_.end())
    {
        found->second->used_ = true;
        return found->second;
    }
    ComboBoxRecord* rec = new ComboBoxRecord();
    rec->combo_ = new QComboBox(this);
    rec->used_ = true;
    connect(rec->combo_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int idx) {
        rec->index_ = idx;
        rec->justChanged_ = true;
    });
    comboBoxCache_[name] = rec;
    return rec;
}

void IMWidget::FlushComboCache()
{
    for (auto rec : comboBoxCache_)
    {
        rec.second->combo_->hide();
        rec.second->combo_->deleteLater();
        delete rec.second;
    }
}

void IMWidget::PrepareComboCache()
{
    for (auto rec : comboBoxCache_)
        rec.second->used_ = false;
}

void IMWidget::CleanComboCache()
{
    for (auto rec : comboBoxCache_)
    {
        rec.second->justChanged_ = false;
        if (!rec.second->used_ && rec.second->combo_->isVisible())
            rec.second->combo_->hide();
    }
}