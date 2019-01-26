#include "TextureDocument.h"

#include "../../Commands/SprueKit/GraphCmds.h"
#include "../../GlobalAccess.h"
#include "../../Dialogs/PathFixupDialog.h"
#include "../../Views/RenderWidget.h"
#include "../../Dialogs/SaveImageDialog.h"
#include "../../SprueKitEditor.h"
#include "TextureDocumentShelf.h"
#include "TextureGraphExportDialog.h"
#include "TextureGraphReportDialog.h"
#include "TexturePreviewView.h"
#include "../../Data/TexGenData.h"
#include "Documents/Sprue/Dialogs/IEditablePathFixupItem.h"

#include "Tasks/TextureGenTask.h"

#include "../../ThirdParty/NodeEditor/QNEBlock.h"
#include "../../ThirdParty/NodeEditor/QNEConnection.h"
#include "../../ThirdParty/NodeEditor/QNEPort.h"

#include <EditorLib/Localization/Localizer.h>
#include <EditorLib/LogFile.h>

#include <SprueEngine/Loaders/BasicImageLoader.h>
#include <SprueEngine/Core/Context.h>
#include <SprueEngine/FileBuffer.h>
#include <SprueEngine/Graph/Graph.h>
#include <SprueEngine/TextureGen/TextureNode.h>

#include <Urho3D/Resource/ResourceCache.h>

#include <QAction>
#include <QDialog>
#include <QIcon>
#include <QGraphicsScene>
#include <QMenu>

using namespace SprueEngine;

namespace SprueEditor
{

int TextureDocument::nextDocumentIndex_ = 1;

class TextureDocumentGraphVisitor : public Graph::NodeVisitor
{
public:
    TextureDocumentGraphVisitor(TextureDocument* panel) : panel_(panel)
    {

    }

    virtual bool Visit(GraphNode* node) override
    {
        panel_->QueuePreviewUpdate(node);
        return true;
    }

