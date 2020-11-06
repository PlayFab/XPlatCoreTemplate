// Copyright (C) Microsoft Corporation. All rights reserved.

#include "TestAppPch.h"

namespace PlayFabUnit
{
    // source: https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
    // note this is not sufficiently random enough to be guid's, but random strings is the original need for our tests.
    std::string GenerateRandomString() {
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::uniform_int_distribution<> dis2(8, 11);

        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }

    const char* ToString(TestActiveState state)
    {
        switch (state)
        {
            case TestActiveState::PENDING: return "PENDING";
            case TestActiveState::ACTIVE: return "ACTIVE";
            case TestActiveState::READY: return "READY";
            case TestActiveState::COMPLETE: return "COMPLETE";
            case TestActiveState::ABORTED: return "ABORTED";
            default: return "UNKNOWN";
        }
    }

    const char* ToString(TestFinishState state)
    {
        switch (state)
        {
            case TestFinishState::PENDING: return "PENDING";
            case TestFinishState::PASSED: return "PASSED";
            case TestFinishState::FAILED: return "FAILED";
            case TestFinishState::SKIPPED: return "SKIPPED";
            case TestFinishState::TIMEDOUT: return "TIMEDOUT";
            default: return "UNKNOWN";
        }
    }
}
