#pragma once

#include <SprueEngine/ClassDef.h>

#include <SprueEngine/BlockMap.h>
#include <SprueEngine/Math/Color.h>
#include <SprueEngine/Reports/HTMLStyleSheet.h>

#include <iostream>
#include <stack>
#include <string>

namespace SprueEngine
{

    /// Writes HTML into a stream.
    class SPRUE HTMLReport
    {
    public:
        /// Construct for a given filepath.
        HTMLReport(const std::string& filePath, const std::string& title);
        /// Construct for a given filepath and with a stylesheet.
        HTMLReport(const std::string& filePath, const std::string& title, const HTMLStyleSheet& stylesheet);
        /// Destruct.
        virtual ~HTMLReport();

        /// Returns true if the file is valid for writing.
        bool IsValid() const { return stream_ != 0x0; }

        /// Writes raw text data.
        void Text(const std::string& text);
        /// Write an img tag with source. Does not require popping the tag as it is self closing.
        void Img(const std::string& path, const std::string& style = std::string());
        /// Write a base64 embedded image. Does not require popping the tag as it is self closing.
        void ImgEmbedded(const FilterableBlockMap<RGBA>* image, const std::string& style = std::string());
        /// Write an anchor tag for #NAME links. Does not require popping the tag as it is self closing.
        void Anchor(const std::string& name);
        /// Write an anchor/link tag with an href.
        void Link(const std::string& url, const std::string& style = std::string());
        /// Write a h# tag.
        void Header(int level, const std::string& style = std::string());
        /// Write a div tag.
        void Div(const std::string& style = std::string()) { PushTag("div", style); }
        /// Write a paragrapht ag.
        void P(const std::string& style = std::string()) { PushTag("p", style); }
        /// Write a table tag.
        void Table(const std::string& style = std::string()) { PushTag("table", style); }
        /// Write a table header cell.
        void Th(int colSpan = 1, int rowSpan = 1, const std::string& style = std::string()) { TableCell("th", colSpan, rowSpan, style); }
        /// Write a table row.
        void Tr(const std::string& style = std::string()) { PushTag("tr", style); }
        /// Write a table cell.
        void Td(int colSpan = 1, int rowSpan = 1, const std::string& style = std::string()) { TableCell("td", colSpan, rowSpan, style); }
        /// Writes a break tag.
        void Br();
        /// Write an onordered list.
        void UL(const std::string& style = std::string()) { PushTag("ul", style); }
        /// Write an ordered list.
        void OL(const std::string& style = std::string()) { PushTag("ol", style); }
        /// Write a list item.
        void LI(const std::string& style = std::string()) { PushTag("li", style); }
        /// Helper function for creating a plain text <li>text</li>
        void ListItem(const std::string& text, const std::string& style = std::string()) { PushTag("li", style); Text(text); PopTag(); }
        /// Write a bold tag.
        void Bold() { PushTag("b", std::string()); }
        /// Write an italic tag.
        void Italic() { PushTag("i", std::string()); }
        /// Write an underline tag.
        void Underline() { PushTag("u", std::string()); }
        /// Write a bold tag with text included.
        void Bold(const std::string& text) { PushTag("b", std::string()); Text(text); PopTag(); }
        /// Write an italic tag with text included.
        void Italic(const std::string& text) { PushTag("i", std::string()); Text(text); PopTag(); }
        /// Write an underline tag with text included.
        void Underline(const std::string& text) { PushTag("u", std::string()); Text(text); PopTag(); }

        /// Pops a tag for closing from the stack.
        void PopTag();

        /// Writes an indent to reach the target depth.
        void WriteIndent(int depth);

        /// Returns the current depth in the tag stack.
        int GetDepth() const { return tagStack_.size(); }

    protected:
        /// Writes the <html><head><title>_____</title></head><body> cluster.
        void WriteHeader(const std::string& title);
        /// Writes the header and writes the contents of the stylesheet into the style tag.
        void WriteHeader(const std::string& title, const HTMLStyleSheet& stylesheet);
        /// Writes the </body></html> cluster for when the document is finished.
        void WriteFooter();

        void TableCell(const std::string& tag, int colSpan, int rowSpan, std::string clazz);
        void PushTag(const std::string& tag, bool selfClosing = false);
        void PushTag(const std::string& tag, const std::string& clazz, bool selfClosing = false);

        std::stack<std::string> tagStack_;
        std::ofstream* stream_;
    };

}