#pragma once

#include <SprueEngine/Deserializer.h>
#include <SprueEngine/Serializer.h>

namespace SprueEngine
{

class SPRUE FileBuffer : public Serializer, public Deserializer
{
    NOCOPYDEF(FileBuffer);
public:
    FileBuffer(const char* filePath, bool open, bool binary);
    ~FileBuffer();

    virtual unsigned Read(void* dest, unsigned size) override;
    
    virtual unsigned Seek(unsigned position) override;

    virtual unsigned Write(const void* data, unsigned size) override;

    std::istream* GetIStream() const { return inStream; }
    std::ostream* GetOStream() const { return outStream; }
    bool IsInput() const { return inStream != 0x0; }

private:
    std::istream* inStream;
    std::ostream* outStream;
};

}