
#include "TestAppPch.h"

#include "PlayFabGroupsTest.h"
#include "playfab/PlayFabApiSettings.h"
#include "playfab/PlayFabSettings.h"

using namespace PlayFab;

namespace PlayFabUnit
{

    // GROUPS API
    void PlayFabGroupsTest::GroupsLoginTest(TestContext& testContext)
    {
        auto req = ClientModels::LoginWithCustomIDRequest();
        req.CustomId = PlayFabSettings::buildIdentifier;

        clientApi->LoginWithCustomID(req, Callback(&PlayFabGroupsTest::GroupsLoginCallback), Callback(&PlayFabGroupsTest::GroupsTestSharedFailureCallback), &testContext);
    }

    void PlayFabGroupsTest::GroupsLoginCallback(const ClientModels::LoginResult&, void* customData)
    {
        TestContext* testContext = static_cast<TestContext*>(customData);
        testContext->Pass();
    }

    void PlayFabGroupsTest::GroupsCreateTest(TestContext& testContext)
    {
        auto req = ClientModels::LoginWithCustomIDRequest();
        req.CustomId = PlayFabSettings::buildIdentifier;

        clientApi->LoginWithCustomID(req, Callback(&PlayFabGroupsTest::GroupsCreateLoginCallback), Callback(&PlayFabGroupsTest::GroupsTestSharedFailureCallback), &testContext);
    }

    void PlayFabGroupsTest::GroupsCreateLoginCallback(const ClientModels::LoginResult&, void* customData)
    {
        auto req = GroupsModels::CreateGroupRequest();
        req.CustomTags = std::map<std::string, std::string>();

        // TODO Bug 29786037: this map is required to be filled to not get a 500 error with the CreateGroup api call
        req.CustomTags.insert(std::pair<std::string, std::string>("One", "Two"));

        req.GroupName = groupName;
        groupsApi->CreateGroup(req, Callback(&PlayFabGroupsTest::GroupsCreateTestSuccessCallback), Callback(&PlayFabGroupsTest::GroupsTestSharedFailureCallback), customData);
    }

    void PlayFabGroupsTest::GroupsCreateTestSuccessCallback(const GroupsModels::CreateGroupResponse& response, void* customData)
    {
        createdGroupKey = response.Group;
        TestContext* testContext = static_cast<TestContext*>(customData);
        testContext->Pass();
    }

    void PlayFabGroupsTest::GroupsRemoveTest(TestContext& testContext)
    {
        auto req = ClientModels::LoginWithCustomIDRequest();
        req.CustomId = PlayFabSettings::buildIdentifier;

        clientApi->LoginWithCustomID(req, Callback(&PlayFabGroupsTest::GroupsRemoveLoginCallback), Callback(&PlayFabGroupsTest::GroupsTestSharedFailureCallback), &testContext);
    }

    void PlayFabGroupsTest::GroupsRemoveLoginCallback(const ClientModels::LoginResult&, void* customData)
    {
        auto req = GroupsModels::DeleteGroupRequest();
        req.CustomTags = std::map<std::string, std::string>();

        // CHECK IS THIS STILL REQUIRED?
        // TODO Bug 29786037: this map is required to be filled to not get a 500 error with the CreateGroup api call
        req.CustomTags.insert(std::pair<std::string, std::string>("One", "Two"));

        req.Group = createdGroupKey;
        groupsApi->DeleteGroup(req, Callback(&PlayFabGroupsTest::GroupsRemoveTestSuccessCallback), Callback(&PlayFabGroupsTest::GroupsTestSharedFailureCallback), customData);
    }

    void PlayFabGroupsTest::GroupsRemoveTestSuccessCallback(const GroupsModels::EmptyResponse&, void* customData)
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
        AddTest("GroupsLoginTest", &PlayFabGroupsTest::GroupsLoginTest);
        AddTest("GroupsCreateTest", &PlayFabGroupsTest::GroupsCreateTest);
        AddTest("GroupsRemoveTest", &PlayFabGroupsTest::GroupsRemoveTest);
    }

    void PlayFabGroupsTest::ClassSetUp()
    {
        groupName = GenerateRandomString();

        groupsTestSettings = std::make_shared<PlayFabApiSettings>();
        groupsTestSettings->titleId = testTitleData.titleId;

        clientApi = std::make_shared<PlayFabClientInstanceAPI>(groupsTestSettings);

        auto req = ClientModels::LoginWithCustomIDRequest();
        req.CustomId = PlayFabSettings::buildIdentifier;

        clientApi->LoginWithCustomID(req,
                Callback(&PlayFabGroupsTest::ClassSetupLoginSucceeded),
                Callback(&PlayFabGroupsTest::ClassSetupLoginFailed));

        // Verify all the inputs won't cause crashes in the tests
        TITLE_INFO_SET = testTitleData.titleId != "";

        // Make sure PlayFab state is clean.
        PlayFabSettings::ForgetAllCredentials();
    }

    void PlayFabGroupsTest::ClassSetupLoginSucceeded(const PlayFab::ClientModels::LoginResult& result, void* )
    {
        groupsApi = std::make_shared<PlayFabGroupsInstanceAPI>(groupsTestSettings, result.authenticationContext);
    }

    void PlayFabGroupsTest::ClassSetupLoginFailed(const PlayFab::PlayFabError& error, void*)
    {
        throw PlayFabException(PlayFabExceptionCode::NotLoggedIn, error.GenerateErrorReport().c_str());
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