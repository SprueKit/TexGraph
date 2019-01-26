#pragma once

#include <QActionGroup>
#include <QWidget>

#include "Documents/Urho/Controls/UrhoSelectionMask.h"

class QComboBox;
class QSexyToolTip;

namespace SprueEditor
{

    class TextureDocument;

    /// Viewport shelf for texture documents.
    class TextureDocumentShelf : public QWidget
    {
        friend class TextureDocument;
        Q_OBJECT
    public:
        TextureDocumentShelf(TextureDocument* document);
        virtual ~TextureDocumentShelf();

    public slots:
        void ShaderChanged(const QString& text);
        void CubeMapChanged(int index);

        void UTileChanged(double value);
        void VTileChanged(double value);

    private:
        void LoadPrimitive(const char* text);
        bool LoadCustom(QString& path);

        TextureDocument* document_;

        QSexyToolTip* transformFlyout_;
        int lastPrimitiveIndex_ = 0;
    };

}