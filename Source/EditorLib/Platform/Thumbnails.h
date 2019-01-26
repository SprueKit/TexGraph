#pragma once

#include <EditorLib/editorlib_global.h>

#include <QIcon>
#include <QPixmap>
#include <QFileIconProvider>


/// Handles the platform specific task of fetching file preview thumbnails from the filesystem.
/// TODO: add OBJ and FBX quick renders and a cache
class EDITORLIB_EXPORT Thumbnails : public QFileIconProvider
{
    /// Prevent copy.
    Thumbnails(const Thumbnails&);
public:
    Thumbnails();
    ~Thumbnails();

    virtual QIcon icon(IconType type) const override;
    virtual QIcon icon(const QFileInfo &info) const override;
    QPixmap GetPixmap(const QString& filePath) const;

private:
    /// Hides platform specific nastiness in the cpp file. Keep interface clean.
    struct Opaque;
    Opaque* privateData_;
};
