// ----------------------------------------------------------------------------
// Copyright (c) 2018 Corona Labs Inc. All rights reserved.
// ----------------------------------------------------------------------------

#include <CoronaAssert.h>
#include <CoronaLua.h>
#include <CoronaEvent.h>


#include "ario/api.h"
#include "ario/CommonErrorCodes.h"
#include "ario/StoreSdkCommon.h"
#include "ario/global_define.h"
#include "ario/sdk_result_code.h"
#include "ario/flat/flat_ario_achievement.h"
#include "ario/flat/flat_ario_leaderboard.h"
#include "ario/flat/flat_ario_user.h"

#include <mutex>
#include <list>
#include <string>
#include <map>

#include <thread>
#include <chrono>
#include <cstring>

#pragma comment(lib, "ArioSdk.lib")
#pragma comment(lib, "StoreSdk.lib")


CORONA_EXPORT int luaopen_plugin_ario( lua_State * );


class ArioLibrary
{
public:
	static const char kName[];
	static const char kEvent[];

protected:
	ArioLibrary();
	virtual ~ArioLibrary();

public:
	static int Open( lua_State *L );

protected:
	static int Finalizer( lua_State *L );

	static int ProcessFrame( lua_State *L );

	static int arioInit( lua_State *L );

	static int getId( lua_State *L );
	static int isLogin( lua_State *L );
	static int getLevel( lua_State *L );

	static int setupAchievement( lua_State *L );
	static int unlockAchievement( lua_State *L );
	static int getGameAchievements( lua_State *L );
	static int showAchievements( lua_State *L );
	static int resetAchievement( lua_State *L );

	static int setupLeaderboards( lua_State *L );
	static int getLeaderboards( lua_State *L );
	static int submitScore( lua_State *L );
	static int getScores( lua_State *L );

	static ArioLibrary *ToLibrary( lua_State *L );

private:
	int frameCallbackIndex;

	static int nextRequestId;
	static std::mutex eventListMutex;
	struct EventResult {
		int isError;
		std::string type;
		std::string response;
		int requestCode;
	};
	static std::list<EventResult> eventResults;

	std::map<int, CoronaLuaRef> eventListeners;
};

// ----------------------------------------------------------------------------
int ArioLibrary::nextRequestId = 1;
std::mutex ArioLibrary::eventListMutex;
std::list<ArioLibrary::EventResult> ArioLibrary::eventResults;

// ----------------------------------------------------------------------------

const char ArioLibrary::kName[] = "plugin.ario";
const char ArioLibrary::kEvent[] = "ario";

ArioLibrary::ArioLibrary()
{
}
ArioLibrary::~ArioLibrary()
{
}

int
ArioLibrary::Open( lua_State *L )
{
	const char kMetatableName[] = __FILE__;
	CoronaLuaInitializeGCMetatable( L, kMetatableName, Finalizer );

	ArioLibrary *library = new ArioLibrary();

	{
		// Runtime:addEventListener( "enterFrame", /ProcessFrame/ )
		CoronaLuaPushRuntime( L ); // push 'Runtime'
		lua_getfield( L, -1, "addEventListener" ); // push 'addEventListener'
		lua_insert( L, -2 ); // swap so 'addEventListener' is below 'Runtime'
		lua_pushstring( L, "enterFrame" );

		// Push ProcessFrame as closure so it has access
		lua_pushlightuserdata( L, library );
		lua_pushcclosure( L, &ProcessFrame, 1 );

		lua_pushvalue( L, -1 );
		library->frameCallbackIndex = luaL_ref( L, LUA_REGISTRYINDEX ); // r = closure

		CoronaLuaDoCall( L, 3, 0 );
	}


	// Functions in library
	const luaL_Reg kVTable[] =
	{
		{ "init", arioInit },
		{ "getId", getId },
		{ "isLogin", isLogin },
		{ "getLevel", getLevel },
		{ "setupAchievement", setupAchievement },
		{ "unlockAchievement", unlockAchievement },
		{ "getGameAchievements", getGameAchievements },
		{ "showAchievements", showAchievements },
		{ "resetAchievement", resetAchievement },
		{ "setupLeaderboards", setupLeaderboards },
		{ "getLeaderboards", getLeaderboards },
		{ "submitScore", submitScore },
		{ "getScores", getScores },

		{ NULL, NULL }
	};
	CoronaLuaPushUserdata( L, library, kMetatableName );
	luaL_openlib( L, kName, kVTable, 1 );
	return 1;
}

