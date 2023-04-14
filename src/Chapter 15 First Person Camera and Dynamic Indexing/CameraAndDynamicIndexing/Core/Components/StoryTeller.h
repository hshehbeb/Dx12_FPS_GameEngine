#pragma once
#include "../Actors/Actor.h"
#include "../UIWidgets/ImageBase.h"

class StoryTeller : public IComponent
{
public:
    StoryTeller(Actor* talkTarget, int dialogId, float triggerDist = 10);

    void Update(ArgsForUpdate args) override;
    void Initialize(ArgsForInit args) override;

private:
    Actor* mTalkTarget;
    int mDialog;
    float mTriggerDistance;
    int mPrevTriggeredDlg;

// private:
//     void SetConversationActive(bool isOn);
};
