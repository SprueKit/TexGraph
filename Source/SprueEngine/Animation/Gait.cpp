#include "Gait.h"

#include <SprueEngine/Math/MathDef.h>

namespace SprueEngine
{

void LegState::Update(float phaseDelta, float phasePeriod, float lastGaitPhase, float newGaitPhase)
{
    // Is our step activation inside
    if (!inStepPhase_ && SprueBetweenInclusive(data_->relativePhase_, lastGaitPhase, newGaitPhase))
    {
        inStepPhase_ = true;
        stepPhase_ = 0.0f;
        //TODO: set the take-off position
    }

    if (inStepPhase_)
    {
        stepPhase_ += phaseDelta;
        transferPhase_ = NORMALIZE(stepPhase_, 0.0f, phasePeriod);
        if (stepPhase_ > 1.0f - data_->dutyFactor_)
        {
            stepPhase_ = 0.0f;
            transferPhase_ = 1.0f;
            inStepPhase_ = false;
        }
    }
}

GaitState::GaitState(GaitData* gait)
{
    gait_ = gait;
    for (unsigned i = 0; i < gait_->steppingData_.size(); ++i)
    {
        LegState* state = new LegState();
        state->data_ = gait->steppingData_[i];
        legStates_.push_back(state);
    }
}

GaitState::GaitState(const GaitState& rhs)
{
    gait_ = rhs.gait_;
    if (!rhs.legStates_.empty())
    {
        for (unsigned i = 0; i < rhs.legStates_.size(); ++i)
        {
            LegState* ls = new LegState();
            ls->data_ = rhs.legStates_[i]->data_;
            ls->inStepPhase_ = rhs.legStates_[i]->inStepPhase_;
            ls->stepPhase_ = rhs.legStates_[i]->stepPhase_;
            ls->takeOff_ = rhs.legStates_[i]->takeOff_;
            ls->transferPhase_ = rhs.legStates_[i]->transferPhase_;
        }
    }
}

GaitState::~GaitState()
{
    for (auto state : legStates_)
        delete state;
}

void GaitState::Update(float deltaTime)
{
    if (legStates_.empty())
        return;

    // Select an appropriate gait and gait period give the velocity
    const float lastPhase = gaitPhase_;
    const float phaseDelta = deltaTime / gaitPeriod_;
    gaitPhase_ += phaseDelta;

    for (auto ls : legStates_)
        ls->Update(phaseDelta, gaitPeriod_, lastPhase, gaitPhase_);

    if (gaitPhase_ > 1.0f)
        gaitPhase_ = fmodf(gaitPhase_, 1.0f);
}

}