int
ArioLibrary::Finalizer( lua_State *L )
{
	ArioLibrary *library = (ArioLibrary *)CoronaLuaToUserdata( L, 1 );

	//Get rid of closure
	CoronaLuaPushRuntime( L ); // push 'Runtime'

	if ( lua_type( L, -1 ) == LUA_TTABLE && library && library->frameCallbackIndex)
	{
		lua_getfield( L, -1, "removeEventListener" ); // push 'f', i.e. Runtime.addEventListener
		lua_insert( L, -2 ); // swap so 'f' is below 'Runtime'
		lua_pushstring( L, "enterFrame" );
		lua_rawgeti( L, LUA_REGISTRYINDEX, library->frameCallbackIndex );// pushes closure
		CoronaLuaDoCall( L, 3, 0 );
		luaL_unref(L, LUA_REGISTRYINDEX,  library->frameCallbackIndex);
		library->frameCallbackIndex = 0;
	}
	else
	{
		lua_pop( L, 1 ); // pop nil
	}
	delete library;

	std::lock_guard<std::mutex> guard(eventListMutex);
	eventResults.clear();

	return 0;
}

int
ArioLibrary::ProcessFrame( lua_State *L )
{
	ArioLibrary *library = (ArioLibrary *)lua_touserdata(L, lua_upvalueindex(1));
	std::lock_guard<std::mutex> guard(eventListMutex);
	for (const EventResult & e : eventResults) {
		auto it = library->eventListeners.find(e.requestCode);
		if(it != library->eventListeners.end()) {
			CoronaLuaRef listener = it->second;
			library->eventListeners.erase(it);

			CoronaLuaNewEvent(L, kEvent);

			lua_pushstring(L, e.type.c_str());
			lua_setfield(L, -2, CoronaEventTypeKey());

			lua_pushstring(L, e.response.c_str());
			lua_setfield(L, -2, CoronaEventResponseKey());

			lua_pushboolean(L, e.isError);
			lua_setfield(L, -2, CoronaEventIsErrorKey());

			CoronaLuaDispatchEvent(L, listener, 0);

			CoronaLuaDeleteRef(L, listener);
		} else {
			printf("ERROR: Unknown event cannot be dispatched!");
		}
	}
	eventResults.clear();
	return 0;
}

ArioLibrary *
ArioLibrary::ToLibrary( lua_State *L )
{
	ArioLibrary *library = (ArioLibrary *)CoronaLuaToUserdata( L, lua_upvalueindex( 1 ) );
	return library;
}

// Init
int ArioLibrary::arioInit(lua_State *L) {
	if(lua_type(L, 1) != LUA_TSTRING) {
		printf("ERROR: init expects package name as string, got %s\n", lua_typename(L, 1));
		return 0;
	}
	const char *packageNameConst = lua_tostring(L, 1);
	size_t len = strlen(packageNameConst) + 1;
	char *packageName = new char[len];
	strcpy_s(packageName, len, packageNameConst);

	int init = Ario::Api_Init(packageName);
	delete []packageName;
	lua_pushinteger(L, init);
	return 1;
}

// Users
int ArioLibrary::getId(lua_State *L) {
	unsigned int res = ArioUser_GetId();
	lua_pushinteger(L, res);
	return 1;
}

