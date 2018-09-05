local debug = {}
debug.limit = 5
debug.text  = display.newText{
    text = "",     
    x = 10 + display.screenOriginX,
    y = 10 + display.screenOriginY,
    font = native.systemFont,   
    fontSize = 36, 
    align = "left"
}

function debug:log(msg)
	table.insert(self, 1, msg) -- в начало
	if #self > self.limit then
		self[#self] = nil
	end
	
	self.text.text = table.concat(self, '\n')
end

setmetatable(debug, {__call = debug.log})

function debug:setLimit(lim)
	self.limit = lim
	for i = limit, #self do
		self[i] = nil
	end
end

return debug