#include "StoryTeller.h"

#include "../../ActorUtilis.h"
#include "../UIWidgets/Image.h"

StoryTeller::StoryTeller(Actor* talkTarget, std::vector<Image*> imgConversation)
    : mTalkTarget(talkTarget)
    , mImgConversation(std::move(imgConversation))
{
}

void StoryTeller::Update(Actor* owner)
{
    if (ActorUtilis::DistanceBetweenActors(owner, mTalkTarget) < TALK_DISTANCE)
    {
        SetConversationActive(true);
    }
    else
    {
        SetConversationActive(false);
    }
}

void StoryTeller::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList)
{
}

void StoryTeller::SetConversationActive(bool isOn)
{
    for (auto& img : mImgConversation)
    {
        img->shouldDraw = isOn;
    }
}