    TextureDocument* panel_;
};

#define DO_PREVIEW_UPDATE(NODE) if (graph_) { TextureDocumentGraphVisitor visitor(this); graph_->VisitDownStream(&visitor, NODE); }

void TextureGraphBrushStrategy(QNEPort* port)
{
    if (port->ptr() == 0x0)
        return;
    if (GraphSocket* socket = (GraphSocket*)port->ptr())
    {
        if (socket->typeID == TEXGRAPH_FLOAT)
        {
            QLinearGradient grad;
            grad.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
            grad.setStart(0, 0.0f);
            grad.setFinalStop(0, 1.0f);
            grad.setColorAt(0, Qt::white);
            grad.setColorAt(1.0f, Qt::black);
            port->setBrush(grad);
        }
        else if (socket->typeID == TEXGRAPH_RGBA)
        {
            QConicalGradient grad;
            grad.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
            grad.setCenter(0.5f, 0.5f);
            QGradientStops stops;
            stops.push_back(QGradientStop(0.0f, Qt::red));
            stops.push_back(QGradientStop(0.33f, Qt::green));
            stops.push_back(QGradientStop(0.66f, Qt::blue));
            stops.push_back(QGradientStop(1.0f, Qt::red));
            grad.setStops(stops);
            port->setBrush(grad);
        }
        else if (socket->typeID == TEXGRAPH_MASK)
        {
            SPRUE_ASSERT(0, "Mask was not expected to be used");
        }
        else if (socket->typeID == TEXGRAPH_CHANNEL)
        {
            QConicalGradient grad;
            grad.setCoordinateMode(QGradient::CoordinateMode::ObjectBoundingMode);
            grad.setCenter(0.5f, 0.5f);
            QGradientStops stops;
            stops.push_back(QGradientStop(0.0f, Qt::red));
            stops.push_back(QGradientStop(0.33f, Qt::darkYellow));
            stops.push_back(QGradientStop(0.66f, Qt::yellow));
            stops.push_back(QGradientStop(1.0f, Qt::red));
            grad.setStops(stops);
            port->setBrush(grad);
        }
    }
}

DocumentBase* TextureDocumentHandler::CreateNewDocument()
{
    Graph* ret = new Graph();

    const int stepSize = 200;
    int startY = -stepSize * 3;

    TextureOutputNode* albedo = new TextureOutputNode();
    albedo->Construct();
    albedo->name = "Albedo";
    albedo->Format = TexGenOutputFormat::TGOF_RGB;
    albedo->OutputType = TexGenOutputType::TGOT_Albedo;
    albedo->YPos = startY;
    startY += stepSize;
    ret->AddNode(albedo, true);

    TextureOutputNode* roughness = new TextureOutputNode();
    roughness->Construct();
    roughness->name = "Roughness";
    roughness->Format = TexGenOutputFormat::TGOF_Alpha;
    roughness->OutputType = TexGenOutputType::TGOT_Roughness;
    roughness->YPos = startY;
    startY += stepSize;
    ret->AddNode(roughness, true);

    TextureOutputNode* metallic = new TextureOutputNode();
    metallic->Construct();
    metallic->name = "Metallic";
    metallic->Format = TexGenOutputFormat::TGOF_Alpha;
    metallic->OutputType = TexGenOutputType::TGOT_Metallic;
    metallic->YPos = startY;
    startY += stepSize;
    ret->AddNode(metallic, true);

    TextureOutputNode* normal = new TextureOutputNode();
    normal->Construct();
    normal->name = "Normal";
    normal->Format = TexGenOutputFormat::TGOF_RGB;
    normal->OutputType = TexGenOutputType::TGOT_Normal;
    normal->YPos = startY;
    normal->DefaultColor = RGBA(0.5f, 0.5f, 1.0f); // default to pefect normal
    startY += stepSize;
    ret->AddNode(normal, true);

    TextureOutputNode* height = new TextureOutputNode();
    height->Construct();
    height->name = "Height";
    height->Format = TexGenOutputFormat::TGOF_Alpha;
    height->OutputType = TexGenOutputType::TGOT_Height;
    height->YPos = startY;
    height->DefaultColor = RGBA(1, 1, 1, 1);
    startY += stepSize;
    ret->AddNode(height, true);

// BEGIN TEST BLOCK
#if 0
    for (auto record : TexGenData::NodeNames)
    {
        auto newNode = SprueEngine::Context::GetInstance()->Create<TextureNode>(record.first);
        newNode->Construct();
        newNode->name = record.second.first;
        ret->AddNode(newNode, false);
    }
#endif
// END TEST BLOCK

    return new TextureDocument(this, ret, QString());
}

DocumentBase* TextureDocumentHandler::OpenDocument(const QString& path)
{
    Graph* graph = new Graph();
    QString str(path);
    
    SprueEngine::SerializationContext ctx;
    ctx.relativePath_ = QFileInfo(path).dir().absolutePath().toStdString();

    if (str.endsWith(".xml"))
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError errCode = doc.LoadFile(path.toStdString().c_str());
        if (errCode == tinyxml2::XMLError::XML_SUCCESS)
        {
            if (tinyxml2::XMLElement* root = doc.FirstChildElement("graph"))
            {
                graph->Deserialize(root, ctx);
                IEditablePathFixupItem::DoDialog(ctx);
                return new TextureDocument(this, graph, path);
            }
            delete graph;
        }
        else
        {
            LOGERROR(doc.GetErrorStr1());
        }
    }
    else
    {
        FileBuffer buffer(path.toStdString().c_str(), true, true);
        if (buffer.ReadFileID().compare("TEXG") == 0)
        {
            // Read the string hash, had to write the hash in order to work with Clone() correctly.
            buffer.ReadStringHash();
            graph->Deserialize(&buffer, ctx);

            IEditablePathFixupItem::DoDialog(ctx);
            return new TextureDocument(this, graph, path);
        }
    }
    return 0x0;
}

