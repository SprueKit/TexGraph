#pragma once

#include <SprueEngine/Math/MathDef.h>
#include <SprueEngine/Math/Color.h>

#include <math.h>

namespace SprueEngine
{

/// Contains arbitrary types in a 2d/3d grid
///     Grids may be blitted into each other as well
///     TYPE MUST BE COPY-CONSTRUCTABLE and MEM-COPYABLE to use all functions
template<typename T>
class BlockMap
{
public:
    /// Construct an empty blockmap
    BlockMap() : data_(0x0), width_(0), height_(0), depth_(0) {

    }

    /// Construct with default state
    BlockMap(unsigned width, unsigned height, unsigned depth = 1) : data_(0x0)
    {
        resize(width, height, depth);
    }

    /// Copy constructor
    BlockMap(const BlockMap<T>& rhs) : data_(0x0)
    {
        resize(rhs.width_, rhs.height_, rhs.depth_);
        memcpy(data_, rhs.data_, sizeof(T) * width_ * height_ * depth_);
    }

    /// Destroy and release data
    virtual ~BlockMap()
    {
        clear();
    }

    /// Resize to new dimensions, wipes previous data
    virtual void resize(unsigned width, unsigned height, unsigned depth = 1)
    {
        clear();
        width_ = width; height_ = height; depth_ = depth;
        data_ = new T[width * height * depth];
    }

    /// Get the value at the given XYZ coordinates
    const T& get(unsigned x, unsigned y, unsigned z = 1) const { return data_[toIndex(x, y, z)]; }

    /// Get the value at the given XYZ coordinates
    T& get(unsigned x, unsigned y, unsigned z = 1) { return data_[toIndex(x, y, z)]; }

    /// Set the value at the given XYZ coordinates
    void set(const T& value, unsigned x, unsigned y, unsigned z = 1) { data_[toIndex(x, y, z)] = value; }

    /// Fill the blockmap with a specified value
    void fill(const T& value)
    {
        std::fill_n(data_, size(), value);
        //for (unsigned i = 0; i < width_ * height_ * depth_; ++i)
        //    data_[i] = value;
    }

    /// Wipe the blockmap to empty
    void clear() {
        if (data_ != 0x0)
            delete[] data_;
        data_ = 0x0;
        width_ = height_ = depth_ = 0;
    }

    /// Returns true if the blockmap is empty
    bool empty() const { return width_ != 0 && height_ != 0 && depth_ != 0 && data_ != 0x0; }

    /// Return width
    unsigned getWidth() const { return width_; }

    /// Return height
    unsigned getHeight() const { return height_; }
    
    /// Return Z depth
    unsigned getDepth() const { return depth_; }

    /// Return flat 1D size
    unsigned size() const { return width_ * height_ * depth_; }

    /// Return pointer to the backing data
    const T* getData() const { return data_; }
    
    /// Return pointer to the backing data
    T* getData() { return data_; }

    /// Flip blockmap horizontally if 2D
    void flipHorizontal()
    {
        if (!data_)
            return;

        if (depth_ > 1)
            return;

        T* newData = new T[width_ * height_];
        unsigned rowSize = width_ * sizeof(T);
        for (int y = 0; y < height_; ++y)
        {
            for (int x = 0; x < width_; ++x)
            {
                for (unsigned c = 0; c < components_; ++c)
                    newData[y * rowSize + x * components_ + c] = data_[y * rowSize + (width_ - x - 1) * components_ + c];
            }
        }
        T* oldData = data_;
        data_ = newData;
        delete[] oldData;
    }

    /// Flip blockmap vertically if 2D
    void flipVertical()
    {
        if (!data_)
            return;

        if (depth_ > 1)
            return;

        T* newData = new T[width_ * height_];
        unsigned rowSize = width_ * sizeof(T);

        for (int y = 0; y < height_; ++y)
            memcpy(&newData[(height_ - y - 1) * rowSize], &data_[y * rowSize], rowSize);

        T* oldData = data_;
        data_ = newData;
        delete[] oldData;
    }

