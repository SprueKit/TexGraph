#include "TextureGenTask.h"

#include "../../../SprueKitEditor.h"
#include "../../../InternalNames.h"
#include "../../../Views/RenderWidget.h"
#include "Documents/TexGen/Controls/TextureGraphControl.h"
#include "../../../UrhoHelpers.h"

#include "../TextureDocument.h"

#include <EditorLib/Controls/ISignificantControl.h>
#include <EditorLib/LogFile.h>
#include <EditorLib/Platform/Thumbnails.h>

#include <SprueEngine/FString.h>
#include <SprueEngine/TextureGen/TextureNode.h>

#include <Urho3D/Graphics/Texture2D.h>

#include <QElapsedTimer>
#include <QPixmap>

using namespace SprueEngine;

namespace SprueEditor
{
    TextureGenTask::TextureGenTask(SprueEngine::Graph* graph, SprueEngine::GraphNode* node, DocumentBase* doc) :
        Task(doc)
    {
        if (graph == 0x0)
            LOGERROR("Attempted to start Texture generation task without a graph");
        if (node == 0x0)
            LOGERROR("Attempted to start Texture generation task without a node");
        source_ = graph;
        nodeID_ = node->GetInstanceID();
        nodeName_ = node->name;

        // This has to be done here, because PrepareTask occurs in the thread.
        clone_ = (Graph*)source_->Clone();
        if (clone_)
        {
            clone_->SetUserData(source_->GetUserData()); // We need to make sure we have the right user data
            node_ = clone_->GetNodeBySourceID(nodeID_);
        }
    }

    TextureGenTask::~TextureGenTask()
    {
        if (clone_)
            delete clone_;
    }

    void TextureGenTask::PrepareTask()
    {
        
    }

    bool TextureGenTask::ExecuteTask()
    {
        // If we have neither a node or a clone then we're done because something has gone wrong
        if (clone_ == 0x0 || node_ == 0x0)
            return true;

        QElapsedTimer timer;
        timer.start();

        if (TextureOutputNode* node = dynamic_cast<TextureOutputNode*>(node_))
            image_ = node_->GetPreview(width_ != 0 ? width_ : node->Width, height_ != 0 ? height_ : node->Height);
        else
            image_ = node_->GetPreview(width_ != 0 ? width_ : 128, height_ != 0 ? height_ : 128);
        if (image_)
        {            
            generatedImage_.reset(new QImage(image_->getWidth(), image_->getHeight(), QImage::Format::Format_RGBA8888));
            for (unsigned y = 0; y < image_->getHeight(); ++y)
            {
                for (unsigned x = 0; x < image_->getWidth(); ++x)
                {
                    RGBA color = image_->get(x, y);
                    color.Clip();
                    QColor col;
                    col.setRedF(color.r);
                    col.setGreenF(color.g);
                    col.setBlueF(color.b);
                    col.setAlphaF(color.a);
                    generatedImage_->setPixelColor(QPoint(x, y), col);
                }
            }
        }

        auto ms = timer.elapsed();
        auto sec = ms / 1000;
        auto min = sec / 60;
        sec = sec % 60;
        ms = ms % 1000;

        QString msg = QString("Texture generation for '%1' took:").arg(node_->name.c_str());
        if (min > 0)
            msg += QString(" %1 min").arg(min);
        if (sec > 0)
            msg += QString(" %1 sec").arg(sec);
        if (ms > 0)
            msg += QString(" %1 ms").arg(ms);

        LOGINFO(msg);

        return true;
    }

