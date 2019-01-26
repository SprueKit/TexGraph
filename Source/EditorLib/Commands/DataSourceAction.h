#pragma once

#include <EditorLib/ApplicationCore.h>
#include <EditorLib/DataSource.h>
#include <EditorLib/Commands/RegisteredAction.h>
#include <EditorLib/Selectron.h>

#include <memory>

template<typename T, bool SINGLE = true>
class EDITORLIB_EXPORT DataSourceAction : public RegisteredAction
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
    
    virtual void CheckCmdUI() override {
        RegisteredAction::CheckCmdUI();
        const auto selectron = ApplicationCore::GetObjectSelectron();

        if (selectron->GetSelectedCount() == 0)
        {
            setEnabled(false);
            return;
        }

        if (SINGLE)
        {
            if (selectron->GetSelectedCount() != 1)
                setEnabled(false);
            else if (auto isTarget = selectron->GetMostRecentSelected<T>())
                setEnabled(true);
            else
                setEnabled(false);
        }    
        else
        {
            bool anyHits = false;
            for (int i = 0; i < selectron->GetSelectedCount(); ++i)
            {
                if (selectr->GetSelected<T>(i))
                    anyHits = true;
            }
            setEnabled(anyHits);
        }
    }
};