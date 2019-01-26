#include "ViewManager.h"

#include "Gizmo.h"
#include "../UrhoHelpers.h"
#include "../Panels/SceneTreePanel.h"
#include "../GlobalAccess.h"
#include "../SprueKitEditor.h"
#include "ViewUtility.h"
#include "InputRecorder.h"
#include "RenderWidget.h"

// SprueEditor
#include "../Documents/Sprue/SprueDocument.h"

#include <EditorLib/LogFile.h>

#include <qapplication.h>
#include <qwidget.h>
#include <qtimer.h>

#include <SprueEngine/Core/SceneObject.h>

#include <Urho3D/Engine/Application.h>
#include <Urho3D/Graphics/Camera.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Engine/DebugHud.h>
#include <Urho3D/Graphics/DebugRenderer.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/IO/IOEvents.h>
#include <Urho3D/Engine/EngineEvents.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/GraphicsEvents.h>
#include <Urho3D/IO/Log.h>
#include <Urho3D/Graphics/Octree.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Viewport.h>
#include <Urho3D/Resource/XMLFile.h>
#include <Urho3D/Graphics/Zone.h>

#include <Urho3D/AngelScript/Script.h>

#include <SprueEngine/Core/SceneObject.h>

using namespace Urho3D;

namespace SprueEditor
{

ViewManager::ViewManager(QWidget* holder) : 
    Urho3D::Object(new Context())
{
    renderWidget_ = dynamic_cast<RenderWidget*>(holder);
    context_ = this->GetContext();
    engine_ = new Engine(context_);

    // Instantiate and register the AngelScript subsystem
    context_->RegisterSubsystem(new Script(context_));

    SubscribeToEvent(E_LOGMESSAGE, URHO3D_HANDLER(ViewManager, HandleLogMessage));
    SubscribeToEvent(E_RENDERUPDATE, URHO3D_HANDLER(ViewManager, HandleRenderUpdate));

    VariantMap engineParameters;
    engineParameters["FrameLimiter"] = false;
    engineParameters["LogName"] = "SprueKitEditor.log";
    engineParameters["ExternalWindow"] = (void*)(holder->winId());
    if (!engine_->Initialize(engineParameters))
        return;

    // Get default style
    ResourceCache* cache = context_->GetSubsystem<ResourceCache>();
    cache->SetAutoReloadResources(true);
    XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Create debug HUD.
    DebugHud* debugHud = engine_->CreateDebugHud();
    debugHud->SetDefaultStyle(xmlFile);
    //debugHud->Toggle(Urho3D::DEBUGHUD_SHOW_STATS);

    CreateSetup();

    connect(Global_DocumentManager(), &DocumentManager::ActiveDocumentChanged, this, &ViewManager::DocumentChanged);
}

ViewManager::~ViewManager()
{
    engine_->Exit();
    delete engine_;
}

void ViewManager::HandleLogMessage(StringHash event, VariantMap& params)
{
    Urho3D::String str = params[Urho3D::LogMessage::P_MESSAGE].ToString();
    // split off the timing information, currently unused
    // TODO: remove the INFO:, WARNING:, DEBUG:, ERROR: portion too?
    str = str.Substring(str.Find(']') + 1);
    str = str.Substring(str.Find(':') + 1);

    LogFile::GetInstance()->Write("Urho3D", str.CString(), 4 - params[Urho3D::LogMessage::P_LEVEL].GetInt());
}

InputRecorder* ViewManager::GetInput() const
{
    return renderWidget_->GetInput();
}

void ViewManager::CreateSetup()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    renderer->GetDefaultZone()->SetFogColor(Urho3D::Color(0.25f, 0.25f, 0.25f));

    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), this, SLOT(OnTimeout()));
    timer_->start(20);
}

void ViewManager::HandleRenderUpdate(Urho3D::StringHash event, Urho3D::VariantMap& params)
{
    IntVector2 delta = GetInput()->GetMouseDelta();
    IntVector2 pos = context_->GetSubsystem<Input>()->GetMousePosition();// GetInput()->GetMousePos();

    if (activeView_)
        activeView_->CheckControllers();

    GetInput()->UpdateFrame();

    if (activeView_)
        activeView_->RenderUpdate();
}

void ViewManager::RunFrame()
{
    QSize renderSize = ((QWidget*)renderWidget_->parent())->size();
    if (engine_ && !engine_->IsExiting() && (renderSize.height() > 0 && renderSize.width() > 0))
    {
        engine_->Update();
    }
}

void ViewManager::OnTimeout()
{
    QSize renderSize = ((QWidget*)renderWidget_->parent())->size();
    if (engine_ && !engine_->IsExiting() && (renderSize.height() > 0 && renderSize.width() > 0))
    {
        engine_->RunFrame();
        renderWidget_->repaintLbl();
    }
}

void ViewManager::OnDataChanged(Selectron* sel, bool visuallyChanged)
{
    if (activeView_)
    {
        activeView_->SelectionChanged(sel);
        if (visuallyChanged)
            activeView_->VisuallyChanged(sel);
    }
}

void ViewManager::DocumentChanged(DocumentBase* newDoc, DocumentBase* oldDoc)
{
    if (activeView_)
        activeView_->Deactivate();
    if (newDoc)
    {
        if (oldDoc)
            disconnect(oldDoc, &DocumentBase::ViewChanged, this, &ViewManager::OnViewChanged);
        connect(newDoc, &DocumentBase::ViewChanged, this, &ViewManager::OnViewChanged);
        activeView_ = newDoc->GetActiveView();
        activeView_->manager_ = this;
    }
    else
        activeView_.reset();
    if (activeView_)
        activeView_->Activate();
    else
        context_->GetSubsystem<Renderer>()->SetViewport(0, 0x0);
    emit ViewChanged(activeView_.get());
}

void ViewManager::OnViewChanged(DocumentBase* doc)
{
    if (doc)
    {
        if (activeView_)
            activeView_->Deactivate();
        activeView_ = doc->GetActiveView();
        activeView_->manager_ = this;
        activeView_->Activate();
        emit ViewChanged(activeView_.get());
    }
}

}