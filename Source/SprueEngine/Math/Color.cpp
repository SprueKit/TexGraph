#include "SprueEngine/Math/Color.h"

#include <SprueEngine/Deserializer.h>
#include <SprueEngine/Serializer.h>

namespace SprueEngine
{

const RGBA RGBA::Clear =    RGBA(0.0f,0.0f,0.0f,0.0f);
const RGBA RGBA::Black =    RGBA(0.0f,0.0f,0.0f,1.0f);
const RGBA RGBA::Red =      RGBA(1.0f,0.0f,0.0f,1.0f);
const RGBA RGBA::Green =    RGBA(0.0f,1.0f,0.0f,1.0f);
const RGBA RGBA::Blue =     RGBA(0.0f,0.0f,1.0f,1.0f);
const RGBA RGBA::White =    RGBA(1.0f,1.0f,1.0f,1.0f);
const RGBA RGBA::Gray =     RGBA(0.5f, 0.5f, 0.5f, 1.0f);
const RGBA RGBA::Gold =     RGBA(1.0f, 0.86f, 0.0f, 1.0f);
const RGBA RGBA::Yellow =   RGBA(1.0f, 1.0f, 0.0f, 1.0f);
const RGBA RGBA::Cyan =     RGBA(0.0f, 1.0f, 1.0f, 1.0f);
const RGBA RGBA::Magenta =  RGBA(1.0f, 0.0f, 1.0f, 1.0f);
const RGBA RGBA::Invalid = RGBA(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);

float RGBA::Distance(const RGBA& rhs) const
{
    return 0.0f;
}

bool RGBA::IsValid() const
{
    return r != FLT_MAX && g != FLT_MAX && b != FLT_MAX && a != FLT_MAX;
}

RGBA ColorRamp::Get(float position) const
{
    RGBA ret;
    for (unsigned i = 0; i < colors.size() - 1; ++i)
    {
        if (position < colors[i].first)
            return colors[i].second;

        if (position > colors[i + 1].first)
            continue;

        const float lhs = colors[i].first;
        const float rhs = colors[i + 1].first;
        return SprueLerp(colors[i].second, colors[i + 1].second, NORMALIZE(position, lhs, rhs));
    }

    if (colors.size() > 0)
        return colors.back().second;

    return ret;
}

void ColorRamp::Deserialize(Deserializer* src)
{
    unsigned ct = src->ReadUInt();
    colors.resize(ct);
    for (int i = 0; i < ct; ++i)
        colors[i] = std::make_pair(src->ReadFloat(), src->ReadColor());
}

void ColorRamp::Serialize(Serializer* dest)
{
    dest->WriteUInt((unsigned)colors.size());
    for (auto color : colors)
    {
        dest->WriteFloat(color.first);
        dest->WriteColor(color.second);
    }
}

float ColorCurve::GetY(float pos) const
{
    for (int i = 0; i < knots_.size() - 1; ++i)
    {
        Vec2 cur = knots_[i];
        Vec2 next = knots_[i + 1];
        if (pos >= cur.x && pos <= next.x)
        {
            float t = (pos - cur.x) / (next.x - cur.x);
 
		    const float a = 1.0f - t;
		    const float b = t;
		    const float h = next.x - cur.x;

            // Couldn't have figured this bit out without, the interpolation was my error:
            // http://www.developpez.net/forums/d331608-3/general-developpement/algorithme-mathematiques/contribuez/image-interpolation-spline-cubique/#post3513925
		    return a * cur.y + b * next.y + (h * h / 6.0f) * ((a*a*a - a) * derivatives_[i] + (b*b*b - b) * derivatives_[i + 1]);
        }
    }
    // Deal with edges
    if (pos <= 0.0f)
        return 0.0f;
    if (pos >= 0.5f && knots_.size() > 0)
        return knots_.back().y;
    return 0.0f;
}

void ColorCurve::CalculateDerivatives()
{
    const int count = knots_.size();

    static const float XDiv = 1.0f / 6.0f; // Should be 2Pi?
    static const float YDiv = 1.0f / 3.0f; // Should be Pi?
    static const float ZDiv = 1.0f / 6.0f; // Shold be 2Pi?
    
    derivatives_.clear();
    std::vector<Vec3> knotTans(count);
    derivatives_.resize(count, 0.0f);

    knotTans[0] = Vec3(0, 1, 0);
    knotTans[count - 1] = Vec3(0, 1, 0);
    for (int i = 1; i < count - 1; ++i)
    {
        knotTans[i].x = (knots_[i].x - knots_[i - 1].x) * XDiv;
        knotTans[i].y = (knots_[i + 1].x - knots_[i - 1].x) * YDiv;
        knotTans[i].z = (knots_[i + 1].x - knots_[i].x) * ZDiv;
        derivatives_[i] = (knots_[i + 1] - knots_[i]).YOverX() - (knots_[i] - knots_[i - 1]).YOverX();
    }

    for (int i = 1; i < count - 1; ++i)
    {
        const float m = knotTans[i].x / knotTans[i - 1].y;
        knotTans[i][1] -= m * knotTans[i - 1].x;
        knotTans[i][0] = 0;
        derivatives_[i] -= m * derivatives_[i - 1];
    }

    for (int i = count - 2; i >= 0; --i)
    {
        const float m = knotTans[i].z / knotTans[i + 1].y;
        knotTans[i][1] -= m * knotTans[i + 1].x;
        knotTans[i][2] = 0;
        derivatives_[i] -= m * derivatives_[i + 1];
    }

    for (int i = 0; i < count; ++i) 
        derivatives_[i] /= knotTans[i].y;
}

void ColorCurve::Deserialize(Deserializer* src)
{
    unsigned ct = src->ReadUInt();
    knots_.clear();
    while (ct)
    {
        knots_.push_back(src->ReadVector2());
        --ct;
    }
    if (knots_.size() > 0)
        CalculateDerivatives();
}

void ColorCurve::Serialize(Serializer* dest) const
{
    dest->WriteUInt((unsigned)knots_.size());
    for (auto knot : knots_)
        dest->WriteVector2(knot);
}

void ColorCurves::Deserialize(Deserializer* src)
{
    R.Deserialize(src);
    G.Deserialize(src);
    B.Deserialize(src);
    A.Deserialize(src);
}

void ColorCurves::Serialize(Serializer* dest) const
{
    R.Serialize(dest);
    G.Serialize(dest);
    B.Serialize(dest);
    A.Serialize(dest);
}

}