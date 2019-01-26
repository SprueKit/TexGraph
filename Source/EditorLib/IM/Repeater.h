#pragma once

#include <EditorLib/editorlib_global.h>
#include <EditorLib/IM/IMGui.h>
#include <QIcon>
#include <QTimer>
#include <QWidget>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class ScrollAreaWidget;

class QFont;
class QFontMetrics;

/// Repeater is a custom Tree/List widget which behaves similarly to an IMGUI.
/// Paint is responsible for emitting the necessary interaction structures and the widget frequently updates.
/// The model is presumably always ephemeral and may even be purely functional.
/// Trades the "already there" advantanges of QTreeWidget for "just works with my stuff"
/// This approach minimized the "data-model doubling" involved with usinig complex classes with QTreeWidget.
/// Bug: ItemID collisions from reallocations with matching integral IDs
///     it's a fact of life without something of more reliable uniqueness like GUIDs, *should* be rare though       
class EDITORLIB_EXPORT Repeater : public QWidget, public IMUserInterface
{
    Q_OBJECT;
public:
    /// Construct, optionally with a given parent.
    Repeater(QWidget* parent = 0x0);
    /// Destruct.
    virtual ~Repeater();

// Implementation methods
    virtual void GenerateUI() { }

    /// Implement to construct a widget for embedded editing, or return 0x0 for no embedded edit
    virtual QWidget* EmbeddedEdit(QWidget* owner, IMItemID item, QRect area) { return 0x0; }
    /// Display the context menu, return true if displayed.
    virtual bool ContextMenu(const std::vector<IMItemID>& items, const QPoint& pos) { return false; }
    /// Execute the command for a command rect click.
    virtual bool ExecuteCommand(const IMCmdRect& cmdData) { return false; }
    /// Filters will allow excluding certain items from rendering
    virtual bool PassesFilters(IMItemID, const QStringList& filters) { return true; }

    /// Return true if the given item can be dragged
    virtual bool CanDragItem(IMItemID) { return false; }
    virtual bool CanDropOn(IMItemID who, IMItemID onto) { return false; }
    virtual bool CanDropBefore(IMItemID who, IMItemID before) { return false; }
    virtual bool CanDropAfter(IMItemID who, IMItemID after) { return false; }
    // Execute the above operations
    virtual void DoDropOn(IMItemID who, IMItemID onto) { }
    virtual void DoDropBefore(IMItemID who, IMItemID before) { }
    virtual void DoDropAfter(IMItemID who, IMItemID before) { }
    // Called by the delete/backspace keys to perform a delete
    virtual bool DeleteItem(IMItemID) { return false; }

// Usage
/// Returns true if expanded. Colorless-Text only overload
    inline bool TreeNode(const IMItemID& id, const char* text, bool isLeaf) { return TreeNode(id, text, 0x0, isLeaf, defaultTextColor_); }
    /// Returns true if expanded. Text+color only overload
    inline bool TreeNode(const IMItemID& id, const char* text, bool isLeaf, QColor color) { return TreeNode(id, text, 0x0, isLeaf, color); }
    /// Returns true if expanded. Pixmap+text without color only overload
    inline bool TreeNode(const IMItemID& id, const char* text, QPixmap* icon, bool isLeaf) { return TreeNode(id, text, icon, isLeaf, defaultTextColor_); }
    /// Returns true if expanded.
    bool TreeNode(const IMItemID& id, const char* text, QPixmap* icon, bool isLeaf, QColor textColor);
    /// Pops a branch node off.
    void TreePop();
    /// Row-headers must be set before outputting a Tree node/leaf
    bool HeaderButton(const IMItemID& id, int index, QPixmap*, const char* tipText);

// Utility
    /// Wraps the given repeater into a scrollwidget configured that is appropriately configured.
    static ScrollAreaWidget* WrapInStandardScrollArea(Repeater* repeater);

// Functionality
    /// Returns the text color that will be used when color is not specified.
    QColor GetDefaultTextColor() const { return defaultTextColor_; }
    /// Sets the color to use for unspecified colors.
    void SetDefaultTextColor(const QColor& col) { defaultTextColor_ = col; }
    /// Returns the default expansion state for items.
    bool IsDefaultExpanded() const { return defaultExpanded_; }
    /// Sets the default expansion state for items.
    void SetDefaultExpanded(bool state) { defaultExpanded_ = true; }
    /// Returns true if the current mode of drawing is like a tree.
    bool IsDrawAsTree() const { return drawAsTree_; }
    /// Sets whether to draw like a tree (true) or a list (false).
    void SetDrawAsTree(bool state) { drawAsTree_ = state; update(); }
    /// Returns whether root-nodes are allowed to collapse or not.
    bool CanRootCollapse() const { return rootsCanCollapse_; }
    /// Sets whether root nodes are allowed to collapse or not.
    void SetRootCanCollapse(bool state) { rootsCanCollapse_ = state; }
    /// Returns the amount to indent each depth level by.
    int GetIndentSize() { return indentSize_; }
    /// Sets the amount to indent each depth level by.
    void SetIndentSize(int size) { indentSize_ = size; update(); }

