#include "FileBuffer.h"

#include <fstream>

namespace SprueEngine
{

FileBuffer::FileBuffer(const char* filePath, bool open, bool binary) :
    inStream(0x0),
    outStream(0x0)
{
    if (open)
    {
        inStream = new std::ifstream(filePath, binary ? std::ios::binary | std::ios::in : std::ios::in);
        inStream->seekg(std::ios::end);
        size_ = inStream->tellg();
        inStream->seekg(std::ios::beg);
        position_ = 0;
    }
    else
        outStream = new std::ofstream(filePath, binary ? std::ios::binary | std::ios::out : std::ios::out);
}

FileBuffer::~FileBuffer()
{
    if (inStream)
    {
        delete inStream;
    }
    if (outStream)
    {
        outStream->flush();
        delete outStream;
    }
}


unsigned FileBuffer::Read(void* dest, unsigned size)
{
    if (inStream)
    {
        inStream->read((char*)dest, size);
        return size;
    }
    return 0;
}

unsigned FileBuffer::Seek(unsigned position)
{
    if (inStream)
    {
        inStream->seekg(position, std::ios::beg);
        return inStream->tellg();
    }
    if (outStream)
    {
        outStream->seekp(position, std::ios::beg);
        return outStream->tellp();
    }
    return -1;
}

unsigned FileBuffer::Write(const void* data, unsigned size)
{
    if (outStream)
    {
        outStream->write((const char*)data, size);
        return size;
    }
    return 0;
}

}