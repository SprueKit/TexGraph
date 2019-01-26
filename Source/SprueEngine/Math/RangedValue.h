#pragma once

#include <SprueEngine/Math/MathDef.h>

namespace SprueEngine
{

    template<typename T>
    struct RangedNumber
    {
        T lowerBound_;
        T upperBound_;
        bool inclusive_;

        RangedNumber() : inclusive_(true) { }
        RangedNumber(T low, T high, bool inclusive = true) : lowerBound_(low), upperBound_(high), inclusive_(inclusive) { }

        virtual T Clip(T value) const = 0;
        virtual T GetRange() const { return upperBound_ - lowerBound_; }
        virtual T GetRandom() const = 0;
    };

    struct RangedInt : public RangedNumber < int >
    {
        int GetRandom() const override {
            return (rand() % (upperBound_ - lowerBound_)) + lowerBound_;
        }

        RangedInt() : RangedNumber<int>() { }
        RangedInt(int low, int high, bool inclusive = true) : RangedNumber<int>(low, high, inclusive) { }

        // need these for C# bindings
        int getLowerBound() const { return lowerBound_; }
        void setLowerBound(int value) { lowerBound_ = value; }
        int getUpperBound() const { return upperBound_; }
        void setUpperBound(int value) { upperBound_ = value; }
        bool getInclusive() const { return inclusive_; }
        void setInclusive(bool state) { inclusive_ = state; }

        virtual int Clip(int value) const { return CLAMP(value, lowerBound_, upperBound_); }
        int getValue() const { return GetRandom(); }
    };

    struct RangedFloat : public RangedNumber < float >
    {
        float GetRandom() const override {
            return lowerBound_ + ((((float)rand()) / (float)RAND_MAX) * (upperBound_ - lowerBound_ + 1));
        }

        RangedFloat() : RangedNumber<float>() { }
        RangedFloat(float low, float high, bool inclusive = true) : RangedNumber<float>(low, high, inclusive) { }

        // need these for C# bindings
        float getLowerBound() const { return lowerBound_; }
        void setLowerBound(float value) { lowerBound_ = value; }
        float getUpperBound() const { return upperBound_; }
        void setUpperBound(float value) { upperBound_ = value; }
        bool getInclusive() const { return inclusive_; }
        void setInclusive(bool state) { inclusive_ = state; }

        virtual float Clip(float value) const { return CLAMP(value, lowerBound_, upperBound_); }
        float getValue() const { return GetRandom(); }
    };

}