#pragma once

#include <EditorLib/DataSource.h>
#include <EditorLib/Controls/DataGridWidget.h>

namespace SprueEngine
{
    class IEditable;
    class Graph;
    class GraphNode;
    class SceneObject;
}

namespace SprueEditor
{

    class IEditableDataSource : public DataSource
    {
    public:
        IEditableDataSource(SprueEngine::IEditable* editable, std::shared_ptr<DataSource> parent = std::shared_ptr<DataSource>());

        virtual int GetID() const override { return 1; }
        virtual void Destroy() override;

        virtual SprueEngine::IEditable* GetEditable() { return editable_; }

        virtual QString GetName() const override;
        virtual QString GetTypeName() const override;

        virtual std::vector<QString> EnumerateFields() override;
        virtual bool CanShow(const QString& column) override;
        virtual bool CanSetText(const QString& column) override;
        virtual void FromText(const QString& column, const QString& text) override;
        virtual QString ToText(const QString& column) override;
        virtual QVariant ToVariant(const QString& column) override;
        virtual void FromVariant(const QString& column, const QVariant& var) override;

    protected:
        virtual bool equals(const DataSource* rhs) const override;

        SprueEngine::IEditable* editable_;
    };

    class SceneObjectDataSource : public IEditableDataSource
    {
    public:
        SceneObjectDataSource(SprueEngine::SceneObject* sceneObject);

        virtual int GetID() const override { return 2; }

        virtual SprueEngine::IEditable* GetEditable() { return editable_; }
        virtual SprueEngine::SceneObject* GetSceneObject() const;

        virtual QString GetName() const override;

    protected:
        SprueEngine::SceneObject* sceneObject_ = 0x0;
    };

    class GraphNodeDataSource : public IEditableDataSource
    {
    public:
        GraphNodeDataSource(SprueEngine::GraphNode* node);

        virtual int GetID() const override { return 7; }
        virtual SprueEngine::GraphNode* GetNode() const;

        virtual QString GetName() const override;
    };

    class GraphDataSource : public IEditableDataSource
    {
    public:
        GraphDataSource(SprueEngine::Graph* graph);

        virtual int GetID() const override { return 8; }
        virtual SprueEngine::Graph* GetGraph() const;

        virtual QString GetName() const override;
    };

}