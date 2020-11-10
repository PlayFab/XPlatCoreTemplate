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
        // Fixed values provided from testInputs
        bool TITLE_INFO_SET;
        
        std::shared_ptr<PlayFab::PlayFabApiSettings> groupsTestSettings;

        void ClassSetupLoginSucceeded(const PlayFab::ClientModels::LoginResult&, void*);
        void ClassSetupLoginFailed(const PlayFab::PlayFabError& error, void*);

        void GroupsLoginTest(TestContext& testContext);
        void GroupsCreateTest(TestContext& testContext);
        void GroupsRemoveTest(TestContext& testContext);

        // Utility
        template<typename T> std::function<void(const T&, void*)> Callback(void(PlayFabGroupsTest::* func)(const T&, void*))
        {
            return std::bind(func, this, std::placeholders::_1, std::placeholders::_2);
        }

        void GroupsLoginCallback(const PlayFab::ClientModels::LoginResult& result, void* customData);
        void GroupsCreateLoginCallback(const PlayFab::ClientModels::LoginResult& result, void* customData);
        void GroupsRemoveLoginCallback(const PlayFab::ClientModels::LoginResult& result, void* customData);

        void GroupsCreateTestSuccessCallback(const PlayFab::GroupsModels::CreateGroupResponse& response, void* customData);
        void GroupsRemoveTestSuccessCallback(const PlayFab::GroupsModels::EmptyResponse& response, void* customData);
        
        void GroupsTestSharedFailureCallback(const PlayFab::PlayFabError& error, void* customData);

    protected:
        std::shared_ptr<PlayFab::PlayFabClientInstanceAPI> clientApi;
        std::shared_ptr<PlayFab::PlayFabGroupsInstanceAPI> groupsApi;

        std::string groupName;
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