#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace PlayFab
{
    typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

    inline TimePoint GetTimePointNow()
    {
        return std::chrono::system_clock::now();
    }

    inline time_t GetPlayFabTimeTNow()
    {
        return time(0);
    }

    inline std::string LocalTimeTToUtcString(time_t now)
    {
        tm timeInfo;
#if defined(PLAYFAB_PLATFORM_WINDOWS) || defined(PLAYFAB_PLATFORM_XBOX)
        gmtime_s(&timeInfo, &now);
#elif defined(PLAYFAB_PLATFORM_LINUX) || defined(PLAYFAB_PLATFORM_IOS) || defined(PLAYFAB_PLATFORM_ANDROID) || defined(PLAYFAB_PLATFORM_PLAYSTATION)
        timeInfo = *gmtime(&now);
#endif
        char buff[64];
        strftime(buff, 64, "%Y-%m-%dT%H:%M:%S.000Z", &timeInfo);

        return buff;
    }

    inline std::string LocalTimePointToUtcString(TimePoint now)
    {
        return LocalTimeTToUtcString(std::chrono::system_clock::to_time_t(now));
    }

    inline time_t UtcStringToLocalTimeT(const std::string& utcString)
    {
        time_t output;
        tm timeStruct = {};

        std::istringstream iss(utcString);
        iss >> std::get_time(&timeStruct, "%Y-%m-%dT%T");
        timeStruct.tm_isdst = 0;  // 0 means "not in DST" PlayFab assumes UTC/Zulu always
#if defined(PLAYFAB_PLATFORM_PLAYSTATION)
        output = mktime(&timeStruct);
#elif defined(PLAYFAB_PLATFORM_IOS) || defined(PLAYFAB_PLATFORM_ANDROID) || defined(PLAYFAB_PLATFORM_LINUX)
        output = gmtime_r(&timeStruct);
#else
        output = _mkgmtime(&timeStruct);
#endif

        return output;
    }
}
