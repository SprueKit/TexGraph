#pragma once

#include <EditorLib/editorlib_global.h>

#include <QAbstractButton>
#include <QBoxLayout>
#include <QIcon>
#include <QWidget>

#include <vector>

class RibbonSection;

/// A page/tab in a "Ribbon" control.
class EDITORLIB_EXPORT RibbonPage : public QWidget
{
    Q_OBJECT;

    friend class Ribbon;
public:
    RibbonPage(const QString& pageTitle);
    virtual ~RibbonPage();

    QString GetPageLabel() { return pageLabel_; }

    void AddSection(RibbonSection* section);
    RibbonSection* GetSection(unsigned index);
    RibbonSection* RemoveSection(unsigned index);
    RibbonSection* RemoveSection(RibbonSection* section);

    void SetPageTitle(const QString& title);
    QString GetPageTitle() const { return title_; }

    bool IsActive() { return active_; }
    void SetActive(bool state);

signals:
    void ActiveChanged(RibbonPage* self);

private:
    /// Whether the page will be displayed in the ribbon tabs or not.
    bool active_ = true;
    /// Title for the tab of the page.
    QString title_;
    /// The ribbon control that owns this page.
    Ribbon* ribbon_ = 0x0;
    /// Color to be used when rendering the tab for the widget (in ribbon mode).
    QColor pageTabColor_;
    /// Icon used used for the ribbon.
    QIcon pageIcon_;
    /// Label for this page.
    QString pageLabel_;
    /// The horizontal layout that the sections are placed into.
    QHBoxLayout* sectionsLayout_;
    /// Ribbon sections.
    std::vector<RibbonSection*> sections_;
    /// Button for toggling whether to display the ribbon constantly or as a popup widget.
    QAbstractButton* ribbonCollapseButton_;
};