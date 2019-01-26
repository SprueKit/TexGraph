#pragma once

#include <EditorLib/DocumentBase.h>

#include <Urho3D/Scene/Scene.h>

namespace UrhoEditor
{

    /// Baseclass for Urho3D related documents. Keeps repetitive elements (such as the scene) in a common place.
    class BaseUrhoDocument : public DocumentBase
    {
    public:
        BaseUrhoDocument(DocumentHandler* handler, Urho3D::SharedPtr<Urho3D::Scene> scene) :
            DocumentBase(handler),
            scene_(scene)
        {

        }
        virtual ~BaseUrhoDocument() { }

        /// Return the scene that is being edited (or possibly just viewed in the case of a material)
        Urho3D::SharedPtr<Urho3D::Scene> GetScene() { return scene_; }

    protected:
        Urho3D::SharedPtr<Urho3D::Scene> scene_;
    };

}