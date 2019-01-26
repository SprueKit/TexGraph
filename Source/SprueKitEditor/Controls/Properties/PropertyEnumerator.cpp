#include "PropertyEnumerator.h"

#include "PropertyEditorHandler.h"

namespace SprueEditor
{

    void EnumeratedProperty::Destroy()
    {
        if (editor_)
            delete editor_;
    }

}