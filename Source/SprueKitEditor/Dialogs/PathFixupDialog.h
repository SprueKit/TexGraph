#pragma once

#include <QDialog>
#include <QString>

#include <memory>

namespace SprueEditor
{
    /// Subclass and implement in order to provide support of using the PathFixupDialog to correct erroneous paths.
    struct PathFixupItem
    {
        /// Object name is the name of the thing containing the bad path.
        virtual QString GetObjectName() const = 0;
        /// Key name is the name of the field with the bad path.
        virtual QString GetKeyName() const = 0;
        /// Gets the current path for the field with a path error.
        virtual QString GetCurrentPath() const = 0;
        /// Sets the new path for the object and returns true if it was successfull.
        virtual bool SetNewPath(const QString& newPath) = 0;
        /// Return a user friendly type name, like 'Image' or 'Model'
        virtual QString GetFileTypeName() const = 0;
        /// Return the QFileDialog mask "Image files (*.png, *.jpeg)"
        virtual QString GetFileMask() const = 0;
    };

    /// Reusable dialog (via sublcassing PathFixupItem) for displaying erroneous filepaths and providing the opportunity
    /// to fix them before proceeding further.
    class PathFixupDialog : public QDialog
    {
        Q_OBJECT;
    public:
        PathFixupDialog(const std::vector< std::shared_ptr<PathFixupItem> >& items, QWidget* parent = 0x0);
        virtual ~PathFixupDialog();

    private:
        std::vector< std::shared_ptr<PathFixupItem> > items_;
    };

}