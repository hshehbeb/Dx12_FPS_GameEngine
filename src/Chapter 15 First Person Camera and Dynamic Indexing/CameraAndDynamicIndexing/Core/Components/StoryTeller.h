#pragma once
#include "../Actors/Actor.h"
#include "../UIWidgets/ImageBase.h"

class StoryTeller : public IComponent
{
public:
    StoryTeller(Actor* talkTarget, std::vector<ImageBase*> imgConversation);

    void Update(Actor* owner) override;
    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        RenderItemsList& rItemsList, Actor* owner) override;

private:
#define TALK_DISTANCE 20.0f

private:
    Actor* mTalkTarget;
    std::vector<ImageBase*> mImgConversation;

private:
    void SetConversationActive(bool isOn);
};