QString TextureDocumentHandler::GetOpenFileFilter() const
{
    return "Texture Graph File (*.texg *.xml);;XML Texture Graph (*.xml);;Binary Texture Graph (*.texg)";
}

QString TextureDocumentHandler::GetSaveFileFilter() const
{
    return "XML Texture Graph (*.xml);;Binary Texture Graph (*.texg)";
}

static const char* TexGenMaterials[] = {
    "Materials/TextureGenPBR.xml",
    "Materials/TextureGenPBR_Gloss.xml",
    "Materials/TextureGenPBRHeight.xml",
    "Materials/TextureGenPBRHeight_Gloss.xml",
    0x0
};

TextureDocument::TextureDocument(DocumentHandler* handler, SprueEngine::Graph* graph, const QString& filePath) :
    GraphDocument(handler, graph)
{
    shelfWidget_ = new TextureDocumentShelf(this);
    graph_->SetUserData((void*)this);
    SetFilePath(filePath);

    Urho3D::Context* context = SprueKitEditor::GetInstance()->GetRenderer()->GetUrhoContext();

    // PBR Rough Metal
    {
        Urho3D::Material* matBase = context->GetSubsystem<Urho3D::ResourceCache>()->GetResource<Urho3D::Material>("Materials/TextureGenPBR.xml");
        Urho3D::Material* matBaseHeight = context->GetSubsystem<Urho3D::ResourceCache>()->GetResource<Urho3D::Material>("Materials/TextureGenPBRHeight.xml");
        roughMetalPreviewMaterial_.first = matBase->Clone();
        roughMetalPreviewMaterial_.second = matBaseHeight->Clone();
    }

    // PBR Gloss Spec
    {
        Urho3D::Material* matBase = context->GetSubsystem<Urho3D::ResourceCache>()->GetResource<Urho3D::Material>("Materials/TextureGenPBR_Gloss.xml");
        Urho3D::Material* matBaseHeight = context->GetSubsystem<Urho3D::ResourceCache>()->GetResource<Urho3D::Material>("Materials/TextureGenPBRHeight_Gloss.xml");
        glossPreviewMaterial_.first = matBase->Clone();
        glossPreviewMaterial_.second = matBaseHeight->Clone();
    }
    

    views_.push_back(std::shared_ptr<ViewBase>(new TexturePreviewView(this, context)));
    activeView_ = views_[0];
    documentIndex_ = nextDocumentIndex_++;

    shelfWidget_->CubeMapChanged(0);
}

TextureDocument::~TextureDocument()
{
    if (scene_)
        delete scene_;
    delete graph_;
}

bool TextureDocument::Save()
{
    QString file(filePath_);

    SprueEngine::SerializationContext ctx;
    ctx.relativePath_ = QFileInfo(filePath_).dir().absolutePath().toStdString();

    if (file.endsWith(".xml", Qt::CaseInsensitive))
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLElement* root = doc.NewElement("graph");
        doc.InsertFirstChild(root);
        graph_->Serialize(root, ctx);
        auto error = doc.SaveFile(filePath_.toStdString().c_str());
        dirty_ = false;

        if (error != tinyxml2::XMLError::XML_SUCCESS)
        {
            QString msg = QString("Failed to save XML texture file %1").arg(filePath_);
            Global_SetStatusBarMessage(msg, 10000);
            LOGERROR(msg);
        }
        return error == tinyxml2::XMLError::XML_SUCCESS;
    }
    else
    {
        SprueEngine::FileBuffer buffer(filePath_.toStdString().c_str(), false, true);
        buffer.WriteFileID("TEXG");
        bool error = graph_->Serialize(&buffer, ctx);
        dirty_ = false;

        if (!error)
        {
            QString msg = QString("Failed to save binary Texture file %1").arg(filePath_);
            Global_SetStatusBarMessage(msg, 10000);
            LOGERROR(msg);
        }

        return error;
    }
}

