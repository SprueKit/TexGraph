#include "TextureDocumentShelf.h"

#include "TextureDocument.h"

#include <QActionGroup>

#include "SprueKitEditor.h"
#include "UrhoHelpers.h"
#include "QtHelpers.h"
#include "Views/ViewBase.h"

#include <EditorLib/IM/QImGui.h>
#include <EditorLib/ThirdParty/QSexyToolTip.h>
#include <EditorLib/Selectron.h>

#include <QComboBox>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QToolButton>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Loaders/FBXLoader.h>
#include <SprueEngine/Loaders/OBJLoader.h>
#include <SprueEngine/ResourceStore.h>

#include <Urho3D/Graphics/CustomGeometry.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Graphics/Skybox.h>
#include <Urho3D/Graphics/StaticModel.h>
#include <Urho3D/Graphics/Technique.h>
#include <Urho3D/Graphics/TextureCube.h>
#include <Urho3D/Graphics/Zone.h>

#include "../Urho/Controls/UrhoIMPropertyEditor.h"

namespace SprueEditor
{
    struct TexturePreviewIBLMap {
        std::string name;
        std::string material;
        std::string texture;
        std::string ibl;
    };
    static const TexturePreviewIBLMap TexturePreviewIBLMaps[] = {
        { "Day", "IBL/Day/Material.xml", "IBL/Day/Cube.xml", "IBL/Day/IBL.xml" },
        { "Forest", "IBL/Forest/Material.xml", "IBL/Forest/Cube.xml", "IBL/Forest/IBL.xml" },
        { "Chapel", "IBL/Chapel/Material.xml", "IBL/Chapel/Cube.xml", "IBL/Chapel/IBL.xml" },
        { "City Night", "IBL/CityNight/Material.xml", "IBL/CityNight/Cube.xml", "IBL/CityNight/IBL.xml" },
        { "Sunset", "IBL/Sunset/Material.xml", "IBL/Sunset/Cube.xml", "IBL/Sunset/IBL.xml" },
        { "Overcast", "IBL/Overcast/Material.xml", "IBL/Overcast/Cube.xml", "IBL/Overcast/IBL.xml" },
        { "Winter", "IBL/Winter/Material.xml", "IBL/Winter/Cube.xml", "IBL/Winter/IBL.xml" },
    };
    static const int TexturePreviewIBLMapCount = 7;

    // TODO: get this from data
    static std::unordered_map<std::string, std::string> TextureDocumentShaderMap = {
        { "PBR Rough/Metal", "CoreData/Techniques/TexGenPBR.xml" },
        { "PBR Gloss/Spec", "CoreData/Techniques/TexGenPBR_Gloss.xml" },
    };
    static const int TextureDocumentShaderCount = 3;

    // TODO: get this from data
    static std::unordered_map<std::string, std::string> TextureDocumentPrimitives = {
        { "Cube", "Data/Models/Cube.mdl" },
        { "Sphere", "Data/Models/Sphere.mdl" },
        { "Cone", "Data/Models/Cone.mdl" },
        { "Plane", "Data/Models/Plane.mdl" },
        { "Teapot", "Data/Models/TeaPot.mdl" },
    };
    static const int TextureDocumentPrimitiveCount = 5;

    int AttributeIndex(Urho3D::Serializable* serial, const std::string& name)
    {
        auto attrs = serial->GetAttributes();
        if (attrs)
        {
            for (unsigned i = 0; i < attrs->Size(); ++i)
                if ((*attrs)[i].name_.Compare(name.c_str()) == 0)
                    return i;
        }
        return -1;
    }

    /// Widget containing the controls used in the shape picker flyout
    class TextureDocPreviewShapeFlyout : public QWidget
    {
    public:
        TextureDocPreviewShapeFlyout(QWidget* parent) : QWidget(parent)
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);

            mainLayout->addWidget(new QLabel("Primitives"));
            QHBoxLayout* prims = new QHBoxLayout();
            prims->addWidget(sphere_ = new QPushButton("Sphere"));
            prims->addWidget(box_ = new QPushButton("Box"));
            prims->addWidget(cone_ = new QPushButton("Cone"));
            prims->addWidget(plane_ = new QPushButton("Plane"));
            prims->addWidget(teapot_ = new QPushButton("Teapot"));
            mainLayout->addLayout(prims);

            mainLayout->addWidget(customModel_ = new QPushButton("Custom Model"));
            mainLayout->addWidget(currentFile_ = new QLabel("< custom model not loaded >"));