    void blit(const BlockMap* src, int xPos, int yPos, int zPos = 0)
    {
        // Copy rows of X data from one into the other
        for (unsigned curZ = 0; curZ < src->depth_ && curZ < depth_ - zPos; ++curZ)
        {
            for (unsigned curY = 0; curY < src->height_ && curY < height_ - yPos; ++curY)
                memcpy(
                    data_ + toIndex(SprueMax(xPos, 0), curY + yPos, curZ + zPos), 
                    src->data_ + src->toIndex(-SprueMin(0, xPos), curY, curZ), 
                    sizeof(T) * SprueMin(width_ - xPos, src->width_));
        }
    }

    /// Convert 3D coordinates into 1D
    int toIndex(unsigned x, unsigned y, unsigned z = 1) const {
        if (flipIndexing_)
            y = height_ - y - 1;
        x = CLAMP(x, 0, width_ - 1);
        y = CLAMP(y, 0, height_ - 1);
        z = CLAMP(z, 0, depth_ - 1);
        return (z * width_ * height_ + y * width_ + x);
    }

    int toIndex(unsigned x, unsigned y, unsigned z, unsigned width, unsigned height, unsigned depth) const {
        x = CLAMP(x, 0, width - 1);
        y = CLAMP(y, 0, height - 1);
        z = CLAMP(z, 0, depth - 1);
        return (z * width * height + y * width + x);
    }

    /// Convert 1D coordinate into 3D.
    void fromIndex(unsigned index, unsigned& x, unsigned& y, unsigned& z) const
    {
        z = index / (width_ * height_);
        y = (index - z * width_ * height_) / width_;
        x = index - width_ * (y + height_ * z);
        if (flipIndexing_)
            y = height_ - y - 1;
    }

    /// Sets whether to flip the Y index.
    void SetFlippedIndexing(bool state) { flipIndexing_ = state; }

    /// Tests whether Y axis is flipped in indexing.
    bool IsFlippedIndexing() const { return flipIndexing_; }

    void Clone(BlockMap<T>* into)
    {
        into->resize(width_, height_, depth_);
        memcpy(into->data_, data_, sizeof(T) * width_ * height_ * depth_);
    }

    bool AllValuesEqual() const {
        T curVal;
        for (int i = 0; i < width_ * height_ * depth_; ++i)
        {
            if (i > 0 && curVal != data_[i])
                return false;
            else
                curVal = data_[i];
        }
        return true;
    }

protected:
    unsigned width_;
    unsigned height_;
    unsigned depth_;
    /// Vertically flip the indexing
    bool flipIndexing_ = false;
    T* data_;
};

/// Variation of BlockMap that uses a type that includes methods for:
///     T T::operator*(float)
///     T T::operator+(const T& rhs)
/// In order to support:
///     bilinear/trilinear filtering
///     scaled blitting
///     edge padding
///     filter kernels
///     blurring
template<typename T>
class FilterableBlockMap : public BlockMap<T>
{
public:
    /// Construct a new empty FilterableBlockMap
    FilterableBlockMap() : 
        BlockMap() 
    {

    }

    /// Construct with specified dimensions
    FilterableBlockMap(unsigned width, unsigned height, unsigned depth = 1) : 
        BlockMap(width, height, depth)
    {
    }

    /// Copy construct from a blockmap of the same type
    FilterableBlockMap(const BlockMap<T>& rhs)
    {
        resize(rhs.width_, rhs.height_, rhs.depth_);
        memcpy(data_, rhs.data_, sizeof(T) * width_ * height_);
    }

    /// Copy construct from another filterable blockmap
    FilterableBlockMap(const FilterableBlockMap<T>& rhs)
    {
        resize(rhs.width_, rhs.height_, rhs.depth_);
        memcpy(data_, rhs.data_, sizeof(T) * width_ * height_ * depth_);
    }

