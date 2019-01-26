#pragma once

#include <stack>
#include <vector>

namespace SprueEngine
{

    struct TagFileHeader
    {
        /// If not "    " then it will indicate a tag-file that should be used for a specific language (ie. "ESPA", "GERM", "FRAN", "JAPA", "CHIN", etc)
        char languageCode_[4];
        /// Determines the order in which this tag file is be used
        unsigned short sortKey_ = -1;
        /// Number of tags in each block, with this information we can seek to any tag desired
        unsigned short tagCounts_[32];
    };

    /*
    
Using the RIFF code:

    Reading a RIFF:

        RIFF* riffFile = new RIFF();
        if (!riffFile->Read(src))
        {
            delete riffFile;
            LOG_ERROR("My error message");
            return;
        }

        RIFFChunk* dataChunk = riffFile->GetChunk("BOBS");
        while (dataChunk)
        {        
            // do stuff with the data
            dataChunk = riffFile->GetChunk("BOBS", dataChunk);
        }

        delete riffFile;

    Writing a RIFF:

        RIFF* riff = RIFF::CreateRIFF("DATA");

        // Create some data blocks
        riff->chunks_.push_back(RIFFChunk::CreateChunk("MONS", myPointer, lengthOfDataInMyPointer);
        riff->chunks_.push_back(RIFFChunk::CreateChunk("BOBS", myOtherPointer, lengthOfDataInMyOtherPointer);

        RIFF* list = RIFF::CreateLIST("SUBD");
        list->chunks_.push_back(RIFFChunk::CreateChunk("PROJ", projectilePointer, projectileDataSize);
        list->chunks_.push_back(RIFFChunk::CreateChunk("MESH", meshPointer, meshDataSize);
        riff->chunks_.push_back(list); // add the list to RIFF

        riff->Write(dest);

    */

    class Serializer;
    class Deserializer;

    struct RIFFChunk;

    /// Chunk visitors can be used to perform tasks on a RIFF object.
    /// Example: compress all chunks (or just chunks of a certain type in a RIFF file)
    struct RIFFChunkVisitor
    {
        virtual void VisitChunk(RIFFChunk* chunk) = 0;
    };

    /// RIFF generic data chunk
    struct RIFFChunk
    {
        /// FOURCC type identifier
        char type_[4];
        /// Size of the data block
        unsigned size_ = 0;
        /// Datablock
        unsigned char* data_ = 0x0;

        /// Offset of the chunk into the riff file, calculated when the file is read.
        unsigned offset_ = 0;
        /// Whether we should delete the data or not
        bool ownsData_ = true;

        /// Construct.
        RIFFChunk();
        /// Destruct, delete data if necessary.
        virtual ~RIFFChunk();

        bool IsType(const char* fourcc) const;

        /// Tests whether or not this chunk object is a list.
        virtual bool IsList() const { return false; }

        /// Write to a serializer.
        virtual void Write(Serializer* dest);
        /// Read from a deserializer.
        virtual void Read(Deserializer* src, bool readAllData = true);

        /// Use for lazy loading data from a serializer using the offsets.
        bool LoadData(Deserializer* src);

        /// Calculates where the datablock would be written relative to a given base position.
        virtual void CalculateOffsets(unsigned base = 0) { offset_ = base + GetHeaderSize(); }
        /// Calculates the sizes of all objects recursively, generic data chunks don't need to do anything.
        virtual void CalculateSize() { }
        /// Gets the total data-block.
        virtual unsigned GetBlockSize() { return GetHeaderSize() + size_; }

        virtual void Visit(RIFFChunkVisitor* visitor) { if (!visitor) return; visitor->VisitChunk(this); }

        /// Create a new RIFF chunk from a block of data.
        static RIFFChunk* CreateChunk(const char* typeID, unsigned char* data, unsigned dataSize, bool owns = true);

    protected:
        /// Header methods are abstracted so that potentially extra data could be included in derived types.
        virtual unsigned GetHeaderSize() const { return 8; }
        /// Header methods are abstracted so that potentially extra data could be included in derived types.
        virtual void WriteHeader(Serializer* src);
        /// Header methods are abstracted so that potentially extra data could be included in derived types.
        virtual bool ReadHeader(Deserializer* src);
    };

    /// Generalized RIFF file or RIFF LIST block. 
    /// The protected virtual methods can be used to stick anything into the RIFF header.
    struct RIFF : public RIFFChunk
    {
        /// RIFF title id, will be either RIFF or LIST
        char title_[4];
        /// List of contained chunks in the RIFF file.
        std::vector<RIFFChunk*> chunks_;

        /// Destruct and release contents.
        virtual ~RIFF();

        /// Confirms that this chunk object is a list.
        virtual bool IsList() const override { return true; }

        /// Write all data into a serializer.
        virtual void Write(Serializer* dest) override;
        /// Read all data from a deserializer.
        virtual void Read(Deserializer* src, bool readAllData = true) override;

        /// Will calculate the offsets of all chunks in the RIFF file.
        virtual void CalculateOffsets(unsigned base = 0) override;
        /// Calculates the data size.
        virtual void CalculateSize() override;
        /// Returns the total size of the RIFF object, both data and header.
        virtual unsigned GetBlockSize() override;

        /// Count the total number of chunks contained.
        unsigned GetChunkCount(bool countLists, bool recurse = true) const;
        /// Get a chunk by index.
        RIFFChunk* GetChunk(unsigned chunk);
        /// Get a chunk by fourcc code.
        RIFFChunk* GetChunk(const char* fourcc, RIFFChunk* previous = 0x0);

        /// Visits every chunk in the RIFF tree with the given visitor object.
        virtual void Visit(RIFFChunkVisitor* visitor) override;

        /// Creates a RIFF file.
        static RIFF* CreateRIFF(const char* typeID) { CreateRIFF("RIFF", typeID); }
        
        /// Creates a LIST object.
        static RIFF* CreateList(const char* typeID) { CreateRIFF("LIST", typeID); }

    protected:
        /// Creates any type of RIFF list object. Derived types may call it.
        static RIFF* CreateRIFF(const char* title, const char* typeID);

        /// Override so that different RIFF types can be constructed during serialization.
        virtual RIFFChunk* CreateChunk(const char* typeID);
        /// Header methods are abstracted so that potentially extra data could be included in derived types.
        virtual unsigned GetHeaderSize() const { return 12; }
        /// Header methods are abstracted so that potentially extra data could be included in derived types.
        virtual void WriteHeader(Serializer* src) override;
        /// Header methods are abstracted so that potentially extra data could be included in derived types.
        virtual bool ReadHeader(Deserializer* src) override;
    };
}