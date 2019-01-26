#pragma once

#include <memory>

class DataSource;
class QImGui;

/// Responsible for translating a DataSource into a general-purpose IMGUI output (properties editor).
/// How the implementation divides its work is not the caller's responsibility.
class PropertyEnumerator
{
public:
    /// Will be called in order to find the appropriate enumerator to use for a datasource.
    virtual bool CanProcessDataSource(std::shared_ptr<DataSource>) = 0;
    /// Will be called when it's desired to emit the properties into a IMGUI window.
    virtual void PrintProperties(QImGui* gui, std::shared_ptr<DataSource>) = 0;
};