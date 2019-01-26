#pragma once

#include <qstring.h>

class QMainWindow;

/* Preference is for new code to use Global_XXXXX functions instead of direct singleton access.

Incomplete types int he Global_XXXXX functions reduces the required headers to only those of the actual object being sought,
not the full SprueKitEditor -> RenderWidget -> ViewManager -> ViewBase etc, path

Old code should be migrated to this as time and encountering of old style access.
*/

namespace SprueEditor
{
    class RenderWidget;
    class SprueKitEditor;
    class ViewManager;
}

class DocumentManager;
class Selectron;
class TaskProcessor;

void Global_SetStatusBarMessage(const QString& message, int duration = 1000);

SprueEditor::SprueKitEditor* Global_GetEditor();

TaskProcessor* Global_MainTaskProcessor();

TaskProcessor* Global_SecondaryTaskProcessor();

DocumentManager* Global_DocumentManager();

SprueEditor::RenderWidget* Global_RenderWidget();

SprueEditor::ViewManager* Global_ViewManager();

Selectron* Global_ObjectSectron();

QMainWindow* Global_MainWindow();

/// Shows a generic error message.
void Global_ShowErrorMessage(const QString& title, const QString& message);