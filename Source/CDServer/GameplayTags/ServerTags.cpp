#include "ServerTags.h"

namespace ServerTags
{
	namespace GameSessionAPI
	{
		UE_DEFINE_GAMEPLAY_TAG(ListFleets, "ServerTags.GameSessionAPI.ListFleets");
		UE_DEFINE_GAMEPLAY_TAG(FindOrCreateGameSession, "ServerTags.GameSessionAPI.FindOrCreateGameSession");
		UE_DEFINE_GAMEPLAY_TAG(CreatePlayerSession, "ServerTags.GameSessionAPI.CreatePlayerSession");
		UE_DEFINE_GAMEPLAY_TAG(FindGameSessions, "ServerTags.GameSessionAPI.FindGameSessions");
		UE_DEFINE_GAMEPLAY_TAG(UpdateGameSession, "ServerTags.GameSessionAPI.UpdateGameSession");
	}

	namespace PortalAPI
	{
		UE_DEFINE_GAMEPLAY_TAG(SignIn, "ServerTags.PortalAPI.SignIn");
		UE_DEFINE_GAMEPLAY_TAG(SignUp, "ServerTags.PortalAPI.SignUp");
		UE_DEFINE_GAMEPLAY_TAG(ConfirmSignUp, "ServerTags.PortalAPI.ConfirmSignUp");
		UE_DEFINE_GAMEPLAY_TAG(SignOut, "ServerTags.PortalAPI.SignOut");
	}
}