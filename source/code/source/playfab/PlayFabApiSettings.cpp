#include <stdafx.h>

#include <playfab/PlayFabApiSettings.h>
#include <playfab/PlayFabSettings.h>

namespace PlayFab
{
    PlayFabApiSettings::PlayFabApiSettings() :
        baseServiceHost(PlayFabSettings::productionEnvironmentURL)
    {
        requestGetParams["sdk"] = PlayFabSettings::versionString;

        // Don't let PlayFabSettings::staticSettings pull titleId from itself
        if (PlayFabSettings::staticSettings != nullptr)
        {
            titleId = PlayFabSettings::staticSettings->titleId;
        }
    }

    std::string PlayFabApiSettings::GetUrl(const std::string& urlPath) const
    {
        std::string fullUrl;
        if(connectionString == "")
        {
            fullUrl.reserve(1000);

            fullUrl += "https://";
            fullUrl += titleId;
            fullUrl += baseServiceHost;
            fullUrl += urlPath;
        }
        else
        {
            fullUrl.reserve(connectionString.size() + 1000);
            fullUrl = connectionString;
        }

        bool firstParam = true;
        for (auto const& paramPair : requestGetParams)
        {
            if (firstParam)
            {
                fullUrl += "?";
                firstParam = false;
            }
            else
            {
                fullUrl += "&";
            }
            fullUrl += paramPair.first;
            fullUrl += "=";
            fullUrl += paramPair.second;
        }

        return fullUrl;
    }
}
