#pragma once

#include "ArtisticNoise.h"

#include "../Core/Context.h"

namespace SprueEngine
{

    static const char* ArtisticNoiseBlendNames[] = {
        "Smart",
        "Alternate +/-",
        0x0
    };

    static const char* ArtisticNoiseRecordNameList[] = {
        "Texture",
        0x0
    };

    static const StringHash ArtisticNoiseRecordHashes[] = {
        StringHash("ArtisticNoiseRecord"),
    };

    void ArtisticNoiseRecord::Register(Context* context)
    {
        context->RegisterFactory<ArtisticNoiseRecord>("ArtisticNoiseRecord", "Combines many textures with varying parameters to create an easily controlled tileable noise");
        REGISTER_RESOURCE(ArtisticNoiseRecord, BitmapResource, GetBitmapResourceHandle, SetBitmapResourceHandle, GetImageData, SetImageData, ResourceHandle("Image"), "Bitmap", "Bitmap to use for each splat", PS_VisualConsequence);
        REGISTER_PROPERTY_MEMORY(ArtisticNoiseRecord, unsigned, offsetof(ArtisticNoiseRecord, Splats), 1, "Splats", "The number of splats that will be placed", PS_VisualConsequence);
        REGISTER_PROPERTY_MEMORY(ArtisticNoiseRecord, float, offsetof(ArtisticNoiseRecord, BlendPower), 1.0f, "Blend Power", "Biases blending so that overlaps contribute less", PS_VisualConsequence | PS_TinyIncrement);
        REGISTER_PROPERTY_MEMORY(ArtisticNoiseRecord, RangedFloat, offsetof(ArtisticNoiseRecord, Offset), RangedFloat(0, 0), "Offset", "Random offset range to apply to splats", PS_VisualConsequence | PS_Translation);
        REGISTER_PROPERTY_MEMORY(ArtisticNoiseRecord, RangedFloat, offsetof(ArtisticNoiseRecord, RotationRange), RangedFloat(0, 0), "Rotation", "Random rotation range to apply to splats", PS_VisualConsequence | PS_Rotation);
        REGISTER_PROPERTY_MEMORY(ArtisticNoiseRecord, RangedFloat, offsetof(ArtisticNoiseRecord, ScaleRange), RangedFloat(1, 1), "Scale", "Random scale range to apply to splats", PS_VisualConsequence | PS_Scale);
    }

    void ArtisticNoise::Register(Context* context)
    {
        ArtisticNoiseRecord::Register(context);
        COPY_PROPERTIES(GraphNode, ArtisticNoise);
        REGISTER_ENUM_MEMORY(ArtisticNoise, int, offsetof(ArtisticNoise, Blending), 0, "Blending", "Method to use for blending each texture pass together", PS_VisualConsequence, ArtisticNoiseBlendNames);
        REGISTER_EDITABLE(ArtisticNoise, GetRecords, SetRecords, "Textures", "", PS_VisualConsequence | PS_IEditableList, ArtisticNoiseRecordNameList, ArtisticNoiseRecordHashes);
        //REGISTER_LIST(ArtisticNoise, GetRecords, SetRecords, "Textures", "", PS_VisualConsequence, ArtisticNoiseRecordNameList, ArtisticNoiseRecordHashes);
        //REGISTER_EDITABLE_LIST(ArtisticNoise, ArtisticNoiseRecord, GetRecords, SetRecords, "Textures", "", PS_VisualConsequence, ArtisticNoiseRecordNameList, ArtisticNoiseRecordHashes);
    }

    void ArtisticNoise::Construct()
    {
        AddOutput("Result", TEXGRAPH_CHANNEL);
    }

    int ArtisticNoise::Execute(const Variant& param)
    {
        RGBA existingValue;
        for (unsigned i = 0; i < Records.size(); ++i)
        {
            RGBA value = Sample(Records[i], param, i);
            if (i == 0)
            {
                if (Blending == ANB_Smart)
                    existingValue.Set(DENORMALIZE(value.r, -1, 1), DENORMALIZE(value.g, -1, 1), DENORMALIZE(value.b, -1, 1), DENORMALIZE(value.a, -1, 1));
                else
                    existingValue = value;
            }
            else
            {
                if (Blending == ANB_AlternatingAddSub)
                {
                    if (i % 2)
                        existingValue -= value;
                    else
                        existingValue += value;
                }
                else
                    existingValue += RGBA(DENORMALIZE(value.r, -1, 1), DENORMALIZE(value.g, -1, 1), DENORMALIZE(value.b, -1, 1), DENORMALIZE(value.a, -1, 1));
            }
        }

        existingValue.r = NORMALIZE(existingValue.r, -1, 1);
        existingValue.g = NORMALIZE(existingValue.g, -1, 1);
        existingValue.b = NORMALIZE(existingValue.b, -1, 1);
        existingValue.a = NORMALIZE(existingValue.a, -1, 1);
        existingValue.Clip();
        GetOutputSocket(0)->StoreValue(existingValue);
        return GRAPH_EXECUTE_COMPLETE;
    }

    Variant ArtisticNoise::GetRecords() const
    {
        VariantVector ret;
        for (auto item : Records)
            ret.push_back(Variant((void*)item));
        return ret;
    }

    void ArtisticNoise::SetRecords(Variant editables)
    {
        VariantVector list = editables.getVariantVector();
        Records.clear();
        for (auto item : list)
            Records.push_back((ArtisticNoiseRecord*)item.getVoidPtr());
    }

    RGBA ArtisticNoise::Sample(ArtisticNoiseRecord* record, const Variant& param, unsigned idx) const
    {
        if (record && record->ImageData)
        {
            Vec3 offset(5 * (idx * 31 + 1), 7 * (idx * 71 + 1), 11);
            Vec2 pos = param.getVec2Safe();
            RGBA current = RGBA(0, 0, 0, 0);
            for (int np = 0; np < record->Splats; ++np)
            {
#define GET_RAND_VAL(NAME) float NAME = noise_.GetNoise(offset.x, offset.y * 67, np); \
                offset *= 1.2f; \
                NAME *= 2.0f; NAME += 1.0f;
                //NORMALIZE(NAME, -1, 2);

                GET_RAND_VAL(px);   // X offset
                GET_RAND_VAL(py);   // Y offset
                GET_RAND_VAL(r);    // rotation
                GET_RAND_VAL(s);    // scale
#undef GET_RAND_VAL
                s = NORMALIZE(s, -1, 1) * record->ScaleRange.GetRange();
                r = record->RotationRange.Clip(r * 359.0f * 0.5f);
                s = record->ScaleRange.Clip(s);
                Mat3x3 transMat; transMat.SetTranslation(Vec2(px, py));
                Mat3x3 rotMat; rotMat.SetRotation(r);
                Vec3 coord = rotMat * Vec3((pos.x + px) * s, (pos.y + py) * s, 0.0f);//(transMat * rotMat) * Vec3(pos.x ,pos.y, 0.0f);
                coord.x = fmodf(coord.x, 1.0f);
                while (coord.x < 0.0f)
                    coord.x += 1.0f;
                coord.y = fmodf(coord.y, 1.0f);
                while (coord.y < 0.0f)
                    coord.y += 1.0f;

                RGBA newColor = record->ImageData->GetImage()->getBilinear(coord.x, coord.y);
                if (newColor.a >= current.a)
                    current = newColor;
            }

            return current;
        }
        return RGBA(0,0,0,0);
    }
}