bool TextureDocument::DoBackup(const QString& backupDir)
{
    QString writePath;
    if (filePath_.isEmpty())
        writePath = QString("Unnamed_Graph_%1.xml").arg(QString::number((uint64_t)graph_));
    else
        writePath = filePath_;

    SprueEngine::SerializationContext ctx;
    ctx.relativePath_ = QFileInfo(filePath_).dir().absolutePath().toStdString();

    QString finalPath = QDir::cleanPath(backupDir + QDir::separator() + writePath);
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("graph");
    doc.InsertFirstChild(root);
    graph_->Serialize(root, ctx);
    auto error = doc.SaveFile(finalPath.toStdString().c_str());
    if (error != tinyxml2::XMLError::XML_SUCCESS)
        return false;
    return true;
}

void TextureDocument::BeginExport()
{
    TextureGraphExportDialog dialog;
    dialog.setModal(true);
    dialog.exec();
}

bool TextureDocument::HasReports() const
{
    return true;
}

void TextureDocument::BeginReport()
{
    TextureGraphReportDialog dialog;
    dialog.setModal(true);
    dialog.exec();
}

QNEBlock* TextureDocument::CreateBlockFor(SprueEngine::GraphNode* node)
{
    QNEBlock* block = GraphDocument::CreateBlockFor(node);
    block->setTitleColor(TexGenData::GetColorOf(node->GetTypeName()));
    block->setIcon(TexGenData::GetIconFor(node->GetTypeName()));

    auto foundName = TexGenData::NodeNames.find(node->GetTypeHash());
    std::string name = foundName != TexGenData::NodeNames.end() && node->name.empty() ? foundName->second.first : node->name.c_str();
    node->name = name;
    block->ports()[0]->setName(node->name.c_str());

    // If we're an output node then set the color
    if (dynamic_cast<TextureOutputNode*>(node))
        block->setBackgroundColor(QColor(40, 120, 40));
    else
        block->setBackgroundColor(QColor(40, 40, 40));

    DO_PREVIEW_UPDATE(node);

    return block;

}

GRAPH_SOCKET_BRUSH_STRATEGY TextureDocument::GetPortStrategy(SprueEngine::GraphSocket* forSocket)
{
    return TextureGraphBrushStrategy;
}

void TextureDocument::QueuePreviewUpdate(SprueEngine::GraphNode* node)
{
    if (node && node->CanPreview())
    {
        std::shared_ptr<Task> task = std::make_shared<TextureGenTask>(graph_, node, this);
        if (DocumentBase* doc = Global_DocumentManager()->GetActiveDocument())
            doc->AddDependentTask(task);
        SprueKitEditor::GetInstance()->GetTaskProcessor()->AddTask(task);
    }
}

