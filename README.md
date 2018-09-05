# Corona Ario Plugin

[Ario](https://docs.ariogames.ir/) is a game network framework

## Structure
* `ArioPlugin.cpp` contains source code
* `ArioSample` contains Lua sample

## Building a plugin

To compile install [Corona](https://coronalabs.com/) open `Plugin.sln` with
Visual Studio (2013), select "Release" and hit "Build Solution".

## Quick Documentation

```Lua

local ario = require "plugin.ario"
ario.init("com.package.id")
ario.getId() -- returns User id
ario.isLogin() -- returns if User is logged in
ario.getLevel() -- returns User's level

local function listener(event)
	event.isError - indicates if call was successful
	event.type - which call invoked the event
	event.response - JSON response from the API
end

ario.setupAchievement("BASE-64-KEY-AAA-BBB") -- setups achievements, returns bool
ario.unlockAchievement({achievementId=123, listener=listener}) -- unlocks specific achievement
ario.getGameAchievements({listener=listener}) -- loads achievements
ario.resetAchievement({achievementId=123, listener=listener}) -- resets specific achievement
ario.showAchievements()

ario.setupLeaderboards("BASE-64-KEY-AAA-BBB") -- setups leaderboards, returns bool
ario.getLeaderboards({listener=listener}) -- loads leaderboards
ario.submitScore({value=9000, leaderboardId=123, listener=listener}) -- submits score to specific leaderboard
ario.getScores({leaderboardId=123, collectionType=123, timeFrame=1, offset=0, limit=20, listener=listener}) -- loads scores for specific leaderboard

```