            sphere_->setCheckable(true);
            box_->setCheckable(true);
            cone_->setCheckable(true);
            plane_->setCheckable(true);
            teapot_->setCheckable(true);
            customModel_->setCheckable(true);
            sphere_->setChecked(true);
        }

        QPushButton* sphere_;
        QPushButton* box_;
        QPushButton* cone_;
        QPushButton* plane_;
        QPushButton* teapot_;
        QPushButton* customModel_;
        QLabel* currentFile_;
    };

    class TextureDocShadingFlyout : public QWidget
    {
    public:
        TextureDocShadingFlyout(QWidget* parent) : QWidget(parent)
        {
            QVBoxLayout* mainLayout = new QVBoxLayout(this);

            mainLayout->addWidget(new QLabel("Shading Method"));
            mainLayout->addWidget(shader_ = new QComboBox());
            mainLayout->addWidget(new QLabel("Image Based Lighting"));
            mainLayout->addWidget(ibl_ = new QComboBox());
        }

        QComboBox* shader_;
        QComboBox* ibl_;
    };

    TextureDocumentShelf::TextureDocumentShelf(TextureDocument* document) :
        document_(document)
    {
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->setMargin(0);
        layout->setAlignment(Qt::AlignLeft);

    // Primitives flyout
        {
            QToolButton* primsButton = new QToolButton();
            primsButton->setText("Shape");

            QSexyToolTip* primsWindow = new QSexyToolTip(SprueKitEditor::GetInstance());
            primsWindow->attach(primsButton);

            QVBoxLayout* flyoutLayout = new QVBoxLayout(primsWindow);
            TextureDocPreviewShapeFlyout* flyoutWidget = new TextureDocPreviewShapeFlyout(0x0);
            flyoutLayout->addWidget(flyoutWidget);
            primsWindow->setContentWidget(flyoutWidget);

            
#define SET_CHECK(WIDGET, STATE) WIDGET->blockSignals(true); WIDGET->setChecked(STATE); WIDGET->blockSignals(false)

            connect(flyoutWidget->sphere_, &QPushButton::clicked, [=]() {
                this->LoadPrimitive("Data/Models/Sphere.mdl");
                SET_CHECK(flyoutWidget->sphere_, true);
                SET_CHECK(flyoutWidget->box_, false);
                SET_CHECK(flyoutWidget->cone_, false);
                SET_CHECK(flyoutWidget->plane_, false);
                SET_CHECK(flyoutWidget->teapot_, false);
                SET_CHECK(flyoutWidget->customModel_, false);
                flyoutWidget->currentFile_->setText("< custom model not loaded >");
            });

            connect(flyoutWidget->box_, &QPushButton::clicked, [=]() {
                this->LoadPrimitive("Data/Models/Box.mdl");
                SET_CHECK(flyoutWidget->sphere_, false);
                SET_CHECK(flyoutWidget->box_, true);
                SET_CHECK(flyoutWidget->cone_, false);
                SET_CHECK(flyoutWidget->plane_, false);
                SET_CHECK(flyoutWidget->teapot_, false);
                SET_CHECK(flyoutWidget->customModel_, false);
                flyoutWidget->currentFile_->setText("< custom model not loaded >");
            });

            connect(flyoutWidget->cone_, &QPushButton::clicked, [=]() {
                this->LoadPrimitive("Data/Models/Cone.mdl");
                SET_CHECK(flyoutWidget->sphere_, false);
                SET_CHECK(flyoutWidget->box_, false);
                SET_CHECK(flyoutWidget->cone_, true);
                SET_CHECK(flyoutWidget->plane_, false);
                SET_CHECK(flyoutWidget->teapot_, false);
                SET_CHECK(flyoutWidget->customModel_, false);
                flyoutWidget->currentFile_->setText("< custom model not loaded >");
            });

            connect(flyoutWidget->plane_, &QPushButton::clicked, [=]() {
                this->LoadPrimitive("Data/Models/Plane.mdl");
                SET_CHECK(flyoutWidget->sphere_, false);
                SET_CHECK(flyoutWidget->box_, false);
                SET_CHECK(flyoutWidget->cone_, false);
                SET_CHECK(flyoutWidget->plane_, true);
                SET_CHECK(flyoutWidget->teapot_, false);
                SET_CHECK(flyoutWidget->customModel_, false);
                flyoutWidget->currentFile_->setText("< custom model not loaded >");
            });

            connect(flyoutWidget->teapot_, &QPushButton::clicked, [=]() {
                this->LoadPrimitive("Data/Models/TeaPot.mdl");
                SET_CHECK(flyoutWidget->sphere_, false);
                SET_CHECK(flyoutWidget->box_, false);
                SET_CHECK(flyoutWidget->cone_, false);
                SET_CHECK(flyoutWidget->plane_, false);
                SET_CHECK(flyoutWidget->teapot_, false);
                SET_CHECK(flyoutWidget->customModel_, false);
                flyoutWidget->currentFile_->setText("< custom model not loaded >");
            });

            connect(flyoutWidget->customModel_, &QPushButton::clicked, [=]() {
                QString path;
                if (this->LoadCustom(path))
                {
                    flyoutWidget->currentFile_->setText(path);
                    SET_CHECK(flyoutWidget->sphere_, false);
                    SET_CHECK(flyoutWidget->box_, false);
                    SET_CHECK(flyoutWidget->cone_, false);
                    SET_CHECK(flyoutWidget->plane_, false);
                    SET_CHECK(flyoutWidget->teapot_, false);
                    SET_CHECK(flyoutWidget->customModel_, true);
                }
                else
                {
                    flyoutWidget->currentFile_->setText("< custom model not loaded >");
                    SET_CHECK(flyoutWidget->customModel_, false);
                }
            });

#undef SET_CHECK

            layout->addWidget(primsButton);
        }

    // Shading Flyout
        {
            QToolButton* shadingButton = new QToolButton();
            shadingButton->setText("Shading");

            QSexyToolTip* shadingWindow = new QSexyToolTip(SprueKitEditor::GetInstance());
            shadingWindow->attach(shadingButton);

            QVBoxLayout* flyoutLayout = new QVBoxLayout(shadingWindow);
            TextureDocShadingFlyout* flyoutWidget = new TextureDocShadingFlyout(0x0);
            flyoutLayout->addWidget(flyoutWidget);
            shadingWindow->setContentWidget(flyoutWidget);

            flyoutWidget->shader_->setFont(QFont("Arial", 10));
            for (auto item : TextureDocumentShaderMap)
                flyoutWidget->shader_->addItem(item.first.c_str());

            //connect(flyoutWidget->shader_, &QComboBox::currentTextChanged, this, &TextureDocumentShelf::ShaderChanged);
            connect(flyoutWidget->shader_, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int idx) {
                document->SetGlossMode(idx != 0);

            });

            flyoutWidget->ibl_->setFont(QFont("Arial", 10));
            for (unsigned int i = 0; i < TexturePreviewIBLMapCount; ++i)
                flyoutWidget->ibl_->addItem(TexturePreviewIBLMaps[i].name.c_str(), QString(TexturePreviewIBLMaps[i].material.c_str()));
            flyoutWidget->ibl_->setCurrentIndex(0);
            connect(flyoutWidget->ibl_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TextureDocumentShelf::CubeMapChanged);

            layout->addWidget(shadingButton);
        }

    // Tiling
        layout->addWidget(new QLabel("Tile"));
        QDoubleSpinBox* uTile = new QDoubleSpinBox();
        uTile->setSingleStep(0.25);
        uTile->setMinimum(0.1);
        uTile->setMaximum(200);
        QDoubleSpinBox* vTile = new QDoubleSpinBox();
        vTile->setSingleStep(0.25);
        vTile->setMinimum(0.1);
        vTile->setMaximum(200);
        uTile->setValue(1.0);
        vTile->setValue(1.0);
        layout->addWidget(uTile);
        layout->addWidget(vTile);

        connect(uTile, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TextureDocumentShelf::UTileChanged);
        connect(vTile, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &TextureDocumentShelf::VTileChanged);

    // Flyout menu
        QToolButton* transformButton = new QToolButton();
        transformButton->setIcon(QIcon(":/Images/godot/icon_matrix.png"));
        transformButton->setText("T");
        layout->addWidget(transformButton);
        transformButton->setCheckable(true);

    // Transform flyout
        auto transWindow = new UrhoEditor::UrhoIMPropertyEditor({ "Name", "Position","Rotation","Scale","Is Enabled" });
        transformFlyout_ = QImGui::WrapInPopup(transWindow, SprueKitEditor::GetInstance());
        transWindow->SetSelectron(document->GetSelectron());
        transWindow->SetAutoFit(true);
        transWindow->setMinimumSize(300, 300);
        transformFlyout_->attach(transformButton);

        layout->addItem(new QSpacerItem(1, 0, QSizePolicy::Policy::Expanding));

        QToolButton* helpButton_ = new QToolButton();
        helpButton_->setIcon(QIcon(":/qss_icons/rc/help.png"));
        layout->addWidget(helpButton_, 0, Qt::AlignRight);
        QtHelpers::AttachHelpFlyout(helpButton_, "VIEW_TEXTURE_DOCUMENT");
    }

    TextureDocumentShelf::~TextureDocumentShelf()
    {
    }

    void TextureDocumentShelf::ShaderChanged(const QString& text)
    {
        auto material = document_->GetMaterial();
        auto found = TextureDocumentShaderMap.find(text.toStdString());
        if (found != TextureDocumentShaderMap.end())
        {
            std::string techniquePath = found->second;
            auto resourceCache = document_->GetActiveView()->GetContext()->GetSubsystem<Urho3D::ResourceCache>();
            if (auto technique = resourceCache->GetResource<Urho3D::Technique>(techniquePath.c_str()))
                document_->GetMaterial()->SetTechnique(0, technique);
        }
    }

    void TextureDocumentShelf::CubeMapChanged(int index)
    {
        if (document_->GetActiveView())
        {
            if (auto scene = document_->GetActiveView()->GetScene())
            {
                if (auto comp = scene->GetComponent<Urho3D::Zone>())
                    comp->SetZoneTexture(comp->GetContext()->GetSubsystem<Urho3D::ResourceCache>()->GetResource<Urho3D::TextureCube>(TexturePreviewIBLMaps[index].ibl.c_str()));
                if (auto node = scene->GetChild("SKYBOX", true))
                {
                    if (auto mesh = node->GetComponent<Urho3D::Skybox>())
                        mesh->SetMaterial(mesh->GetContext()->GetSubsystem<Urho3D::ResourceCache>()->GetResource<Urho3D::Material>(TexturePreviewIBLMaps[index].material.c_str()));
                }
            }
        }
    }

    void TextureDocumentShelf::UTileChanged(double value)
    {
        if (document_)
            document_->GetMaterial()->SetShaderParameter("UOffset", Urho3D::Vector4(value, 0, 0, 0));
    }

    void TextureDocumentShelf::VTileChanged(double value)
    {
        if (document_)
            document_->GetMaterial()->SetShaderParameter("VOffset", Urho3D::Vector4(0, value, 0, 0));
    }

    void TextureDocumentShelf::LoadPrimitive(const char* text)
    {
        auto meshNode = document_->GetActiveView()->GetScene()->GetChild("PREVIEW_MESH");
        auto modelComp = meshNode->GetOrCreateComponent<Urho3D::StaticModel>();

        meshNode->RemoveComponent<Urho3D::CustomGeometry>();
        auto resource = modelComp->GetContext()->GetSubsystem<Urho3D::ResourceCache>();
        modelComp->SetModel(resource->GetResource<Urho3D::Model>(text));

        auto mats = modelComp->GetMaterialsAttr();
        for (unsigned i = 0; i < mats.names_.Size(); ++i)
            modelComp->SetMaterial(i, document_->GetMaterial());
    }

    bool TextureDocumentShelf::LoadCustom(QString& path)
    {
        auto meshNode = document_->GetActiveView()->GetScene()->GetChild("PREVIEW_MESH");
        auto modelComp = meshNode->GetOrCreateComponent<Urho3D::StaticModel>();

        QString primPath = QFileDialog::getOpenFileName(0x0, "Select Model", QString(), "Model Files (*.fbx *.obj);;FBX Model (*.fbx);;OBJ Model (*.obj)");
        bool modelLoaded = false;
        if (!primPath.isEmpty())
        {
            if (auto loader = SprueEngine::Context::GetInstance()->GetResourceLoader("Mesh", primPath.toStdString().c_str()))
            {
                if (auto loaded = loader->LoadResource(primPath.toStdString().c_str()))
                {
                    if (auto meshResource = std::dynamic_pointer_cast<SprueEngine::MeshResource>(loaded))
                    {
                        meshNode->RemoveComponent<Urho3D::StaticModel>();
                        auto dynamicMesh = meshNode->GetOrCreateComponent<Urho3D::CustomGeometry>();
                        ConvertGeometry(dynamicMesh, meshResource.get());
                        modelLoaded = true;

                        auto mats = dynamicMesh->GetMaterialsAttr();
                        for (unsigned i = 0; i < mats.names_.Size(); ++i)
                            dynamicMesh->SetMaterial(i, document_->GetMaterial());
                    }
                }
            }
        }
        if (!modelLoaded)
            return false;

        path = primPath;
        return true;
    }
}
