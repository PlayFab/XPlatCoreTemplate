#pragma once

#if !defined(DISABLE_PLAYFABCLIENT_API)

#include "TestCase.h"
#include <playfab/PlayFabClientInstanceApi.h>

#include "playfab/PlayFabClientDataModels.h"
#include "playfab/PlayFabGroupsInstanceApi.h"
#include "playfab/PlayFabGroupsDataModels.h"

namespace PlayFabUnit
{
    class PlayFabGroupsTest : public PlayFabApiTestCase
    {
    private:
        // Fixed values provided from testInputs
        bool TITLE_INFO_SET;

        void GroupsApiTest(TestContext& testContext);

        // Utility
        template<typename T> std::function<void(const T&, void*)> Callback(void(PlayFabGroupsTest::* func)(const T&, void*))
        {
            return std::bind(func, this, std::placeholders::_1, std::placeholders::_2);
        }

        void GroupsTestLoginCallback(const PlayFab::ClientModels::LoginResult& result, void* customData);
        void GroupsTestGroupCallback(const PlayFab::GroupsModels::CreateGroupResponse& response, void* customData);
        void GroupsTestSharedFailureCallback(const PlayFab::PlayFabError& error, void* customData);

    protected:
        void AddTests() override;

    public:
        std::shared_ptr<PlayFab::PlayFabClientInstanceAPI> clientApi;

        void ClassSetUp() override;
        void SetUp(TestContext& testContext) override;
        void Tick(TestContext& testContext) override;
        void ClassTearDown() override;
    };
}

#endif