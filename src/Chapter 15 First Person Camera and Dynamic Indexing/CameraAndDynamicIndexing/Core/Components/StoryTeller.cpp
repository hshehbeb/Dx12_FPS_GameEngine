#include "StoryTeller.h"

#include "../../Scripter.h"
#include "../Utilis//UtilisFunctionLibery.h"
#include "../UIWidgets/ImageBase.h"


namespace /* CONSTANTS */
{
    static const int NULL_DIALOG = -1;
}

StoryTeller::StoryTeller(Actor* talkTarget, int dialogId, float triggerDist)
    : mTalkTarget(talkTarget)
    , mDialog(dialogId)
    , mTriggerDistance(triggerDist)
    , mPrevTriggeredDlg(NULL_DIALOG)
{
}

void StoryTeller::Update(ArgsForUpdate args)
{
    auto* owner = args.Owner;
    bool closeEnough =
        ActorUtilis::DistanceBetweenActors(owner, mTalkTarget) < mTriggerDistance;
    bool notTriggered = (mPrevTriggeredDlg != mDialog);
    
    if (closeEnough && notTriggered)
    {
        Resources::gScripter->ShowDialog(mDialog, {});
        mPrevTriggeredDlg = mDialog;
    }
}

void StoryTeller::Initialize(ArgsForInit args)
{
}

// void StoryTeller::SetConversationActive(bool isOn)
// {
//     for (auto& img : mImgConversation)
//     {
//         img->Visible = isOn;
//     }
// }
