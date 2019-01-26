    #pragma once
    
    #include <SprueEngine/ClassDef.h>
    #include <SprueEngine/Math/MathDef.h>
    
    #include <math.h>
    
    namespace SprueEngine
    {
        
    enum CurveType
    {
        CT_Constant,               // Fixed value
        CT_Linear,                 // Algebra standard MX+B
        CT_Quadratic,              // Exponential
        CT_Logistic,               // Sigmoid
        CT_Logit,                  // 90 degree Sigmoid (biology/psych origins)
        CT_Threshold,              // Boolean/stair
        CT_Sine,                   // Sine wave
        CT_Parabolic,              // Algebra standard form parabola
        CT_NormalDistribution,     // Probability density function
        CT_Bounce,                 // Bouncing degrading pattern, effectively decaying noise
    };
    
    struct SPRUE ResponseCurve
    {
        CurveType type_;
        float xIntercept_;
        float yIntercept_;
        float slopeIntercept_;
        float exponent_;
        bool flipX_;
        bool flipY_;
    
        ResponseCurve() : type_(CT_Linear), xIntercept_(0.0f), yIntercept_(0.0f), slopeIntercept_(1.0f), exponent_(1.0f), flipX_(false), flipY_(false)
        {
            xIntercept_ = yIntercept_ = 0.0f;
        }
    
        float GetValue(float x) const
        {
            if (flipX_)
                x = 1.0f - x;
    
            // Evaluate the curve function for the given inputs.
            float value = 0.0f;
            switch (type_)
            {
            case CT_Constant:
                value = yIntercept_;
                break;
            case CT_Linear:
                // y = m(x - c) + b ... x expanded from standard mx+b
                value = (slopeIntercept_ * (x - xIntercept_)) + yIntercept_;
                break;
            case CT_Quadratic:
                // y = mx * (x - c)^K + b
                value = ((slopeIntercept_ * x) * powf(abs(x + xIntercept_), exponent_)) + yIntercept_;
                break;
            case CT_Logistic:
                // y = (k * (1 / (1 + (1000m^-1*x + c))) + b
                value = (exponent_ * (1.0f / (1.0f + powf(abs(1000.0f * slopeIntercept_), (-1.0f * x) + xIntercept_ + 0.5f)))) + yIntercept_; // Note, addition of 0.5 to keep default 0 XIntercept sane
                break;
            case CT_Logit:
                // y = -log(1 / (x + c)^K - 1) * m + b
                value = (-logf((1.0f / powf(abs(x - xIntercept_), exponent_)) - 1.0f) * 0.05f * slopeIntercept_) + (0.5f + yIntercept_); // Note, addition of 0.5f to keep default 0 XIntercept sane
                break;
            case CT_Threshold:
                value = x > xIntercept_ ? (1.0f - yIntercept_) : (0.0f - (1.0f - slopeIntercept_));
                break;
            case CT_Sine:
                // y = sin(m * (x + c)^K + b
                value = (sinf(slopeIntercept_ * powf(x + xIntercept_, exponent_)) * 0.5f) + 0.5f + yIntercept_;
                break;
            case CT_Parabolic:
                // y = mx^2 + K * (x + c) + b
                value = powf(slopeIntercept_ * (x + xIntercept_), 2) + (exponent_ * (x + xIntercept_)) + yIntercept_;
                break;
            case CT_NormalDistribution:
                // y = K / sqrt(2 * PI) * 2^-(1/m * (x - c)^2) + b
                value = (exponent_ / (sqrtf(2 * 3.141596f))) * powf(2.0f, (-(1.0f / (abs(slopeIntercept_) * 0.01f)) * powf(x - (xIntercept_ + 0.5f), 2.0f))) + yIntercept_;
                break;
            case CT_Bounce:
                value = abs(sinf((6.28f * exponent_) * (x + xIntercept_ + 1.0f) * (x + xIntercept_ + 1.0f)) * (1.0f - x) * slopeIntercept_) + yIntercept_;
                break;
            }
    
            // Invert the value if specified as an inverse.
            if (flipY_)
                value = 1.0f - value;
    
            // Constrain the return to a normal 0-1 range.
            return CLAMP01(value);
        }
    };
    
    }