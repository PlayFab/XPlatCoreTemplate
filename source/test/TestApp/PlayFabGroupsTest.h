#pragma once

#if !defined(DISABLE_PLAYFABCLIENT_API)

#include "TestCase.h"
#include <playfab/PlayFabClientInstanceApi.h>

namespace PlayFabUnit
{
    class PlayFabGroupsTest : public PlayFabApiTestCase
    {
    private:
        void GroupsApiTest(TestContext& testContext);

    protected:
        void AddTests() override;

    public:
        std::shared_ptr<PlayFab::PlayFabClientInstanceAPI> clientApi;

        void ClassSetUp() override;
        void SetUp(TestContext& testContext) override;
        void Tick(TestContext& testContext) override;
        void TearDown(TestContext& testContext) override;
        void ClassTearDown() override;
    };
}

#endif