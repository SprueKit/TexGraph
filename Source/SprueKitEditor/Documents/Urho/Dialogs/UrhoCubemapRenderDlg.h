#pragma once

#include <QDialog>

namespace Urho3D
{
    class Zone;
}

namespace UrhoEditor
{
    /// Dialog for rendering cubemaps for zones
    class UrhoCubemapRenderDlg : public QDialog
    {
    public:
        UrhoCubemapRenderDlg(Urho3D::Zone*);
        virtual ~UrhoCubemapRenderDlg();

    private:
        /// Zone we're rendering a cubemap for
        Urho3D::Zone* zone_;
        int imageSize_ = 256;
    };

}