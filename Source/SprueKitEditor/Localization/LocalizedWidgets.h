#pragma once

#include <EditorLib/ThirdParty/QSexyToolTip.h>

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QListWidget>
#include <QStringList>
#include <QTableWidget>
#include <QTreeWidget>

namespace SprueEditor
{
    /// Specialization of QLabel that is hooked up for automatic localization.
    class LocalizedLabel : public QLabel
    {
        Q_OBJECT
    public:
        LocalizedLabel(const QString& text, const QString& tip = QString());

    private slots:
        void LanguageChanged();
    private:
        QString textKey_;
        QString tip_;
    };

    /// Specialization of QPushbutton that uses automatic localization (text/tooltip).
    class LocalizedButton : public QPushButton
    {
        Q_OBJECT
    public:
        LocalizedButton(const QString& text, const QString& toolTip);
    private slots:
        void LanguageChanged();
    private:
        QString textKey_;
        QString tooltipKey_;
    };

    /// Specialization of QCheckBox that is hooked up for automatic localization.
    class LocalizedCheckBox : public QCheckBox
    {
        Q_OBJECT
    public:
        LocalizedCheckBox(const QString& text, const QString& toolTip = QString());

    private slots:
        void LanguageChanged();

    private:
        QString textKey_;
        QString tooltipKey_;
    };

    /// Specialization of QComboBox that is hooked up for automatic localization.
    class LocalizedComboBox : public QComboBox
    {
        Q_OBJECT
    public:
        LocalizedComboBox();

        /// This method must be used instead of QComboBox::addItem so that the original key strings are tracked.
        void AddItem(const QString &text, const QVariant &userData = QVariant());
        /// This method must be used instead of QComboBox::addItem so that the original key strings are tracked.
        void AddItem(const QIcon &icon, const QString &text, const QVariant &userData = QVariant());
        /// This method must be used instead of QComboBox::setToolTip so that the original string is tracked.
        void SetToolTip(const QString& tip);

    private slots:
        void LanguageChanged();

    private:
        QStringList originalItemText_;
        QString tip_;
    };

    class LocalizedDockHeader : public QWidget
    {
        Q_OBJECT;
    public:
        LocalizedDockHeader(const QString& label, QDockWidget* parent);

        QHBoxLayout* GetControlLayout() { return leftSide_; }

    private:
        QDockWidget* dock_ = 0x0;
        QHBoxLayout* leftSide_ = 0x0;
        QLabel* titleLabel_ = 0x0;
        QToolButton* helpButton_ = 0x0;
        QToolButton* closeButton_ = 0x0;
        QToolButton* popOutButton_ = 0x0;
        QSexyToolTip* helpWindow_ = 0x0;
    };

    class LocalizedDock : public QDockWidget
    {
        Q_OBJECT
    public:
        LocalizedDock(const QString& objectName, const QString& title, const QString& tip = QString());
        virtual ~LocalizedDock();

        void SetTitle(const QString& title);
        void SetToolTip(const QString& tip);

        static std::vector<LocalizedDock*> GetInstances();

    private slots:
        void LanguageChanged();
        void FloatingChanged(bool isFloating);

    private:
        QString originalTitle_;
        QString originalTip_;
        LocalizedDockHeader* localizedHeader_ = 0x0;

        static std::vector<LocalizedDock*> allDocks_;
    };

    class LocalizedListWidget : public QListWidget
    {
        Q_OBJECT;
    public:
        LocalizedListWidget();
        virtual ~LocalizedListWidget() { }

    private slots:
        void LanguageChanged();
    };

    /// Only the header is translated.
    class LocalizedTreeWidget : public QTreeWidget
    {
        Q_OBJECT
    public:
        LocalizedTreeWidget();

        void setHeaderLabels(const QStringList& list);

    private slots:
        void LanguageChanged();

    private:
        QStringList originalHeaderLabels_;
    };

    /// Only the headers are translated.
    class LocalizedTableWidget : public QTableWidget
    {
        Q_OBJECT
    public:
        LocalizedTableWidget();

        void setHorizontalHeaderLabels(const QStringList& list);
        void setVerticalHeaderLabels(const QStringList& list);

    private slots:
        void LanguageChanged();

    private:
        QStringList originalHorizontalLabels_;
        QStringList originalVerticalLabels_;
    };
}