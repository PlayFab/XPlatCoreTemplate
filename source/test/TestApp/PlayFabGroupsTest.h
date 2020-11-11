#pragma once

#if !defined(DISABLE_PLAYFABCLIENT_API)

#include "TestCase.h"
#include <playfab/PlayFabClientInstanceApi.h>

#include "playfab/PlayFabClientDataModels.h"
#include "playfab/PlayFabGroupsInstanceApi.h"
#include "playfab/PlayFabGroupsDataModels.h"


#include "playfab/PlayFabApiSettings.h"

namespace PlayFabUnit
{
    class PlayFabGroupsTest : public PlayFabApiTestCase
    {
    private:
        void GroupsTestSharedFailureCallback(const PlayFab::PlayFabError& error, void* customData);

        void GroupsLoginTest(TestContext& testContext);
        void GroupsLoginCallback(const PlayFab::ClientModels::LoginResult& result, void* customData);

        void GroupsCreateTest(TestContext& testContext);
        void GroupsCreateTestSuccessCallback(const PlayFab::GroupsModels::CreateGroupResponse& response, void* customData);

        void GroupsRemoveTest(TestContext& testContext);
        void GroupsRemoveTestSuccessCallback(const PlayFab::GroupsModels::EmptyResponse& response, void* customData);

        // Utility
        template<typename T> std::function<void(const T&, void*)> Callback(void(PlayFabGroupsTest::* func)(const T&, void*))
        {
            return std::bind(func, this, std::placeholders::_1, std::placeholders::_2);
        }

    protected:
        std::shared_ptr<PlayFab::PlayFabClientInstanceAPI> clientApi;
        std::shared_ptr<PlayFab::PlayFabGroupsInstanceAPI> groupsApi;

        PlayFab::GroupsModels::EntityKey createdGroupKey;

        void AddTests() override;

    public:
       void ClassSetUp() override;
        void SetUp(TestContext& testContext) override;
        void Tick(TestContext& testContext) override;
        void ClassTearDown() override;
    };
}

#endif
