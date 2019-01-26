#pragma once

#include <EditorLib/editorlib_global.h>

#include <QFile>
#include <QString>

/// Verifies that the given file path contains XML data that starts with the named root element.
EDITORLIB_EXPORT bool CheckXMLRootElement(const QString& filePath, const QString& rootName);

/// Verifies that the given file contains XML data that starts with the named root element. Uses QXmStream for minimal checking.
EDITORLIB_EXPORT bool CheckXMLRootElement(QFile* file, const QString& rootName);