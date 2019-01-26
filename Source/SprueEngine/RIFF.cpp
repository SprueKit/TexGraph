#include "RIFF.h"

#include <SprueEngine/Deserializer.h>
#include <SprueEngine/Serializer.h>

namespace SprueEngine
{

    RIFFChunk::RIFFChunk()
    {
        memset(type_, 0, 4);
        size_ = 0;
        offset_ = 0;
    }

    RIFFChunk::~RIFFChunk()
    {
        if (data_ && ownsData_)
            delete[] data_;
    }

    bool RIFFChunk::IsType(const char* fourcc) const
    {
        return memcmp(type_, fourcc, 4) == 0;
    }

    void RIFFChunk::Write(Serializer* dest)
    {
        WriteHeader(dest);

        if (data_)
            dest->Write(data_, size_);

        // Write padding byte if size is odd
        if (size_ % 2 == 1)
            dest->WriteByte(0xFDFDFD);
    }

    void RIFFChunk::Read(Deserializer* src, bool readAllData)
    {
        if (!ReadHeader(src))
            return; // TODO: throw up error

        if (data_)
            delete[] data_;
        data_ = 0x0;

        const bool needsPadByte = size_ % 2 == 1;
        if (readAllData)
        {
            data_ = new unsigned char[size_];
            src->Read(data_, size_);
            if (needsPadByte)
                src->Seek(src->GetPosition() + 1);
        }
        else
            src->Seek(src->GetPosition() + size_ + (needsPadByte ? 1 : 0));
    }

    bool RIFFChunk::LoadData(Deserializer* src)
    {
        if (size_ == 0 || offset_ == 0)
            return false;

        src->Seek(offset_);
        if (data_ && ownsData_)
            delete[] data_;
        data_ = 0x0;

        if (!data_)
            data_ = new unsigned char[size_];
        ownsData_ = true;
        src->Read(data_, size_);
    }

    RIFFChunk* RIFFChunk::CreateChunk(const char* typeID, unsigned char* data, unsigned dataSize, bool ownsData)
    {
        RIFFChunk* ret = new RIFFChunk();
        ret->ownsData_ = ownsData;
        memcpy(ret->type_, typeID, 4);
        ret->size_ = dataSize;
        if (ownsData)
        {
            ret->data_ = new unsigned char[dataSize];
            memcpy(ret->data_, data, dataSize);
        }
        else
        {
            ret->data_ = (unsigned char*)data;
        }
        return ret;
    }


    void RIFFChunk::WriteHeader(Serializer* dest)
    {
        dest->Write(type_, 4);
        dest->Write(&size_, sizeof(unsigned));
    }

    bool RIFFChunk::ReadHeader(Deserializer* src)
    {
        src->Read(type_, 4);
        src->Read(&size_, 4);
        offset_ = src->GetPosition();
        return true;
    }

    RIFF::~RIFF()
    {
        for (auto chunk : chunks_)
            delete chunk;
    }

    void RIFF::CalculateOffsets(unsigned base)
    {
        unsigned current = base;
        current += GetHeaderSize();
        for (auto chunk : chunks_)
        {
            chunk->CalculateOffsets(current);
            current += chunk->GetBlockSize();
        }
    }

    void RIFF::CalculateSize()
    {
        for (auto chunk : chunks_)
            size_ += chunk->GetBlockSize(); // 4 byte FOURCC, 4 byte size
    }

    unsigned RIFF::GetBlockSize()
    {
        CalculateSize();
        return size_ + GetHeaderSize();
    }

    unsigned RIFF::GetChunkCount(bool countLists, bool recurse) const
    {
        unsigned ct = 0;
        for (auto chunk : chunks_)
        {
            if (!chunk->IsList())
                ++ct;
            else if (recurse)
                ct += ((RIFF*)chunk)->GetChunkCount(recurse) + (countLists ? 1 : 0);
            else if (countLists)
                ++ct;
        }
        return ct;
    }

    void RIFF::Write(Serializer* dest)
    {
        CalculateSize();

        WriteHeader(dest);

        for (auto chunk : chunks_)
        {
            dest->Write(chunk->type_, 4);
            dest->WriteUInt(chunk->size_);
            if (chunk->data_ && chunk->size_)
                dest->Write(chunk->data_, chunk->size_);
            if (chunk->size_ % 2 == 1)
                dest->WriteByte(0xFDFDFD);
        }
    }

    void RIFF::Read(Deserializer* src, bool readAllData)
    {
        size_ = 0;
        memset(type_, 0, 4);

        if (!ReadHeader(src))
            return;

        offset_ = src->GetPosition();
        
        char chunkType[4] = { 'N', 'O', 'N', 'E' };
        while (src->GetPosition() < (offset_ + size_) && src->Read(chunkType, 4) == 4) // as long as we keep hitting valid chunktypes then continue
        {
            if (RIFFChunk* chunk = CreateChunk(chunkType))
            {
                // Move back 4 bytes
                src->Seek(src->GetPosition() - 4);
                chunk->Read(src, readAllData);
                chunks_.push_back(chunk);
            }
        }
    }

    /// Get a chunk by index.
    RIFFChunk* RIFF::GetChunk(unsigned chunk)
    {
        if (chunk < chunks_.size())
            return chunks_[chunk];
        return 0x0;
    }

    /// Get a chunk by fourcc code.
    RIFFChunk* RIFF::GetChunk(const char* fourcc, RIFFChunk* previous)
    {
        // No previous object given to check then we've always hit the 'previous'
        bool hitPrevious = previous == 0x0 ? true : false;
        for (auto chunk : chunks_)
        {
            if (memcmp(chunk->type_, fourcc, 4) == 0)
            {
                // If we're passed the given 'previous' object then return the new thing found
                if (hitPrevious)
                    return chunk;
                else if (chunk == previous) // if we are the 'previous' object then indicate that we hit the previous
                    hitPrevious == true;
            }
        }
        return 0x0;
    }

    void RIFF::Visit(RIFFChunkVisitor* visitor)
    {
        if (!visitor)
            return;

        visitor->VisitChunk(this);

        for (auto chunk : chunks_)
            chunk->Visit(visitor);
    }

    RIFF* RIFF::CreateRIFF(const char* title, const char* typeID)
    {
        RIFF* newRiff = new RIFF();
        memcpy(newRiff->title_, title, 4);
        memcpy(newRiff->type_, typeID, 4);
        return newRiff;
    }

    RIFFChunk* RIFF::CreateChunk(const char* typeID)
    {
        if (memcmp(typeID, "RIFF", 4) == 0)
            return new RIFF();
        else if (memcmp(typeID, "LIST", 4) == 0)
            return new RIFF();
        return new RIFFChunk();
    }

    void RIFF::WriteHeader(Serializer* dest)
    {
        dest->Write(title_, 4); // RIFF FOURCC
        dest->WriteUInt(size_);
        dest->Write(type_, 4); // File FOURCC
    }

    bool RIFF::ReadHeader(Deserializer* src)
    {
        size_ = 0;
        memset(type_, 0, 4);

        // Read the basic riff header
        src->Read((void*)title_, 4);

        if (memcmp(title_, "RIFF", 4) != 0)
            return false; // TODO error messages

        src->Read(&size_, 4);
        src->Read(&type_, 4);

        return true;
    }
}