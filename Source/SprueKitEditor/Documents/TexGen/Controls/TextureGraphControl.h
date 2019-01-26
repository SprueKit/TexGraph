#pragma once

#include "../../../Controls/BaseGraphControl.h"

#include <EditorLib/Controls/ISignificantControl.h>

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEditor
{
    class TextureGenTask;

    /// Specialization of the graph control for manipulating texture graphs
    class TextureGraphControl : public BaseGraphControl, public ISignificantControl
    {
        friend class TextureGraphUpdateVisitor;
        Q_OBJECT
    public:
        TextureGraphControl();
        virtual ~TextureGraphControl();

        void PreviewGenerated(TextureGenTask* task);

    protected:
        virtual void AllowConnect(bool&, QNEPort*, QNEPort*) override;
        virtual void ConnectionDeleted(QNEPort* lhs, QNEPort* rhs) override;

        //virtual void contextMenuEvent(QContextMenuEvent*) Q_DECL_OVERRIDE;
        virtual void dragMoveEvent(QDragMoveEvent*) Q_DECL_OVERRIDE;
        virtual void dragEnterEvent(QDragEnterEvent*) Q_DECL_OVERRIDE;
        virtual void dropEvent(QDropEvent*) Q_DECL_OVERRIDE;

        virtual void GraphNodeExternallyUdpdated(SprueEngine::GraphNode*) override;

    private:
        void QueuePreviewUpdate(SprueEngine::GraphNode*);
    };

}