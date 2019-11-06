#include <stdafx.h>

#include <playfab/PlayFabAuthenticationContext.h>
#include <playfab/PlayFabSettings.h>

namespace PlayFab
{
    PlayFabAuthenticationContext::PlayFabAuthenticationContext()
    {
        ForgetAllCredentials();
    }

    void PlayFabAuthenticationContext::ForgetAllCredentials()
    {
#ifndef DISABLE_PLAYFABCLIENT_API
        playFabId.clear();
        clientSessionTicket.clear();
        advertisingIdType.clear();
        advertisingIdValue.clear();
#endif
        entityId.clear();
        entityType.clear();
        entityToken.clear();
    }

    void SetIfNotNull(const std::string& input, std::string& output)
    {
        if (!input.empty())
        {
            output = input;
        }
    }

    void PlayFabAuthenticationContext::HandlePlayFabLogin(
        const std::string& setPlayFabId,
        const std::string& setClientSessionTicket,
        const std::string& setEntityId,
        const std::string& setEntityType,
        const std::string& setEntityToken
    )
    {
        SetIfNotNull(setPlayFabId, playFabId);
        SetIfNotNull(setClientSessionTicket, clientSessionTicket);
        SetIfNotNull(setEntityId, entityId);
        SetIfNotNull(setEntityType, entityType);
        SetIfNotNull(setEntityToken, entityToken);
    }
}
