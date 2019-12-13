#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace PlayFab
{
#if defined(PLAYFAB_PLATFORM_SWITCH)
    static_assert("You must request the Nintendo specific XPlat SDK from PlayFab support.");
#else
    typedef std::chrono::system_clock Clock;
    typedef std::chrono::time_point<Clock> TimePoint;
#endif

    // The primary purpose of these format strings is to communicate to and from the PlayFab server with consistent accuracy across platforms supported by this SDK
    constexpr char TIMESTAMP_READ_FORMAT[] = "%Y-%m-%dT%T";
    constexpr char TIMESTAMP_WRITE_FORMAT[] = "%Y-%m-%dT%H:%M:%S.000Z";

    // Initialize may be required on some platforms
    inline void InitializeClock()
    {
#if defined(PLAYFAB_PLATFORM_SWITCH)
        static_assert("You must request the Nintendo specific XPlat SDK from PlayFab support.");
#endif
    }

    // Time type conversions
    inline time_t TimePointToTimeT(TimePoint input)
    {
        return Clock::to_time_t(input);
    }

    inline TimePoint TimeTToTimePoint(time_t input)
    {
        return Clock::from_time_t(input);
    }

    inline tm TimeTToUtcTm(time_t input)
    {
        tm timeInfo;
#if defined(PLAYFAB_PLATFORM_WINDOWS) || defined(PLAYFAB_PLATFORM_XBOX)
#define gmtime_r gmtime_s
#endif
        gmtime_r(&input, &timeInfo);
        return timeInfo;
    }

    inline time_t UtcTmToTimeT(tm input)
    {
#if defined(PLAYFAB_PLATFORM_WINDOWS) || defined(PLAYFAB_PLATFORM_XBOX)
#define timegm _mkgmtime
#endif
        return timegm(&input);
    }

    inline tm TimePointToUtcTm(TimePoint input)
    {
        return TimeTToUtcTm(Clock::to_time_t(input));
    }

    inline TimePoint UtcTmToTimePoint(tm input)
    {
        return TimeTToTimePoint(UtcTmToTimeT(input));
    }

    // Get Time now - Platform dependent granularity (granularity: upto 1 second, accuracy within a few seconds)
    inline TimePoint GetTimePointNow()
    {
        // The conversion is mostly to ensure consistent behavior among all platforms
        return std::chrono::time_point_cast<std::chrono::seconds>(Clock::now());
    }

    inline time_t GetTimeTNow()
    {
        return TimePointToTimeT(GetTimePointNow());
    }

    // Get a tick count that represents now in milliseconds (not useful for absolute time)
    inline long GetMilliTicks()
    {
#if defined(PLAYFAB_PLATFORM_SWITCH)
        static_assert("You must request the Nintendo specific XPlat SDK from PlayFab support.");
#else
        auto msClock = std::chrono::time_point_cast<std::chrono::milliseconds>(Clock::now());
        return msClock.time_since_epoch().count();
#endif
    }

    // Time Serialization
    inline std::string UtcTmToIso8601String(tm input)
    {
        char buff[64];
        strftime(buff, 64, TIMESTAMP_WRITE_FORMAT, &input);
        return buff;
    }

    inline tm Iso8601StringToTm(const std::string& utcString)
    {
        tm timeInfo;
        std::istringstream iss(utcString);
        iss >> std::get_time(&timeInfo, TIMESTAMP_READ_FORMAT);
        return timeInfo;
    }

    inline std::string TimeTToIso8601String(time_t input)
    {
        return UtcTmToIso8601String(TimeTToUtcTm(input));
    }

    inline time_t Iso8601StringToTimeT(std::string input)
    {
        return UtcTmToTimeT(Iso8601StringToTm(input));
    }

    // TODO: Invert this conversion at some point, and serialize the milliseconds as well
    inline std::string TimePointToIso8601String(TimePoint input)
    {
        return UtcTmToIso8601String(TimePointToUtcTm(input));
    }

    inline TimePoint Iso8601StringToTimePoint(std::string input)
    {
        return UtcTmToTimePoint(Iso8601StringToTm(input));
    }
}