QMenu* TextureDocument::GetContextMenu(QGraphicsView* view, QGraphicsItem* item, QPointF pt, std::vector<QAction*>& standardActions)
{
    QMenu* menu = new QMenu();
    if (item)
    {
        if (item->type() == QNEBlock::Type)
        {
            QNEBlock* block = (QNEBlock*)item;

            // Clear the selection so that nothing awkward happens ...
            view->scene()->clearSelection();
            item->setSelected(true);

            QAction* refresh = new QAction("Refresh");
            connect(refresh, &QAction::triggered, [=](bool) {
                DO_PREVIEW_UPDATE((GraphNode*)block->getUserData())
                    //QueuePreviewUpdate((GraphNode*)block->getUserData());
            });

            QAction* exportAct = new QAction("Export Texture");
            connect(exportAct, &QAction::triggered, [=](bool) {
                SaveImageDialog dlg;
                if (dlg.exec() == QDialog::Accepted)
                {
                    QString fileName = dlg.selectedFiles().size() ? dlg.selectedFiles()[0] : QString();
                    int selectedType = dlg.nameFilters().indexOf(dlg.selectedNameFilter(), 0);

                    if (!fileName.isEmpty())
                    {
                        TextureNode* node = ((TextureNode*)block->getUserData());
                        std::shared_ptr<FilterableBlockMap<RGBA>> image = node->GetPreview(SaveImageDialog::ImageWidth, SaveImageDialog::ImageHeight);
                        if (selectedType == 0)
                        {
                            if (!fileName.endsWith(".png"))
                                fileName.append(".png");
                            BasicImageLoader::SavePNG(image.get(), fileName.toStdString().c_str());
                        }
                        else if (selectedType == 1)
                        {
                            if (!fileName.endsWith(".tga"))
                                fileName.append(".tga");
                            BasicImageLoader::SaveTGA(image.get(), fileName.toStdString().c_str());
                        }
                        else if (selectedType == 2)
                        {
                            if (!fileName.endsWith(".hdr"))
                                fileName.append(".hdr");
                            BasicImageLoader::SaveHDR(image.get(), fileName.toStdString().c_str());
                        }
                        else if (selectedType == 3)
                        {
                            if (!fileName.endsWith(".dds"))
                                fileName.append(".dds");
                            if (image->getWidth() % 4 || image->getHeight() % 4)
                            {
                                //TODO! show error dialog
                                Global_ShowErrorMessage(Localizer::Translate("Unable to generate DDS image"), Localizer::Translate("DDS textures must have dimensions that are a multiple of 4 pixels in size."));
                                return;
                            }
                            BasicImageLoader::SaveDDS(image.get(), fileName.toStdString().c_str());
                        }
                        else
                        {
                            // Just fall back on spitting out a PNG
                            fileName.append(".png");
                            BasicImageLoader::SavePNG(image.get(), fileName.toStdString().c_str());
                        }
                    }
                }
            });
            menu->addAction(refresh);
            menu->addAction(exportAct);
            menu->addSeparator();
            menu->addAction(standardActions[1]);
            menu->addSeparator();
            QAction* deleteAction = new QAction("Delete");
            connect(deleteAction, &QAction::triggered, [=](bool) {
                view->scene()->removeItem(item);
                delete item;
            });
            menu->addAction(deleteAction);

            return menu;
        }
    }
    else
    {
        QAction* createBlendNodeAction = new QAction("Blend");
        connect(createBlendNodeAction, &QAction::triggered, [=](bool) {
            if (graph_)
            {
                if (GraphNode* graphNode = Context::GetInstance()->Create<GraphNode>("BlendNode"))
                {
                    graphNode->name = "Blend";
                    graphNode->Construct();
                    graphNode->XPos = pt.x();
                    graphNode->YPos = pt.y();
                    graph_->AddNode(graphNode, false);
                    GraphCommands::CreateCmd* cmd = new GraphCommands::CreateCmd(graph_, std::make_shared<GraphNodeDataSource>(graphNode), false);
                    GetUndoStack()->Push(cmd);
                    //document_->CreateBlockFor(graphNode)->setPos(pt);
                }
            }
        });

        menu->addAction(createBlendNodeAction);
        QMenu* values = menu->addMenu("Values");
        FillMenu(values, "Value", pt);
        QMenu* generators = menu->addMenu("Generators");
        FillMenu(generators, "Generator", pt);
        QMenu* math = menu->addMenu("Math");
        FillMenu(math, "Math", pt);
        QMenu* color = menu->addMenu("Color Manip");
        FillMenu(color, "Color", pt);
        QMenu* filters = menu->addMenu("Filters");
        FillMenu(filters, "Filter", pt);
        QMenu* normals = menu->addMenu("Normals");
        FillMenu(normals, "Normal Maps", pt);
        QMenu* bakers = menu->addMenu("Mesh Bakers");
        FillMenu(bakers, "Bakers", pt);

        QMenu* outputs = menu->addMenu("Outputs");
        AddOutputMenu(outputs, "Albedo", pt, TexGenOutputFormat::TGOF_RGB, TexGenOutputType::TGOT_Albedo);
        AddOutputMenu(outputs, "Roughness", pt, TexGenOutputFormat::TGOF_Alpha, TexGenOutputType::TGOT_Albedo);
        AddOutputMenu(outputs, "Metallic", pt, TexGenOutputFormat::TGOF_Alpha, TexGenOutputType::TGOT_Albedo);
        AddOutputMenu(outputs, "Normal", pt, TexGenOutputFormat::TGOF_RGB, TexGenOutputType::TGOT_Normal);
        AddOutputMenu(outputs, "Height", pt, TexGenOutputFormat::TGOF_Alpha, TexGenOutputType::TGOT_Height);
        AddOutputMenu(outputs, "Surface Thickness", pt, TexGenOutputFormat::TGOF_Alpha, TexGenOutputType::TGOT_SurfaceThickness);
        AddOutputMenu(outputs, "Subsurface Color", pt, TexGenOutputFormat::TGOF_RGBA, TexGenOutputType::TGOT_Subsurface);
        AddOutputMenu(outputs, "Custom", pt, TexGenOutputFormat::TGOF_RGBA, TexGenOutputType::TGOT_Custom);
    }
    return menu;
}