    void TextureGenTask::FinishTask()
    {
        if (TextureGraphControl* panel = ISignificantControl::GetControl<TextureGraphControl>())
            panel->PreviewGenerated(this);
        if (TextureOutputNode* output = dynamic_cast<TextureOutputNode*>(node_))
        {
            TextureDocument* document = (TextureDocument*)output->graph->GetUserData();
            if (!document)
                return;

            TextureDocument::PreviewMaterial& mat = document->GetRoughMat();
            TextureDocument::PreviewMaterial& matB = document->GetGlossMat();
            Urho3D::Context* context = SprueKitEditor::GetInstance()->GetRenderer()->GetUrhoContext();
            switch (output->OutputType)
            {
            case TGOT_Albedo: {
                auto tex = CreateTexture(context, FString("ALBEDO_DOC_%1", document->GetDocumentIndex()).c_str(), image_, true);
                mat.first->SetTexture(Urho3D::TU_DIFFUSE, tex);
                mat.second->SetTexture(Urho3D::TU_DIFFUSE, tex);
                matB.first->SetTexture(Urho3D::TU_DIFFUSE, tex);
                matB.second->SetTexture(Urho3D::TU_DIFFUSE,tex);
                break;
            }
            case TGOT_Glossiness: {
                auto tex = CreateTexture(context, FString("GLOSSINESS_DOC_%1", document->GetDocumentIndex()).c_str(), image_, false);
                mat.first->SetTexture(Urho3D::TU_SPECULAR, tex);
                mat.second->SetTexture(Urho3D::TU_SPECULAR, tex);
                matB.first->SetTexture(Urho3D::TU_SPECULAR, tex);
                matB.second->SetTexture(Urho3D::TU_SPECULAR,tex);
                break;
            }
            case TGOT_Roughness: {
                auto tex = CreateTexture(context, FString("ROUGHNESS_DOC_%1", document->GetDocumentIndex()).c_str(), image_, false);
                mat.first->SetTexture(Urho3D::TU_SPECULAR, tex);
                mat.second->SetTexture(Urho3D::TU_SPECULAR, tex);
                matB.first->SetTexture(Urho3D::TU_SPECULAR, tex);
                matB.second->SetTexture(Urho3D::TU_SPECULAR,tex);
                break;
            }
            case TGOT_Metallic: {
                auto tex = CreateTexture(context, FString("METALLIC_DOC_%1", document->GetDocumentIndex()).c_str(), image_, false);
                mat.first->SetTexture(Urho3D::TU_EMISSIVE, tex);
                mat.first->SetTexture(Urho3D::TU_EMISSIVE, tex);
                matB.first->SetTexture(Urho3D::TU_EMISSIVE,tex);
                matB.first->SetTexture(Urho3D::TU_EMISSIVE,tex);
                break;
            }
            case TGOT_Specular: {
                auto tex = CreateTexture(context, FString("SPECULAR_DOC_%1", document->GetDocumentIndex()).c_str(), image_, true);
                mat.first->SetTexture(Urho3D::TU_EMISSIVE, tex);
                mat.second->SetTexture(Urho3D::TU_EMISSIVE, tex);
                matB.first->SetTexture(Urho3D::TU_EMISSIVE, tex);
                matB.second->SetTexture(Urho3D::TU_EMISSIVE, tex);
                break;
            }
            case TGOT_Normal: {
                auto tex = CreateTexture(context, FString("NORMAL_DOC_%1", document->GetDocumentIndex()).c_str(), image_, false);
                mat.first->SetTexture(Urho3D::TU_NORMAL, tex);
                mat.second->SetTexture(Urho3D::TU_NORMAL, tex);
                matB.first->SetTexture(Urho3D::TU_NORMAL, tex);
                matB.second->SetTexture(Urho3D::TU_NORMAL, tex);
                break;
            }
            case TGOT_Height: {
                document->SetEnableHeightShader(!image_->AllValuesEqual());
                auto tex = CreateTexture(context, FString("HEIGHT_DOC_%1", document->GetDocumentIndex()).c_str(), image_, false);
                mat.first->SetTexture(Urho3D::TU_ENVIRONMENT, tex);
                mat.second->SetTexture(Urho3D::TU_ENVIRONMENT, tex);
                matB.first->SetTexture(Urho3D::TU_ENVIRONMENT, tex);
                matB.second->SetTexture(Urho3D::TU_ENVIRONMENT, tex);
                break;
            }
            case TGOT_Subsurface: {
                auto tex = CreateTexture(context, FString("SUBSURFACE_DOC_%1", document->GetDocumentIndex()).c_str(), image_, true);
                mat.first->SetTexture(Urho3D::TU_CUSTOM2, tex);
                mat.second->SetTexture(Urho3D::TU_CUSTOM2, tex);
                matB.first->SetTexture(Urho3D::TU_CUSTOM2, tex);
                matB.second->SetTexture(Urho3D::TU_CUSTOM2, tex);
                break;
            }
            }
        }
    }
}