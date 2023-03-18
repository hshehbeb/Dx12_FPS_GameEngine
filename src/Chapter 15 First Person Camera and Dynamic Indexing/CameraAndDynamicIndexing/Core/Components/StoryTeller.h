#pragma once
#include "../Actors/Actor.h"
#include "../UIWidgets/Image.h"

class StoryTeller : public IComponent
{
public:
    StoryTeller(Actor* talkTarget, std::vector<Image*> imgConversation);

    void Update(Actor* owner) override;
    void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, RenderItemsList& rItemsList) override;

private:
#define TALK_DISTANCE 20.0f

private:
    Actor* mTalkTarget;
    std::vector<Image*> mImgConversation;

private:
    void SetConversationActive(bool isOn);
};
