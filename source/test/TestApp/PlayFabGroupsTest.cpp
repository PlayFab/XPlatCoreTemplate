
#include "TestAppPch.h"

#include "PlayFabGroupsTest.h"
#include "playfab/PlayFabApiSettings.h"
#include "playfab/PlayFabSettings.h"

using namespace PlayFab;

namespace PlayFabUnit
{

    // GROUPS API
    void PlayFabGroupsTest::GroupsApiTest(TestContext& testContext)
    {
        auto settings = std::make_shared<PlayFabApiSettings>();

        auto req = ClientModels::LoginWithCustomIDRequest();
        req.CustomId = PlayFabSettings::buildIdentifier;

        clientApi->LoginWithCustomID(req, Callback(&PlayFabGroupsTest::GroupsTestLoginCallback), Callback(&PlayFabGroupsTest::GroupsTestSharedFailureCallback), &testContext);
    }

    void PlayFabGroupsTest::GroupsTestLoginCallback(const ClientModels::LoginResult& result, void* customData)
    {
        auto groupApi = std::make_shared<PlayFabGroupsInstanceAPI>(result.authenticationContext);
        auto req = GroupsModels::CreateGroupRequest();
        req.CustomTags = std::map<std::string, std::string>();

        // TODO Bug 29786037: this map is required to be filled to not get a 500 error with the CreateGroup api call
        req.CustomTags.insert(std::pair<std::string, std::string>("One", "Two"));

        req.GroupName = GenerateRandomString();
        groupApi->CreateGroup(req, Callback(&PlayFabGroupsTest::GroupsTestGroupCallback), Callback(&PlayFabGroupsTest::GroupsTestSharedFailureCallback), customData);
    }

    void PlayFabGroupsTest::GroupsTestGroupCallback(const GroupsModels::CreateGroupResponse&, void* customData)
    {
        TestContext* testContext = static_cast<TestContext*>(customData);
        testContext->Pass();
    }

    void PlayFabGroupsTest::GroupsTestSharedFailureCallback(const PlayFabError& error, void* customData)
    {
        TestContext* testContext = static_cast<TestContext*>(customData);
        testContext->Fail("Expected call to succeed but instead got the error: " + error.GenerateErrorReport());
    }

    void PlayFabGroupsTest::AddTests()
    {
        AddTest("CreateGroupApi", &PlayFabGroupsTest::GroupsApiTest);
    }

    void PlayFabGroupsTest::ClassSetUp()
    {
        clientApi = std::make_shared<PlayFabClientInstanceAPI>(PlayFabSettings::staticPlayer);
        PlayFabSettings::staticSettings->titleId = testTitleData.titleId;

        // Verify all the inputs won't cause crashes in the tests
        TITLE_INFO_SET = !PlayFabSettings::staticSettings->titleId.empty();

        // Make sure PlayFab state is clean.
        PlayFabSettings::ForgetAllCredentials();
    }

    void PlayFabGroupsTest::SetUp(TestContext& testContext)
    {
        if (!TITLE_INFO_SET)
        {
            testContext.Skip(); // We cannot do client tests if the titleId is not given
        }
    }

    void PlayFabGroupsTest::Tick(TestContext& /*testContext*/)
    {
        // No work needed, async tests will end themselves
    }

    void PlayFabGroupsTest::ClassTearDown()
    {
        // Clean up any PlayFab state for next TestCase.
        PlayFabSettings::ForgetAllCredentials();
    }
}