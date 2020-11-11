
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
        auto req = GroupsModels::CreateGroupRequest();
        req.CustomTags = std::map<std::string, std::string>();

        // TODO Bug 29786037: this map is required to be filled to not get a 500 error with the CreateGroup api call
        req.CustomTags.insert(std::pair<std::string, std::string>("One", "Two"));

        req.GroupName = GenerateRandomString();
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
        // Make sure PlayFab state is clean.
        PlayFabSettings::ForgetAllCredentials();

        auto context = std::make_shared<PlayFabApiSettings>();
        auto settings = std::make_shared<PlayFabApiSettings>();
        groupsTestSettings->titleId = testTitleData.titleId;

        clientApi = std::make_shared<PlayFabClientInstanceAPI>(settings, context);
        groupsApi = std::make_shared<PlayFabGroupsInstanceAPI>(settings, context);
    }

    void PlayFabGroupsTest::ClassSetupLoginSucceeded(const PlayFab::ClientModels::LoginResult& result, void* )
    {
    }

    void PlayFabGroupsTest::SetUp(TestContext& testContext)
    {
        if (testTitleData.titleId == "")
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