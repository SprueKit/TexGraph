#pragma once

#include <EditorLib/IM/QImGui.h>
#include <EditorLib/Selectron.h>

#include <Urho3D/Math/StringHash.h>

#include <set>

namespace Urho3D
{
    class Serializable;
}

namespace UrhoEditor
{

    class UrhoIMPropertyEditor : public QImGui, public SelectronLinked
    {
        Q_OBJECT;
    public:
        UrhoIMPropertyEditor();
        UrhoIMPropertyEditor(std::set<Urho3D::StringHash> filters);

        virtual void GenerateUI(QPainter*, const QSize&) override;

        virtual void Link(Selectron* sel) override;

    private slots:
        void SelectionChange(void*, Selectron* sel) { update(); }

    protected:
        void PrintSerializable(Urho3D::Serializable* serializable, int depth);

        // If not empty then only these properties will be considered.
        std::set<Urho3D::StringHash> filters_;
    };

}