std::vector<QAction*> TextureDocument::CreateNodeActions(QGraphicsView* view)
{
    std::vector<QAction*> actions;
    if (!graph_)
        return actions;
    FillList(view, actions, "SPECIAL");
    FillList(view, actions, "Value");
    FillList(view, actions, "Generator");
    FillList(view, actions, "Math");
    FillList(view, actions, "Color");
    FillList(view, actions, "Filter");
    FillList(view, actions, "Normal Maps");
    FillList(view, actions, "Bakers");
    return actions;
}

void TextureDocument::ConnectionFormed(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to)
{
    DO_PREVIEW_UPDATE(to->node);
}

void TextureDocument::ConnectionDestroyed(SprueEngine::GraphSocket* from, SprueEngine::GraphSocket* to)
{
    DO_PREVIEW_UPDATE(to->node);
}

void TextureDocument::FillList(QGraphicsView* view, std::vector<QAction*>& actionsList, const char* forItems)
{
    std::vector<TexGenData::NodeNameRecord> found = TexGenData::GetSortedNodeRecords(forItems);

    for (auto record : found)
    {
        QAction* action = new QAction(record.second.first);
        action->connect(action, &QAction::triggered, [=](bool) {
            if (!graph_)
                return;
            if (GraphNode* graphNode = SprueEngine::Context::GetInstance()->Create<SprueEngine::GraphNode>(record.first))
            {
                graphNode->Construct();
                graphNode->name = record.second.first;
                auto pos = view->mapToScene(view->viewport()->rect().center());
                graphNode->XPos = pos.x();
                graphNode->YPos = pos.y();
                graph_->AddNode(graphNode, false);
                GraphCommands::CreateCmd* cmd = new GraphCommands::CreateCmd(graph_, std::make_shared<GraphNodeDataSource>(graphNode), graph_->IsEntryNode(graphNode));
                GetUndoStack()->Push(cmd);
                //CreateBlockFor(graphNode)->setPos(this->view_->mapToScene(this->view_->viewport()->rect().center()));
            }
        });
        actionsList.push_back(action);
    }
}

void TextureDocument::FillMenu(QMenu* menu, const char* forItems, QPointF pos)
{
    std::vector<TexGenData::NodeNameRecord> found = TexGenData::GetSortedNodeRecords(forItems);

    for (auto record : found)
    {
        QAction* action = new QAction(record.second.first);
        action->setIcon(TexGenData::GetIconFor(record.second.typeName_));
        action->connect(action, &QAction::triggered, [=](bool) {
            if (!graph_)
                return;
            if (GraphNode* graphNode = Context::GetInstance()->Create<GraphNode>(record.first))
            {
                graphNode->Construct();
                graphNode->name = record.second.first;
                graphNode->XPos = pos.x();
                graphNode->YPos = pos.y();
                graph_->AddNode(graphNode, false);
                GraphCommands::CreateCmd* cmd = new GraphCommands::CreateCmd(graph_, std::make_shared<GraphNodeDataSource>(graphNode), false);
                GetUndoStack()->Push(cmd);
                //document_->CreateBlockFor(graphNode)->setPos(pos);
            }
        });
        menu->addAction(action);
    }
}