    /// Returns true if horizontal dividers are drawn.
    bool IsDrawHorizontalLines() const { return drawHorizontalLines_; }
    /// Horizontal lines will draw lines seperating each item from the next (vertical lines will be supported when columns are supported)
    void SetDrawHorizontalLines(bool state) { drawHorizontalLines_ = state; update(); }
    /// Returns true if a vertical line will be drawn seperating the row-header from the items.
    bool IsDrawHeaderLine() const { return drawHeaderVert_; }
    /// Toggles the drawing of a vertical line dividing the row headers from the items.
    void SetDrawHeaderLine(bool state) { drawHeaderVert_ = state; }

    /// Returns true if zebra-striping is used for drawing.
    bool IsZebraStripe() const { return zebraStripe_; }
    /// Zebra-striping will draw the rows with alternating light and dark colors.
    void SetZebraStripe(bool state) { zebraStripe_ = state; }

    /// Returns true if this list only allows a single object to be selected.
    bool IsSingleSelect() const { return isSingleSelect_; }
    /// Sets whether to allow multiple select or not.
    void SetSingleSelect(bool state) { isSingleSelect_ = state; update(); }

    /// Queries if the given item is in expanded state.
    bool IsExpanded(IMItemID who);
    /// Queries if the given item is selected.
    bool IsSelected(IMItemID who);
    /// Sets the given item's selection state (honors single select).
    void SetSelected(IMItemID who, bool state, bool repaint = true);
    /// Sets the given item's expansion state.
    void SetExpanded(IMItemID who, bool state, bool repaint = true);
    /// Remove all items from selection.
    void ClearSelected(bool repaint = true);

    /// Record selection and expansion states with the given void* key.
    void SaveStates(void* key);
    /// Restore selection and expansion states from the given void* key.
    void RestoreStates(void* key);
    /// Erase the stored data for selection and expansion states that matches the void* key.
    void DestroyState(void* key);

    /// Query for an item at a given point, returns true if was found. May optionally use only the drawn-rect or whole row, and can output the visual rect for the item if requested.
    bool ItemAt(IMItemID& id, const QPoint& pt, bool wholeRow = false, QRect* rect = 0x0);

    /// Whenever an update is forced the automatic update timer will be reset to prevent pointless repaints.
    void DoUpdate();

signals:
    void SelectionChanged(std::vector<IMItemID> selections);

public slots:
    void SetFilterString(const QString& text);
    void SetFilters(const QStringList& terms);

protected:
    virtual void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
    int CalculateTextWidth(const QString& text /*QString for UTF8*/);

    virtual bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE;
    virtual void contextMenuEvent(QContextMenuEvent*) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void mouseMoveEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void mouseReleaseEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void mouseDoubleClickEvent(QMouseEvent*) Q_DECL_OVERRIDE;
    virtual void keyReleaseEvent(QKeyEvent*) Q_DECL_OVERRIDE;
    virtual void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;
    virtual void dragLeaveEvent(QDragLeaveEvent*) Q_DECL_OVERRIDE;
    virtual void dragMoveEvent(QDragMoveEvent*) Q_DECL_OVERRIDE;
    virtual void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;

    /// Reset the ephemeral position tracker for measuring mouse-drag distance for drag-drop testing.
    void ResetDrag() { dragStart_.reset();  }
    /// Performs some cleanup to minimize possible pointer address matches causing expansion bugs.
    void ForgetItem(IMItemID id);

