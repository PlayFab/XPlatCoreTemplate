
#include "TestAppPch.h"


namespace PlayFabUnit
{

    // GROUPS API
    void PlayFabGroupsTest::GroupsApiTest(TestContext& testContext)
    {
        auto settings = std::make_shared<PlayFabApiSettings>();

        auto req = PlayFab::ClientModels::LoginWithCustomIDRequest();
        req.CustomId = PlayFabSettings::buildIdentifier;

        clientApi->LoginWithCustomID(req, Callback(&PlayFabApiTest::GroupsTestLoginCallback), Callback(&PlayFabApiTest::GroupsTestLoginFailedCallback), &testContext);
    }

    void PlayFabGroupsTest::GroupsTestLoginCallback(const LoginResult& result, void* customData)
    {
        auto groupApi = std::make_shared<PlayFab::PlayFabGroupsInstanceAPI>(result.authenticationContext);
        auto req = PlayFab::GroupsModels::CreateGroupRequest();
        req.CustomTags = std::map<std::string, std::string>();

        // TODO Bug 29786037: this map is required to be filled to not get a 500 error with the CreateGroup api call
        req.CustomTags.insert(std::pair<std::string, std::string>("One", "Two"));

        req.GroupName = PlayFabUnit::GenerateUuidV4();
        groupApi->CreateGroup(req, Callback(&PlayFabApiTest::GroupsTestGroupCallback), Callback(&PlayFabApiTest::GroupsTestLoginFailedCallback), customData);
    }

    void PlayFabGroupsTest::GroupsTestGroupCallback(const PlayFab::GroupsModels::CreateGroupResponse& response, void* customData)
    {
        TestContext* testContext = static_cast<TestContext*>(customData);
        testContext->Pass("CreateGroups succeeded and made the new group: " + response.GroupName);
    }

    void PlayFabGroupsTest::GroupsTestLoginFailedCallback(const PlayFabError& error, void* customData)
    {
        TestContext* testContext = static_cast<TestContext*>(customData);
        testContext->Fail("Expected Group Login to succeed. Got error " + error.ErrorMessage);
    }

    //
    //
    // Add test calls to this method, after implementation
    //
    //
    void PlayFabGroupsTest::AddTests()
    {
        // AddTest("InvalidSettings", &PlayFabApiTest::InvalidSettings);
        AddTest("CreateGroupApi", &PlayFabApiTest::GroupsApiTest);
    }

    void PlayFabGroupsTest::ClassSetUp()
    {
        clientApi = std::make_shared<PlayFabClientInstanceAPI>(PlayFabSettings::staticPlayer);
        PlayFabSettings::staticSettings->titleId = testTitleData.titleId;
        USER_EMAIL = testTitleData.userEmail;

        // Verify all the inputs won't cause crashes in the tests
        TITLE_INFO_SET = !PlayFabSettings::staticSettings->titleId.empty() && !USER_EMAIL.empty();

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