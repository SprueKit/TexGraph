#include "UrhoIMPropertyEditor.h"

#include "../../../GlobalAccess.h"
#include "../../../Data/UrhoDataSources.h"
#include "../../../Commands/Urho/UrhoPropertyCmd.h"
#include "../../../Commands/Urho/UrhoSceneCmds.h"

#include <QFileDialog>

#include <EditorLib/ThirdParty/imgui_custom.h>
#include <EditorLib/ThirdParty/imgui_internal.h>

using namespace Urho3D;

namespace UrhoEditor
{
    static std::map<Urho3D::StringHash, std::string> UrhoResourceToFileType = {
        std::make_pair("LuaScript",     "Lua Script files (*.lua)"),
        std::make_pair("Material",      "Materials (*.xml)"),
        std::make_pair("Model",         "Models (*.mdl)"),
        std::make_pair("ParticleEffect","Particle effects (*.xml)"),
        std::make_pair("ScriptFile",    "Angelscript files (*.as)"),
        std::make_pair("Sound",         "Sound files (*.wav)"),
        std::make_pair("Texture",       "Textures (*.xml, *.png, *.jpg, *.jpeg, *.tga)"),
        std::make_pair("Texture2D",     "2D Textures (*.xml, *.png, *.jpg, *.jpeg, *.tga, *.dds)"),
        std::make_pair("Texture3D",     "3D Textures (*.xml, *.dds)"),
        std::make_pair("TextureCube",   "Textures (*.xml, *.dds)"),
        std::make_pair("TextureArray",  "Texture Arrays (*.xml, *.dds)"),
        std::make_pair("XMLFile",       "XML files(*.xml)"),
    };


    UrhoIMPropertyEditor::UrhoIMPropertyEditor()
    {

    }

    UrhoIMPropertyEditor::UrhoIMPropertyEditor(std::set<Urho3D::StringHash> filters) :
        filters_(filters)
    {

    }

    static ImTextureID resetValueTex = 0x0;
    static ImTextureID addTex = 0x0;
    static ImTextureID deleteTex = 0x0;

    void UrhoIMPropertyEditor::GenerateUI(QPainter*, const QSize& size)
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
        ImGui::SetNextWindowSize(ImVec2(size.width(), size.height()), ImGuiSetCond_Always);
        ImGui::Begin("Another Window", 0x0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

        // grab our static textures
        if (IsFirstRun())
        {
            static QPixmap resetValueIcon = QIcon(":/Images/fugue/reset.png").pixmap(16);
            static QPixmap addIcon = QIcon(":/Images/godot/icon_add.png").pixmap(32);
            static QPixmap deleteIcon = QIcon(":/Images/godot/icon_remove.png").pixmap(32);

            resetValueTex = RegisterTexture(&resetValueIcon);
            addTex = RegisterTexture(&addIcon);
            deleteTex = RegisterTexture(&deleteIcon);
        }

        if (auto mostRecentDataSource = GetSelectron()->GetMostRecentSelected<SerializableDataSource>())
        {
            if (auto serializable = mostRecentDataSource->GetObject())
            {
                ImGui::PushID(serializable);
                PrintSerializable(serializable, 0);
                ImGui::PopID();
            }
        }
        else
            ImGui::TextWrapped("No selected object to display properties for.");

        ImGui::End();
    }

    void UrhoIMPropertyEditor::PrintSerializable(Urho3D::Serializable* serializable, int depth)
    {
        auto attributeInfos = serializable->GetAttributes();
        if (!attributeInfos || attributeInfos->Empty())
        {
            ImGui::TextWrapped("Selected object has no editable properties.");
            return;
        }
        for (unsigned attrIndex = 0; attrIndex < attributeInfos->Size(); ++attrIndex)
        {
            auto& attr = (*attributeInfos)[attrIndex];
                    
            // Don't show no-edit fields
            if (attr.mode_ & AM_NOEDIT)
                continue;

            // Check for filters
            if (filters_.size())
            {
                if (filters_.find(attr.name_) == filters_.end())
                    continue;
            }

            Variant defVal = serializable->GetAttributeDefault(attrIndex);
            Variant value = serializable->GetAttribute(attrIndex);
            if (value.IsEmpty())
                continue;

            ImGui::PushItemWidth(-1);

            ImVec4 lblColor;
            if (value == defVal)
                lblColor = { 1,1,1,1 };
            else
                lblColor = { 0.39f,1.0f,0.39f,1.0f };

            const std::string withoutLabel = std::string("##") + attr.name_.CString();
            const char* labelLessName = withoutLabel.c_str();

// Emit label, without a macro this would be massively redundant and bloat things more than necessary
#define COMMON_LABEL() ImGui::TextColored(lblColor, attr.name_.CString()); ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - 16); \
            ImGui::PushID(&attr); \
            if (ImGui::ImageButton(resetValueTex, { 16,16 })) {  PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, defVal, value)); } \
            ImGui::PopID();

