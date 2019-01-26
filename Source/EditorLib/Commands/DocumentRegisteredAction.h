#pragma once

#include <EditorLib/Commands/RegisteredAction.h>

template<typename T>
class DocumentRegisteredAction : RegisteredAction
{
public:
    DocumentRegisteredAction(const QString& actionClass, const QString& text, QObject* owner = 0x0) :
        RegisteredAction(actionClass, text, owner)
    {

    }
    /// Construct. Action class is used for grouping commands
    DocumentRegisteredAction(const QString& actionClass, const QIcon& icon, const QString& text, QObject* owner = 0x0) :
        RegisteredAction(actionClass, icon, text, owner)
    {

    }

    virtual bool IsDocumentRelevant(DocumentBase* doc)
    {
        return dynamic_cast<T*>(doc) != 0x0;
    }
};