    /// Destruct and release data
    virtual ~FilterableBlockMap()
    {
        clear();
    }

    /// Resize to new dimensions, bilinear filter to get new data
    virtual void resize(unsigned width, unsigned height, unsigned depth = 1) override
    {
        if (width < 1 || height < 1 || depth < 1)
            return;

        if (data_ == 0x0)
        {
            BlockMap<T>::resize(width, height, depth);
            return;
        }

        T* oldData = data_;
        data_ = new T[width * height * depth];

        for (unsigned z = 0; z < depth; ++z)
        {
            for (unsigned y = 0; y < height; ++y)
            {
                for (unsigned x = 0; x < width; ++x)
                {
                    T& dest = data_[toIndex(x, y, z, width, height, depth)];
                    const float dx = (width_ > 1 && height > 1) ? (float)x / (float)(width - 1) : 0.0f;
                    const float dy = (height_ > 1 && depth > 1) ? (float)y / (float)(height - 1) : 0.0f;
                    const float dz = (depth_ > 1 && depth > 1) ? (float)z / (float)(depth - 1) : 0.0f;
                    T sample = getTrilinear(dx, dy, dz);
                    dest = sample;
                }
            }
        }

        width_ = width; height_ = height; depth_ = depth;
        delete oldData;
    }

    /// Sample using bilinear filtering for a 2D blockmap (always cell 0 of Z)
    T getBilinear(float x, float y)
    {
        x = x - floorf(x);
        y = y - floorf(y);
        x = CLAMP(x * width_ - 0.5f, 0.0f, (float)(width_ - 1));
        y = CLAMP(y * height_ - 0.5f, 0.0f, (float)(height_ - 1));

        int xI = (int)x;
        int yI = (int)y;

        float xF = x - std::floorf(x);
        float yF = y - std::floorf(y);

        T topValue = (get(xI, yI) * (1.0f - xF)) + (get(xI + 1, yI) * xF);
        T bottomValue = (get(xI, yI + 1) * (1.0f - xF)) + (get(xI + 1, yI + 1) * xF);
        return (topValue * (1.0f - yF)) + (bottomValue * yF);
    }

    /// Sample using trilinear filtering from a 3D blockmap, falls back on bilinear if really just 2D
    T getTrilinear(float x, float y, float z)
    {
        if (depth_ < 2)
            return getBilinear(x, y);

        x = CLAMP(x * width_ - 0.5f, 0.0f, (float)(width_ - 1));
        y = CLAMP(y * height_ - 0.5f, 0.0f, (float)(height_ - 1));
        z = CLAMP(z * depth_ - 0.5f, 0.0f, (float)(depth_ - 1));

        int xI = (int)x;
        int yI = (int)y;
        int zI = (int)z;
        if (zI == depth_ - 1)
            return getBilinear(x, y);

        // Determine blend weights
        const float dx = x - std::floorf(x);
        const float dy = y - std::floorf(y);
        const float dz = z - std::floorf(z);
        const float invDX = 1.0f - dx;
        const float invDY = 1.0f - dy;
        const float invDZ = 1.0f - dz;

        T topValueNear = get(xI, yI, zI) * invDX + get(xI + 1, yI, zI) * dx;
        T bottomValueNear = get(xI, yI + 1, zI) * invDX + get(xI + 1, yI + 1, zI) * dx;
        T valueNear = topValueNear * invDY + bottomValueNear * dy;

        T topValueFar = get(xI, yI, zI + 1) * invDX + get(xI + 1, yI, zI + 1) * dx;
        T bottomValueFar = get(xI, yI + 1, zI + 1) * invDX + get(xI + 1, yI + 1, zI + 1) * dx;
        T valueFar = topValueFar * invDY + bottomValueFar * dy;
        return valueNear * invDZ + valueFar * dz;
    }

protected:
};

}