            if (attr.type_ == VAR_BOOL)
            {
                bool val = value.GetBool();
                if (ImGui::Checkbox(labelLessName, &val))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                ImGui::SameLine();
                COMMON_LABEL();
            }
            else if (attr.type_ == VAR_INT)
            {
                // Check first for a mask field
                if (attr.name_.Contains("Mask") || attr.name_.Contains("Flags"))
                {
                    COMMON_LABEL();
                    int val = value.GetInt();
                    unsigned hoverBit = 0;
                    if (ImGui::Bitfield(labelLessName, (unsigned*)(&val), &hoverBit))
                        PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                }
                else if (attr.enumNames_) // do we have enum names?
                {
                    int ct = 0;
                    for (; attr.enumNames_[ct] != 0x0; ++ct);

                    COMMON_LABEL();
                    int val = value.GetInt();
                    if (ImGui::Combo(labelLessName, &val, attr.enumNames_, ct, -1))
                        PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                }
                else if (attr.mode_ & AM_NODEID)
                {
                    // TODO: add some sort of picker ... autocomplete combobox?
                    COMMON_LABEL();
                    int val = value.GetInt();
                    if (ImGui::DragInt(labelLessName, &val, 1.0f, 0, INT_MAX))
                        PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                }
                else if (attr.mode_ & AM_COMPONENTID)
                {
                    // TODO: add some sort of picker ... autocomplete combobox
                    COMMON_LABEL();
                    int val = value.GetInt();
                    if (ImGui::DragInt(labelLessName, &val, 1.0f, 0, INT_MAX))
                        PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                }
                else
                {
                    COMMON_LABEL();
                    int val = value.GetInt();
                    if (ImGui::DragInt(labelLessName, &val))
                        PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                }
            }
            else if (attr.type_ == VAR_FLOAT)
            {
                COMMON_LABEL();
                float val = value.GetFloat();
                if (ImGui::DragFloat(labelLessName, &val))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_DOUBLE)
            {
                COMMON_LABEL();
                float val = value.GetDouble();
                if (ImGui::DragFloat(labelLessName, &val))
                {
                    double v = val;
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                }
            }
            else if (attr.type_ == VAR_STRING)
            {
                COMMON_LABEL();
                std::string str = value.GetString().CString();
                if (EditString(labelLessName, str))
                {
                    value = str.c_str();
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, (String)str.c_str(), value));
                }
            }
            else if (attr.type_ == VAR_STRINGVECTOR)
            {
                ImGui::PushID(&attr.name_);
                {
                    StringVector vec = value.GetStringVector();
                    COMMON_LABEL();
                    ImGui::BeginChild(ImGui::GetID(&attr), { ImGui::GetWindowContentRegionWidth() - 32, ImGui::GetFontSize() * 8 }, true, ImGuiWindowFlags_HorizontalScrollbar);
                    {
                        for (unsigned i = 0; i < vec.Size(); ++i)
                        {
                            ImGui::PushID(i);
                            {
                                std::string text = vec[i].CString();
                                ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() - 32);
                                if (EditString("##v", text))
                                {
                                    String str = text.c_str();
                                    vec[i] = str;
                                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, vec, value));
                                }
                                ImGui::SameLine();
                                if (ImGui::ImageButton(deleteTex, { 16,16 }))
                                {
                                    vec.Erase(i, 1);
                                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, vec, value));
                                }
                            }
                            ImGui::PopID();
                        }
                        ImGui::EndChild();
                    }
                    ImGui::SameLine();
                    if (ImGui::ImageButton(addTex, { 16,16 }))
                    {
                        vec.Push("<new string>");
                        PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, vec, value));
                    }
                }
                ImGui::PopID();
            }
            else if (attr.type_ == VAR_INTVECTOR2)
            {
                COMMON_LABEL();
                IntVector2 val = value.GetIntVector2();
                if (ImGui::DragInt2(labelLessName, &val.x_, 1, 0, 0, "%.3f", ImGuiInputTextFlags_Colorize))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_INTRECT)
            {
                COMMON_LABEL();
                IntRect val = value.GetIntRect();
                if (ImGui::DragInt4(labelLessName, &val.left_))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_VECTOR2)
            {
                COMMON_LABEL();
                Vector2 val = value.GetVector2();
                if (ImGui::DragFloat2(labelLessName, &val.x_, 1, 0, 0, "%.3f", 1, ImGuiInputTextFlags_Colorize))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_VECTOR3)
            {
                COMMON_LABEL();
                Vector3 val = value.GetVector3();
                if (ImGui::DragFloat3(labelLessName, &val.x_, 1, 0, 0, "%.3f", 1, ImGuiInputTextFlags_Colorize))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_VECTOR4)
            {
                COMMON_LABEL();
                Vector4 val = value.GetVector4();
                if (ImGui::DragFloat4(labelLessName, &val.x_, 1, 0, 0, "%.3f", 1, ImGuiInputTextFlags_Colorize))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_QUATERNION)
            {
                COMMON_LABEL();
                Quaternion val = value.GetQuaternion();
                Vector3 euler = val.EulerAngles();
                if (ImGui::DragFloat3(labelLessName, &euler.x_, 1, 0, 0, "%.3f", 1, ImGuiInputTextFlags_Colorize))
                {
                    val.FromEulerAngles(euler.x_, euler.y_, euler.z_);
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                }
            }
            else if (attr.type_ == VAR_MATRIX3)
            {
                COMMON_LABEL();
                Matrix3 val = value.GetMatrix3();
                bool anyChanged = ImGui::DragFloat3("##a", &val.m00_);
                anyChanged |= ImGui::DragFloat3("##b", &val.m10_);
                anyChanged |= ImGui::DragFloat3("##c", &val.m20_);
                if (anyChanged)
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_MATRIX3X4)
            {
                COMMON_LABEL();
                ImGui::PushID(&attr.name_);
                Matrix3x4 val = value.GetMatrix3x4();
                bool anyChanged = ImGui::DragFloat4("##a", &val.m00_);
                anyChanged |= ImGui::DragFloat4("##b", &val.m10_);
                anyChanged |= ImGui::DragFloat4("##c", &val.m20_);
                if (anyChanged)
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                ImGui::PopID();
            }
            else if (attr.type_ == VAR_MATRIX4)
            {
                COMMON_LABEL();
                ImGui::PushID(&attr.name_);
                Matrix4 val = value.GetMatrix4();
                bool anyChanged = ImGui::DragFloat4("##a", &val.m00_);
                anyChanged |= ImGui::DragFloat4("##b", &val.m10_);
                anyChanged |= ImGui::DragFloat4("##c", &val.m20_);
                anyChanged |= ImGui::DragFloat4("##d", &val.m30_);
                if (anyChanged)
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
                ImGui::PopID();
            }
            else if (attr.type_ == VAR_COLOR)
            {
                COMMON_LABEL();
                Color val = value.GetColor();
                if (ImGui::ColorEdit4(labelLessName, &val.r_, true))
                    PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, val, value));
            }
            else if (attr.type_ == VAR_VARIANTVECTOR)
            {
                COMMON_LABEL();
                if (attr.mode_ & AM_NODEIDVECTOR)
                {
                    // Node identifiers
                }
                else
                {
                    // standard variant vector???
                }
            }
            else if (attr.type_ == VAR_VARIANTMAP)
            {
                COMMON_LABEL();
            }
            else if (attr.type_ == VAR_BUFFER)
            {
                COMMON_LABEL();
            }
            else if (attr.type_ == VAR_RESOURCEREF)
            {
                ResourceRef val = value.GetResourceRef();
                ResourceRef referenceVal = defVal.GetResourceRef();

                COMMON_LABEL();

                float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                float width = ImGui::CalcItemWidth();

                auto win = ImGui::GetCurrentWindow();
                win->DC.ItemWidthStack.push_back(20);
                win->DC.ItemWidthStack.push_back(width - 20 - spacing * 2);
                win->DC.ItemWidth = win->DC.ItemWidthStack.back();
                
                ImGui::InputText(labelLessName, (char*)val.name_.CString(), val.name_.Length(), ImGuiInputTextFlags_ReadOnly);
                ImGui::PopItemWidth();
                ImGui::SameLine(0, spacing);
                if (ImGui::Button(("...##" + std::string(labelLessName)).c_str()))
                {
                    auto found = UrhoResourceToFileType.find(referenceVal.type_);
                    PushDeferredCall([=]() {
                        QString fileName = QFileDialog::getOpenFileName(0x0, "Select file...", QString(), found->second.c_str());
                        if (!fileName.isEmpty())
                        {
                            ResourceRef newHandle = referenceVal;
                            newHandle.name_ = fileName.toUtf8().constData();
                            PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, newHandle, value));
                        }
                    });
                }
                ImGui::PopItemWidth();
            }
            else if (attr.type_ == VAR_RESOURCEREFLIST)
            {
                ImGui::PushID(&attr.name_);
                {
                    ResourceRefList vec = value.GetResourceRefList();
                    ResourceRefList refVec = defVal.GetResourceRefList();

                    COMMON_LABEL();
                    ImGui::BeginChild(ImGui::GetID(&attr), { ImGui::GetContentRegionAvailWidth(), ImGui::GetFontSize() * 8 }, true, ImGuiWindowFlags_HorizontalScrollbar);
                    {
                        for (unsigned i = 0; i < vec.names_.Size(); ++i)
                        {
                            ImGui::PushID(i);
                            {
                                std::string text = vec.names_[i].CString();
                                ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() - 40);
                                ImGui::InputText(labelLessName, (char*)vec.names_[i].CString(), vec.names_[i].Length(), ImGuiInputTextFlags_ReadOnly);

                            // Browse button
                                ImGui::SameLine();
                                if (ImGui::Button("..."))
                                {
                                    auto foundType = UrhoResourceToFileType.find(refVec.type_);
                                    if (foundType != UrhoResourceToFileType.end())
                                    {
                                        PushDeferredCall([=]() {
                                            QString fileName = QFileDialog::getOpenFileName(0x0, "Select file...", QString(), foundType->second.c_str());
                                            if (!fileName.isEmpty())
                                            {
                                                // const-casting around vec seemed to work ... but screams undefined behaviour
                                                ResourceRefList safeCpy = vec;
                                                safeCpy.names_[i] = String(fileName.toUtf8().constData());
                                                PUSH_DOCUMENT_UNDO(new UrhoPropertyCmd(std::make_shared<SerializableDataSource>(serializable), attrIndex, safeCpy, value));
                                            }
                                        });
                                    }
                                }
                            }
                            ImGui::PopID();
                        }
                        ImGui::EndChild();
                    }
                }
                ImGui::PopID();
            }
        }

        // if we have filters then do not go deeper
        if (filters_.size())
            return;

        if (auto node = dynamic_cast<Urho3D::Node*>(serializable))
        {
            auto comps = node->GetComponents();

            for (unsigned i = 0; i < comps.Size(); ++i)
            {
                ImGui::PushID(comps[i].Get());
                bool exists = true;
                bool open = ImGui::CollapsingHeader(comps[i]->GetTypeName().CString(), &exists);
                if (open)
                {
                    ImGui::Indent();
                    PrintSerializable(comps[i], depth+1);
                    ImGui::Unindent();
                }

                if (!exists)
                    PUSH_DOCUMENT_UNDO(new SceneCommands::DeleteComponentCmd(node, std::make_shared<ComponentDataSource>(comps[i])));
                ImGui::PopID();
            }
        }
    }

    void UrhoIMPropertyEditor::Link(Selectron* sel)
    {

    }    
}