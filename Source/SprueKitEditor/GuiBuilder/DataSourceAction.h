#pragma once

#include "../SprueKitEditor.h"

#include <EditorLib/Commands/RegisteredAction.h>

namespace SprueEditor
{

    /// Registered action that is only available when specific types of datasources are selected.
    /// This is largely intended to be used as a baseclass.
    template<typename DSTYPE>
    class DataSourceAction : public RegisteredAction
    {
    public:
        /// Construct. Action class is used for grouping commands
        DataSourceAction(const QString& actionClass, const QString& text, QObject* owner = 0x0) :
            RegisteredAction(actionClass, text, owner)
        {
        }
        /// Construct. Action class is used for grouping commands
        DataSourceAction(const QString& actionClass, const QIcon& icon, const QString& text, QObject* owner = 0x0) :
            RegisteredAction(actionClass, icon, text, owner)
        {
        }
        virtual ~DataSourceAction() { }

        virtual bool IsAvailable() const override 
        { 
            bool ret = RegisteredAction::IsAvailable(); 
            ret &= (SprueKitEditor::GetInstance()->GetObjectSelectron()->GetMostRecentSelected<DSTYPE>().get()) != 0x0;
            return ret;
        }
    };
}