#pragma once

#include <SprueEngine/ClassDef.h>

#include <map>
#include <string>
#include <vector>

namespace SprueEngine
{

    class SPRUE HTMLStyleSheet
    {
    public:
        HTMLStyleSheet();
        virtual ~HTMLStyleSheet();

        struct CSSClass {
            std::string selector_;
            std::map<std::string, std::string> properties_;
        };

        void AddClass(const std::string& selector, const std::map<std::string, std::string>& properties) {
            classes_.push_back({ selector, properties });
        }

        const std::vector<CSSClass>& GetClasses() const { return classes_; }

    private:
        std::vector<CSSClass> classes_;
    };

}