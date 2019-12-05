// Copyright (C) Microsoft Corporation. All rights reserved.

#pragma once

#include "TestCase.h"

namespace PlayFabUnit
{
    class PlayFabPlatformSpecificTest : public TestCase
    {
    private:
        void TestPlatformSpecificLogin(TestContext& testContext);

    protected:
        void AddTests() override;

    public:
        void SetTitleInfo(const TestTitleData& testTitleData);

        void ClassSetUp() override;
        void SetUp(TestContext& testContext) override;
        void Tick(TestContext& testContext) override;
        void TearDown(TestContext& testContext) override;
        void ClassTearDown() override;
    };
}
