#include "HTMLReport.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/Loaders/BasicImageLoader.h>
#include <SprueEngine/Libs/Base64.h>

#include <fstream>

namespace SprueEngine
{

    HTMLReport::HTMLReport(const std::string& filePath, const std::string& title)
    {
        stream_ = new std::ofstream(filePath, std::ios::out);
        if (!stream_->is_open())
        {
            delete stream_;
            stream_ = 0x0;
        }
        else
            WriteHeader(title);
    }

    HTMLReport::HTMLReport(const std::string& filePath, const std::string& title, const HTMLStyleSheet& stylesheet)
    {
        stream_ = new std::ofstream(filePath, std::ios::out);
        if (!stream_->is_open())
        {
            delete stream_;
            stream_ = 0x0;
        }
        else
            WriteHeader(title, stylesheet);
    }

    HTMLReport::~HTMLReport()
    {
        if (stream_)
        {
            if (stream_->is_open())
                stream_->flush();
            delete stream_;
        }
    }

    void HTMLReport::WriteHeader(const std::string& title)
    {
        PushTag("html");
        PushTag("head");
        PushTag("title");
        *stream_ << title;
        PopTag(); // title
        PopTag(); // head
        PushTag("body");
    }

    void HTMLReport::WriteHeader(const std::string& title, const HTMLStyleSheet& stylesheet)
    {
        PushTag("html");
        PushTag("head");
        PushTag("title");
        *stream_ << title;
        PopTag(); // title
        PushTag("style");

        for (auto clazz : stylesheet.GetClasses())
        {
            WriteIndent(tagStack_.size());

            *stream_ << clazz.selector_ << " {\r\n";

            for (auto property : clazz.properties_)
            {
                WriteIndent(tagStack_.size() + 1);
                *stream_ << property.first << ": " << property.second << ";\r\n";
            }

            *stream_ << "}\r\n\r\n";
        }

        PopTag(); // style
        PopTag(); // head
        PushTag("body");
    }

    void HTMLReport::WriteFooter()
    {
        PopTag();
        PopTag();
    }

    void HTMLReport::Img(const std::string& path, const std::string& style)
    {
        WriteIndent(tagStack_.size());
        *stream_ << "<img src=\"" << path << "\"";
        if (!style.empty())
            *stream_ << " class=\"" << style << "\"";
        *stream_ << " />\r\n";
    }

    void HTMLReport::ImgEmbedded(const FilterableBlockMap<RGBA>* image, const std::string& style)
    {
        WriteIndent(tagStack_.size());
        VectorBuffer imgBuffer;
        BasicImageLoader::SavePNG(image, imgBuffer);
        
        size_t b64len = Base64::EncodedLength(imgBuffer.GetSize());
        char* buffer = new char[b64len];
        if (Base64::Encode((const char*)imgBuffer.GetData(), imgBuffer.GetSize(), buffer, b64len))
        {
            *stream_ << "<img src=\"data:image/png;base64,";
            stream_->write(buffer, b64len);
            *stream_ << "\"";
            if (!style.empty())
                *stream_ << " class=\"" << style << "\"";
            *stream_ << " />\r\n";
        }
        delete[] buffer;
    }

    void HTMLReport::Anchor(const std::string& name)
    {
        WriteIndent(tagStack_.size());
        *stream_ << "<a name=\"" << name << "\" />\r\n";
    }

    void HTMLReport::Link(const std::string& url, const std::string& style)
    {
        WriteIndent(tagStack_.size());
        *stream_ << "<a href=\"" << url << "\"";
        if (!style.empty())
            *stream_ << " class=\"" << style << "\"";
        *stream_ << " >";
        tagStack_.push("a");
    }

    void HTMLReport::Header(int level, const std::string& style)
    {
        switch (level)
        {
        case 1:
            return PushTag("h1", style);
        case 2:
            return PushTag("h2", style);
        case 3:
            return PushTag("h3", style);
        case 4:
            return PushTag("h4", style);
        case 5:
            return PushTag("h5", style);
        }
        return PushTag("h3", style);
    }

    void HTMLReport::TableCell(const std::string& tag, int colSpan, int rowSpan, std::string clazz)
    {
        WriteIndent(tagStack_.size());
        *stream_ << "<" << tag << " colspan=\"" << colSpan << "\" rowspan=\"" << rowSpan << "\"";
        if (!clazz.empty())
            *stream_ << " class=\"" << clazz << "\"";
        *stream_ << '>';
        tagStack_.push("td");
    }

    void HTMLReport::PushTag(const std::string& tag, bool selfClosing)
    {
        PushTag(tag, std::string(), selfClosing);
    }

    void HTMLReport::PushTag(const std::string& tag, const std::string& clazz, bool selfClosing)
    {
        WriteIndent(tagStack_.size());
        *stream_ << '<' << tag;
        if (!clazz.empty())
            *stream_ << " class=\"" << clazz << "\"";
        *stream_ << (selfClosing ? " />" : " >");
        if (!selfClosing)
            tagStack_.push(tag);
    }

    void HTMLReport::PopTag()
    {
        if (tagStack_.size())
        {
            WriteIndent(tagStack_.size() - 1);
            *stream_ << "</" << tagStack_.top() << ">\r\n";
            tagStack_.pop();
        }
    }

    void HTMLReport::WriteIndent(int depth)
    {
        for (int i = 0; i < depth; ++i)
            *stream_ << "    ";
    }

    void HTMLReport::Text(const std::string& text) 
    { 
        *stream_ << text; 
    }

    void HTMLReport::Br() 
    { 
        *stream_ << "<br />"; 
    }
}