int ArioLibrary::isLogin(lua_State *L) {
	bool res = ArioUser_IsLogin();
	lua_pushboolean(L, res);
	return 1;
}

int ArioLibrary::getLevel(lua_State *L) {
	int res = ArioUser_GetLevel();
	lua_pushinteger(L, res);
	return 1;
}

// Achievements
int ArioLibrary::setupAchievement(lua_State *L) {
	if(lua_type(L, 1) != LUA_TSTRING) {
		printf("ERROR: setupAchievement expects string key name as first parameter, got %s\n", lua_typename(L, 1));
		return 0;
	}
	const char *keyConst = lua_tostring(L, 1);
	size_t len = strlen(keyConst) + 1;
	char *key = new char[len];
	strcpy_s(key, len, keyConst);

	int result = ArioAchievement_StartSetup(key);
	delete []key;

	lua_pushboolean(L, result == RESULT_OK);
	return 1;
}

int ArioLibrary::unlockAchievement(lua_State *L) {
	ArioLibrary *library = ToLibrary( L );

	if (lua_type(L, 1) != LUA_TTABLE) {
		printf("ERROR: unlockAchievement first parameter must be a table with arguments, but got %s", lua_typename(L, 1));
		return 0;
	}

	int reqCode = nextRequestId++;

	int achievementId = 0;
	CoronaLuaRef listener = NULL;

	lua_getfield(L, 1, "achievementId");
	if(lua_type(L, -1) == LUA_TNUMBER) {
		achievementId = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	if (achievementId == 0) {
		printf("Error: no achievementId field passed");
		return 0;
	}

	lua_getfield(L, 1, "listener");
	if(CoronaLuaIsListener(L, -1, kEvent)) {
		listener = CoronaLuaNewRef(L, -1);
	}
	lua_pop(L, 1);

	if (listener == NULL) {
		printf("Error: no valid listener passed");
		return 0;
	}

	library->eventListeners[reqCode] = listener;

	ArioAchievement_Unlock(achievementId, reqCode, [](int lReqCode, int result, int response_size) {
		EventResult e;
		e.requestCode = lReqCode;
		e.type = "unlockAchievement";
		e.isError = (result != 0);

		if (result == 0)
		{
			int bufferSize = response_size;
			char* response = new char[bufferSize];
			Ario::GetJsonResult(lReqCode, response, bufferSize);
			e.response = response;
			delete[] response;
		}

		std::lock_guard<std::mutex> guard(eventListMutex);
		eventResults.push_back(e);
	});
	return 0;
}

int ArioLibrary::getGameAchievements(lua_State *L) {
	ArioLibrary *library = ToLibrary(L);

	if (lua_type(L, 1) != LUA_TTABLE) {
		printf("ERROR: getGameAchievements parameter must be a table with arguments, but got %s", lua_typename(L, 1));
		return 0;
	}

	int reqCode = nextRequestId++;
	CoronaLuaRef listener = NULL;

	lua_getfield(L, 1, "listener");
	if (CoronaLuaIsListener(L, -1, kEvent)) {
		listener = CoronaLuaNewRef(L, -1);
	}
	lua_pop(L, 1);

	if (listener == NULL) {
		printf("Error: no valid listener passed");
		return 0;
	}

	library->eventListeners[reqCode] = listener;

	ArioAchievement_GetGameAchievements(reqCode, [](int lReqCode, int result, int response_size) {
		EventResult e;
		e.requestCode = lReqCode;
		e.type = "gameAchievements";
		e.isError = (result != 0);

		if (result == 0)
		{
			int bufferSize = response_size;
			char* response = new char[bufferSize];
			Ario::GetJsonResult(lReqCode, response, bufferSize);
			e.response = response;
			delete[] response;
		}

		std::lock_guard<std::mutex> guard(eventListMutex);
		eventResults.push_back(e);
	});
	return 0;
}

int ArioLibrary::showAchievements(lua_State *L) {
	ArioAchievement_ShowAchievements();
	return 0;
}

int ArioLibrary::resetAchievement(lua_State *L) {
	ArioLibrary *library = ToLibrary(L);

	if (lua_type(L, 1) != LUA_TTABLE) {
		printf("ERROR: resetAchievement parameter must be a table with arguments, but got %s", lua_typename(L, 1));
		return 0;
	}

	int reqCode = nextRequestId++;
	CoronaLuaRef listener = NULL;
	int achievementId = 0;

	lua_getfield(L, 1, "achievementId");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		achievementId = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);


	lua_getfield(L, 1, "listener");
	if (CoronaLuaIsListener(L, -1, kEvent)) {
		listener = CoronaLuaNewRef(L, -1);
	}
	lua_pop(L, 1);

	if (listener == NULL) {
		printf("Error: no valid listener passed");
		return 0;
	}

	library->eventListeners[reqCode] = listener;

	ArioAchievement_ResetStatus(achievementId, reqCode, [](int lReqCode, int result, int response_size) {
		EventResult e;
		e.requestCode = lReqCode;
		e.type = "resetAchievement";
		e.isError = (result != 0);

		if (result == 0)
		{
			int bufferSize = response_size;
			char* response = new char[bufferSize];
			Ario::GetJsonResult(lReqCode, response, bufferSize);
			e.response = response;
			delete[] response;
		}

		std::lock_guard<std::mutex> guard(eventListMutex);
		eventResults.push_back(e);
	});
	return 0;
}

