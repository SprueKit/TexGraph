#pragma once

#include <EditorLib/TaskProcessor.h>

#include <SprueEngine/TextureGen/TextureNode.h>
#include <SprueEngine/FString.h>

#include <QImage>

#include <memory>

namespace SprueEditor
{

class TextureGenTask : public Task
{
public:
    TextureGenTask(SprueEngine::Graph* graph, SprueEngine::GraphNode* node, DocumentBase* doc);
    virtual ~TextureGenTask();

    virtual QString GetName() const override { return QString("Generating %1").arg(nodeName_.c_str()); }
    virtual void PrepareTask() override;
    virtual bool ExecuteTask() override;
    virtual void FinishTask() override;
    virtual bool Supercedes(Task* other) { 
        if (TextureGenTask* rhs = dynamic_cast<TextureGenTask*>(other)) 
            return rhs->nodeID_ == nodeID_;
        return false;
    }

    SprueEngine::GraphNode* GetNode() { return node_; }

    std::shared_ptr<QImage> GetGeneratedImage() { return generatedImage_; }

protected:
    std::shared_ptr<QImage> generatedImage_;
    std::shared_ptr<SprueEngine::FilterableBlockMap<SprueEngine::RGBA> > image_;
    SprueEngine::GraphNode* node_ = 0x0;
    unsigned nodeID_;
    std::string nodeName_;
    SprueEngine::Graph* clone_ = 0x0;
    SprueEngine::Graph* source_ = 0x0;
    unsigned width_ = 0;
    unsigned height_ = 0;
};

}