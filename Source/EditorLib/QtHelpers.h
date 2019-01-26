#pragma once

#include <EditorLib/editorlib_global.h>

#include <QString>
#include <QApplication>
#include <QClipboard>
#include <qlayout>
#include <qmimedata>
#include <qfontmetrics>
#include <qmenu>
#include <qmenubar>
#include <QAbstractButton>

#include <vector>

class QPainter;
class QFontMetrics;
class RegisteredAction;

namespace QtHelpers
{
    EDITORLIB_EXPORT void ClearLayout(QLayout* layout);
    EDITORLIB_EXPORT void HideLayout(QLayout* layout);
    EDITORLIB_EXPORT void ShowLayout(QLayout* layout);
    EDITORLIB_EXPORT void SetEnabledOnAll(QLayout* layout, bool state);

    EDITORLIB_EXPORT void UpdateMenus(QMenuBar* bar);
    EDITORLIB_EXPORT void UpdateMenus(QMenu* bar);

    /// Get the path to something in the AppData/Roaming directory
    EDITORLIB_EXPORT QString GetAppDataPath(const QString& file);
    /// Get the path to something in the Users/USERNAME/Documents directory
    EDITORLIB_EXPORT QString GetDocumentsPath(const QString& file);

    EDITORLIB_EXPORT QSize MeasureTextForceWrap(const QString& text, const QFontMetrics& metrics, int width, std::vector<std::pair<int, int> >* indices);
    EDITORLIB_EXPORT void PaintTextForceWrap(QPainter* painter, const QPoint& paintAt, const QString& text, QFontMetrics& metrics, int width);

    EDITORLIB_EXPORT void AttachHelpFlyout(QAbstractButton* button, const QString& name);

    /// Utility for quickly adding a collection of actions in a AddActions(bar, { actA, actB, actC }); form
    EDITORLIB_EXPORT void AddActions(QMenuBar* bar, const std::vector<RegisteredAction*>& actions);
    /// Utility for quickly adding a collection of actions in a AddActions(menu, { actA, actB, actC }); form
    EDITORLIB_EXPORT void AddActions(QMenu* menu, const std::vector<RegisteredAction*>& actions);
}