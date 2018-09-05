-- package.cpath = system.pathForFile()..[[\?.dll;]] .. package.cpath
display.setDefault( 'anchorX', 0 )
display.setDefault( 'anchorY', 0 )

mainGroup = display.newGroup();
mainGroup.parent:insert(mainGroup);

local pacageName = "com.greenolor.necromancerreturns"
local key = "YOUR KEY"
local leaderboardId = 39;

log = require'debug_log'

-- log('currentdir: '     .. lfs.currentdir())

local json = require "json"
local ario = require "plugin.ario"
local init = ario.init(pacageName);
log('ario init: ' .. tostring(init))

-- enum CollectionType { ALL_USER = 0, FRIENDS = 1 };
-- enum TimeFrame { DAILY = 0, WEEKLY = 1, ALL_TIME = 2 };

local achId = 73;
local collectionType = 0;
local timeFrame = 2;
local buttons = {};

local tfId = display.newText("ID: " .. achId, 0, 0, fontMain, 40);
tfId:setFillColor( 1,1,1 );
tfId.x = 400;
tfId.y = 200;
mainGroup:insert(tfId);

function addButton(title, tx, ty, fname, onRelease, tfSize, tfColor, ico)
	local item_mc = display.newGroup();
	item_mc.act = title;
	item_mc.onRelease = onRelease;
	item_mc.x, item_mc.y = tx,ty;
	local body_mc = display.newImage(item_mc, fname);
	body_mc.x, body_mc.y = -body_mc.width/2, -body_mc.height/2;
	local over_mc = display.newImage(item_mc, fname);
	over_mc.blendMode = "add";
	over_mc.alpha = 0.0;
	over_mc.x, over_mc.y = -over_mc.width/2, -over_mc.height/2;
	item_mc.r = body_mc.width*body_mc.xScale/2;
	item_mc.rr = item_mc.r*item_mc.r;
	item_mc.w = item_mc.width*item_mc.xScale;
	item_mc.h = item_mc.height*item_mc.yScale;
	item_mc._selected = false;
	item_mc._body = body_mc;
	item_mc._over = over_mc;
	item_mc.enabled = true;

	if(tfSize)then
		item_mc.tf = display.newText( title, 0, 0, nil, tfSize );
		if(tfColor)then
			item_mc.tf:setFillColor( tfColor[1]/255, tfColor[2]/255, tfColor[3]/255 );
		else
			item_mc.tf:setFillColor( 1,1,1 );
		end
		item_mc.tf.x = -item_mc.tf.width/2;
		item_mc.tf.y = -item_mc.tf.height/2;
		item_mc:insert(item_mc.tf);
	end
	
	table.insert(buttons, item_mc);
	
	return item_mc
end

local tf = addButton("-", 0, 0, "btnMiniOk.png", nil, 40);
tf.act = "downId";
tf.x = tfId.x + 250;
tf.y = tfId.y;
mainGroup:insert(tf)
local tf = addButton("+", 0, 0, "btnMiniOk.png", nil, 40);
tf.act = "upId";
tf.x = tfId.x + 350;
tf.y = tfId.y;
mainGroup:insert(tf)
local tfUnlock = addButton("Unlock", 0, 0, "btnMenu.png", nil, 40);
tfUnlock.act = "unlock";
tfUnlock.x = tfId.x + 250;
tfUnlock.y = tfId.y + 100;
mainGroup:insert(tfUnlock)
local tfShow = addButton("Show", 0, 0, "btnMenu.png", nil, 40);
tfShow.act = "show";
tfShow.x = tfId.x + 250;
tfShow.y = tfUnlock.y + 100;
mainGroup:insert(tfShow)
local btnLead = addButton("Set Leaderboard", 0, 0, "btnMenu.png", nil, 30);
btnLead.act = "leaderboard";
btnLead.x = tfId.x + 250;
btnLead.y = tfShow.y + 100;
mainGroup:insert(btnLead)
local btnScore = addButton("getScores", 0, 0, "btnMenu.png", nil, 30);
btnScore.act = "getScores";
btnScore.x = tfId.x + 250;
btnScore.y = btnLead.y + 100;
mainGroup:insert(btnScore)

if(init == 0)then
	log('getId: '     .. tostring(ario.getId()))
	log('getLevel: '     .. tostring(ario.getLevel()))
	log('setupAchievement: '     .. tostring(ario.setupAchievement(key)))
	log('setupLeaderboards: '     .. tostring(ario.setupLeaderboards(key)))
	
	-- log('getGameAchievements: '     .. tostring(ario.getAchievements()))
	-- log('cpath:'     .. package.cpath:gsub(';', ';\n'))

	-- for key, value in pairs(ario) do
		-- print(key, value)
	-- end
end

local function checkButtons(event)
	for i=1,#buttons do
		local item_mc = buttons[i];
		local _x, _y = item_mc:localToContent(0, 0); -- localToGlobal
		local dx = event.x - _x;
		local dy = event.y - _y;
		local w = item_mc.w;
		local h = item_mc.h;

		if(math.abs(dx)<w/2 and math.abs(dy)<h/2 and 
		item_mc.isVisible and item_mc.alpha == 1)then
			if(item_mc._selected == false)then
				item_mc._selected = true;
				if(item_mc._over)then
					item_mc._over.alpha = 0.3;
				end
			end
		else
			if(item_mc._selected)then
				item_mc._selected = false;
				if(item_mc._over)then
					item_mc._over.alpha = 0;
				end
			end
		end
	end
end

local function touchHandler(event)
	local phase = event.phase;
	if(phase=='began')then
		checkButtons(event);
	elseif(phase=='moved')then
		checkButtons(event);
	else
		for i=1,#buttons do
			local item_mc = buttons[i];
			if(item_mc._selected)then
				item_mc._selected = false;
				if(item_mc._over)then
					item_mc._over.alpha = 0;
				end
				if(item_mc.act == "downId")then
					achId = achId - 1;
					achId = math.max(1, achId);
					tfId.text = "ID: " .. achId;
					return true;
				elseif(item_mc.act == "upId")then
					achId = achId + 1;
					tfId.text = "ID: " .. achId;
					return true;
				elseif(item_mc.act == "unlock")then
					ario.unlockAchievement({
						achievementId=achId,
						listener = function( e )
							print("unlockAchievement:", json.prettify(e))
						end
					})
					return true;
				elseif(item_mc.act == "show")then
					ario.showAchievements();
					return true;
				elseif(item_mc.act == "leaderboard")then
					ario.submitScore({
						leaderboardId=leaderboardId,
						value=100,
						listener = function( e )
							print("submitScore:", json.prettify(e))
						end
					})
					return true;
				elseif(item_mc.act == "getScores")then
					ario.getScores({
						leaderboardId=leaderboardId,
						collectionType=collectionType,
						timeFrame=timeFrame,
						offset=0,
						limit=19,
						listener = function( e )
							print("getScores:", json.prettify(e))
						end
					})
					return true;
				end
			end
		end
	end
end


Runtime:addEventListener( "touch", touchHandler );