void TextureDocument::AddOutputMenu(QMenu* menu, const char* name, QPointF pos, SprueEngine::TexGenOutputFormat fmt, SprueEngine::TexGenOutputType type)
{
    QAction* action = new QAction(name);
    connect(action, &QAction::triggered, [=](bool) {
        if (!graph_)
            return;
        if (TextureOutputNode* graphNode = Context::GetInstance()->Create<TextureOutputNode>("TextureOutputNode"))
        {
            graphNode->Construct();
            graphNode->name = name;
            graphNode->Format = fmt;
            graphNode->OutputType = type;

            graph_->AddNode(graphNode, false);
            // Needs to be an entry node so that we know to process it
            graph_->AddEntryNode(graphNode);
            GraphCommands::CreateCmd* cmd = new GraphCommands::CreateCmd(graph_, std::make_shared<GraphNodeDataSource>(graphNode), true);
            GetUndoStack()->Push(cmd);
            //QNEBlock* block = document_->CreateBlockFor(graphNode);
            //block->setPos(pos);
            //block->setBackgroundColor(QColor(40, 120, 40));
        }
    });

    menu->addAction(action);
}

int TextureDocument::GetExportWorkCount() const
{
    if (!graph_)
        return 0;
    return graph_->GetEntryNodes().size();
}

bool TextureDocument::WriteTextures(const QString& path, const QString& namingConvention, int index, int format)
{
    if (!graph_)
        return false;

    static const char* OutputNames[] = {
        "Albedo",
        "Roughness",
        "Glossiness",
        "Metallic",
        "Normal",
        "Specular",
        "SurfaceThickness",
        "Subsurface",
        "Height",
        "Custom",
    };

    static const char* ShortNames[] = {
        "c",
        "r",
        "g",
        "m",
        "n",
        "s",
        "ss",
        "ssc",
        "h",
        "cust",
    };

    static const char* formatExt[] = {
        ".png",
        ".tga",
        ".hdr",
        ".dds"
    };

    auto entryNodes = graph_->GetEntryNodes();
    if (entryNodes.size() > index)
    {
        if (auto outputNode = dynamic_cast<TextureOutputNode*>(entryNodes[index]))
        {
            auto texture = outputNode->GetPreview(outputNode->Width, outputNode->Height);
            QDir dir(path);
            QString filePath = dir.filePath(namingConvention.arg(outputNode->name.c_str(), OutputNames[outputNode->OutputType]) + formatExt[format]);
            if (format == 0)
                SprueEngine::BasicImageLoader::SavePNG(texture.get(), filePath.toStdString().c_str());
            else if (format == 1)
                SprueEngine::BasicImageLoader::SaveTGA(texture.get(), filePath.toStdString().c_str());
            else if (format == 2)
                SprueEngine::BasicImageLoader::SaveHDR(texture.get(), filePath.toStdString().c_str());
            else if (format == 3)
            {
                if (texture->getWidth() % 4 || texture->getHeight() % 4)
                {
                    Global_ShowErrorMessage(Localizer::Translate("Unable to generate DDS image"), Localizer::Translate("DDS textures must have dimensions that are a multiple of 4 pixels in size."));
                    return false;
                }
                SprueEngine::BasicImageLoader::SaveDDS(texture.get(), filePath.toStdString().c_str());
            }
            else
                SprueEngine::BasicImageLoader::SavePNG(texture.get(), filePath.toStdString().c_str());
        }
        return true;
    }
    return false;
}

void TextureDocument::SetEnableHeightShader(bool state)
{
    showHeight_ = state;
    if (activeView_)
        ((TexturePreviewView*)activeView_.get())->UpdateMaterial();
}

void TextureDocument::SetGlossMode(bool gloss)
{
    glossMode_ = gloss;
    if (activeView_)
        ((TexturePreviewView*)activeView_.get())->UpdateMaterial();
}

}