// Leaderboards
int ArioLibrary::setupLeaderboards(lua_State *L) {
	if(lua_type(L, 1) != LUA_TSTRING) {
		printf("ERROR: setupLeaderboards expects string key name as first parameter, got %s\n", lua_typename(L, 1));
		return 0;
	}
	const char *keyConst = lua_tostring(L, 1);
	size_t len = strlen(keyConst) + 1;
	char *key = new char[len];
	strcpy_s(key, len, keyConst);

	int result = ArioLeaderboard_StartSetup(key);
	delete []key;

	lua_pushboolean(L, result == RESULT_OK);
	return 1;
}

int ArioLibrary::getLeaderboards(lua_State *L) {
	ArioLibrary *library = ToLibrary(L);

	if (lua_type(L, 1) != LUA_TTABLE) {
		printf("ERROR: getLeaderboards parameter must be a table with arguments, but got %s", lua_typename(L, 1));
		return 0;
	}

	int reqCode = nextRequestId++;
	CoronaLuaRef listener = NULL;

	lua_getfield(L, 1, "listener");
	if (CoronaLuaIsListener(L, -1, kEvent)) {
		listener = CoronaLuaNewRef(L, -1);
	}
	lua_pop(L, 1);

	if (listener == 0) {
		printf("Error: no valid listener passed");
		return 0;
	}

	library->eventListeners[reqCode] = listener;

	ArioLeaderboard_GetLeaderboards(reqCode, [](int lReqCode, int result, int response_size) {
		EventResult e;
		e.requestCode = lReqCode;
		e.type = "getLeaderboards";
		e.isError = (result != 0);

		if (result == 0)
		{
			int bufferSize = response_size;
			char* response = new char[bufferSize];
			Ario::GetJsonResult(lReqCode, response, bufferSize);
			e.response = response;
			delete[] response;
		}

		std::lock_guard<std::mutex> guard(eventListMutex);
		eventResults.push_back(e);
	});
	return 0;
}