    void SetEmbeddedEditor(QWidget* widget);

    struct ItemState {
        bool expanded_ = true;

        ItemState() { }
        ItemState(bool exp) { expanded_ = exp; }
    };

    struct Expander {
        QRect rect_;
        IMItemID item_;
    };

    enum DropType {
        DT_ONTO,
        DT_BEFORE,
        DT_AFTER
    };

    static DropType DetermineDropType(const QPoint& pt, const QRect& rect);
    bool CanDoDrop(std::vector<IMItemID>& itemIDs, IMItemID whom, DropType nature);

    struct DropTarget {
        IMItemID dropTarget_;
        DropType dropNature_;

        DropTarget() { }
        DropTarget(IMItemID tgt, DropType type) : dropTarget_(tgt), dropNature_(type) { }
    };

    /// Tracks selections independently of the statemap, optimization
    std::vector<IMItemID> selectedItems_;
    /// Stores the presence of the items in sequential order, selectedItems_ is not trustworthy for sequence of items
    std::vector<IMItemID> visualIndex_;
    /// When drawing as a tree expanders will be pushed into this vector for input processing to expand/collapse items
    std::vector<Expander> expanders_;
    /// Start position of drag
    std::unique_ptr<QPoint> dragStart_;
    /// The object we're intending to drop onto/before/after, required so can paint be done appropriately
    std::unique_ptr<DropTarget> dropTarget_;

    typedef std::map<IMItemID, QRect> ItemRects;
    typedef std::map<IMItemID, ItemState> StateMap;
    
    struct SavedState {
        std::vector<IMItemID> selection_;
        StateMap expansionState_;
    };

    // Tracks the current painting position
    struct PaintState {
        // The QPainter being used
        QPainter* painter_;
        // current position in row header, reset for each item
        int rowHeaderPosition_ = 0;
        // Current position for item painting paint
        QPoint position_;
        // Location of the mouse, only present in response to single click events
        QPoint mousePosition_;
        // Farthest right we've gone?
        int maxWidth_ = 0;
        // For debugging
        int treeDepth_ = 0;

        void Reset() {
            rowHeaderPosition_ = 0;
            position_ = QPoint(0, 0);
            mousePosition_ = QPoint(-10000, -10000);
            maxWidth_ = 0;
            treeDepth_ = 0;
        }
    };
    PaintState paintingState_;

    ItemRects rects_;
    StateMap states_;
    /// Currently active embedded editor.
    QWidget* embeddedEditor_ = 0x0;

// General stylistic things
    /// Items without stored expansion state should default as expanded.
    bool defaultExpanded_ = true;
    /// If false then drawing is always forced into a flat format.
    bool drawAsTree_ = true;
    /// Whether top-most nodes in the tree are allowed to collapse or not.
    bool rootsCanCollapse_ = false;
    /// How intensely to indent.
    int indentSize_ = 24;
    /// Draw seperators between all items.
    bool drawHorizontalLines_ = false;
    /// Default text color for drawing
    QColor defaultTextColor_;

// Row-headers and footers
    /// Space to assign for row header.
    int rowHeaderWidth_ = 0;
    /// Draw a vertical line dividing the header portion from the items.
    bool drawHeaderVert_ = false;
    /// TODO: Space to assign for row footer (right-hand side of items)
    int rowFooterWidth_ = 0;
    /// TODO: Draw a vertical line dividing the footer portion.
    bool drawFooterVert_ = false;
    /// Alternate the background colors of rows.
    bool zebraStripe_ = true;
    /// Only allows a single selection.
    bool isSingleSelect_ = false;
    /// Image to use when an item is collapsed and viewing as tree
    QIcon chevronCollapsed_;
    /// Image to use when an item is expanded and viewing as tree
    QIcon chevronExpanded_;
    std::unordered_map<void*, SavedState> savedStates_;
    /// Filtering text
    QStringList filterText_;
    /// 
    QTimer updateTimer_;

// Font information
    /// Height of a character
    int shortHeight_;
    /// Line height (height + descent)
    int lineHeight_;
    // Glyph widts, faster than QFontMetrics::width(...)
    int characterWidths_[512];
};