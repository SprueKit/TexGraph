#pragma once

#include <EditorLib/editorlib_global.h>

#include <QAbstractButton>
#include <QPushButton>
#include <QTabWidget>
#include <QWidget>

#include <vector>

class RibbonPage;

/// The main ribbon control. Contains a set of RibbonPages which are themselves composed of RibbonSections.
/// Unlike the MS ribbon this ribbon can be oriented vertically for wide-screens, in which case the form of it.
/// In vertical mode only the page icons are displayed (tooltip however indcates the text)
class EDITORLIB_EXPORT Ribbon : public QTabWidget
{
    Q_OBJECT;
public:
    Ribbon(QWidget* parent = 0x0);
    virtual ~Ribbon();

    void AddPage(RibbonPage* page);
    RibbonPage* GetCurrentPage() { return (RibbonPage*)currentWidget(); }
    RibbonPage* GetPage(unsigned index);
    void RemovePage(RibbonPage* page);

    QPushButton* GetSettingsButton() { return settingsButton_; }
    QPushButton* GetApplicationButton() { return applicationButton_; }
    QPushButton* GetDocumentButton() { return documentButton_; }

    bool IsAutoHide() { return autoHide_; }
    void SetAutoHide(bool state);

    void SetShownHeight(int amnt) { shownHeight_ = amnt; }

signals:
    void PageAdded(RibbonPage* page);
    void PageRemoved(RibbonPage* page);
    void ApplicationButtonClicked();

public slots:
    void SetRibbonDisplayed(bool state);

private slots:
    void PageActivityChanged(RibbonPage* page);
    void TabClicked(int index);
    void HandleApplicationButton(bool);

private:
    std::vector<RibbonPage*> allpages_;
    QAbstractButton* toggleRibbon_;
    QPushButton* settingsButton_;
    QPushButton* applicationButton_;
    QPushButton* documentButton_;
    /// Whether to automatically collapse the ribbon.
    bool autoHide_ = true;
    int oldHeight_ = 0;
    int shownHeight_ = 0;
};