int ArioLibrary::submitScore(lua_State *L) {
	ArioLibrary *library = ToLibrary(L);

	if (lua_type(L, 1) != LUA_TTABLE) {
		printf("ERROR: submitScore parameter must be a table with arguments, but got %s", lua_typename(L, 1));
		return 0;
	}

	int reqCode = nextRequestId++;
	char metadata[] = "";

	int value = 0;
	int leaderboardId = 0;
	CoronaLuaRef listener = NULL;

	lua_getfield(L, 1, "value");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		value = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 1, "leaderboardId");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		leaderboardId = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	if (leaderboardId == 0) {
		printf("Error: no achievementId field passed");
		return 0;
	}

	lua_getfield(L, 1, "listener");
	if (CoronaLuaIsListener(L, -1, kEvent)) {
		listener = CoronaLuaNewRef(L, -1);
	}
	lua_pop(L, 1);

	if (listener == 0) {
		printf("Error: no valid listener passed");
		return 0;
	}

	library->eventListeners[reqCode] = listener;

	ArioLeaderboard_SubmitScore(value, leaderboardId, metadata, reqCode, [](int lReqCode, int result, int response_size) {
		EventResult e;
		e.requestCode = lReqCode;
		e.type = "submitScore";
		e.isError = (result != 0);

		if (result == 0)
		{
			int bufferSize = response_size;
			char* response = new char[bufferSize];
			Ario::GetJsonResult(lReqCode, response, bufferSize);
			e.response = response;
			delete[] response;
		}

		std::lock_guard<std::mutex> guard(eventListMutex);
		eventResults.push_back(e);
	});
	return 0;
}

int ArioLibrary::getScores(lua_State *L) {
	ArioLibrary *library = ToLibrary(L);

	if (lua_type(L, 1) != LUA_TTABLE) {
		printf("ERROR: submitScore parameter must be a table with arguments, but got %s", lua_typename(L, 1));
		return 0;
	}

	int leaderboardId = 0;
	int collectionType = 0; // { ALL_USER = 0, FRIENDS = 1 };
	int timeFrame = 0; // { DAILY = 0, WEEKLY = 1, ALL_TIME = 2 };
	int offset = 0;
	int limit = 0;
	int reqCode = nextRequestId++;

	CoronaLuaRef listener = NULL;

	lua_getfield(L, 1, "leaderboardId");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		leaderboardId = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	if (leaderboardId == 0) {
		printf("Error: no leaderboardId field passed");
		return 0;
	}

	lua_getfield(L, 1, "collectionType");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		collectionType = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	if (collectionType > 1) {
		printf("Error: no collectionType field passed");
		return 0;
	}

	lua_getfield(L, 1, "timeFrame");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		timeFrame = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	if (timeFrame > 2) {
		printf("Error: no timeFrame field passed");
		return 0;
	}

	lua_getfield(L, 1, "offset");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		offset = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 1, "limit");
	if (lua_type(L, -1) == LUA_TNUMBER) {
		limit = (int)lua_tointeger(L, -1);
	}
	lua_pop(L, 1);

	lua_getfield(L, 1, "listener");
	if (CoronaLuaIsListener(L, -1, kEvent)) {
		listener = CoronaLuaNewRef(L, -1);
	}
	lua_pop(L, 1);

	if (listener == 0) {
		printf("Error: no valid listener passed");
		return 0;
	}

	library->eventListeners[reqCode] = listener;

	ArioLeaderboard_GetScores(leaderboardId, collectionType, timeFrame, offset, limit, reqCode,
							  [](int lReqCode, int result, int response_size) {
								  EventResult e;
								  e.requestCode = lReqCode;
								  e.type = "getScores";
								  e.isError = (result != 0);

								  if (result == 0)
								  {
									  int bufferSize = response_size;
									  char* response = new char[bufferSize];
									  Ario::GetJsonResult(lReqCode, response, bufferSize);
									  e.response = response;
									  delete[] response;
								  }

								  std::lock_guard<std::mutex> guard(eventListMutex);
								  eventResults.push_back(e);
							  });
	return 0;
}

// ----------------------------------------------------------------------------

CORONA_EXPORT int luaopen_plugin_ario( lua_State *L )
{
	return ArioLibrary::Open( L );
}

