#include "StoryTeller.h"

#include "../Utilis//UtilisFunctionLibery.h"
#include "../UIWidgets/ImageBase.h"

StoryTeller::StoryTeller(Actor* talkTarget, std::vector<ImageBase*> imgConversation)
    : mTalkTarget(talkTarget)
    , mImgConversation(std::move(imgConversation))
{
}

void StoryTeller::Update(ArgsForUpdate args)
{
    auto* owner = args.Owner;
    if (ActorUtilis::DistanceBetweenActors(owner, mTalkTarget) < TALK_DISTANCE)
    {
        SetConversationActive(true);
    }
    else
    {
        SetConversationActive(false);
    }
}

void StoryTeller::Initialize(ArgsForInit args)
{
}

void StoryTeller::SetConversationActive(bool isOn)
{
    for (auto& img : mImgConversation)
    {
        img->Visible = isOn;
    }
}
