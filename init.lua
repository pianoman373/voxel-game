--local inspect = require("inspect")

registerBlock(0, {
	name = "Air",
	color = {0, 0, 0},
	getTextureCoord = function(side)
		return 0, 0
	end,
	isSolid = function()
		return false
	end
})

registerBlock(1, {
	name = "Grass",
	color = rgb(78, 111, 40),
	getTextureCoord = function(side)
		if side == EnumDirection.POSITIVE_Y then
			return 0, 0
		end
		if side == EnumDirection.NEGATIVE_Y then
			return 2, 0
		end
		return 3, 0
	end,
	isSolid = function()
		return true
	end
})

registerBlock(2, api.simpleBlock("Stone", 1, 0, rgb(138, 128, 115)))

registerBlock(3, api.simpleBlock("Dirt", 2, 0, rgb(73, 51, 41)))

registerBlock(4, api.simpleBlock("Cobblestone", 0, 1, rgb(148, 138, 129)))

registerBlock(5, api.simpleBlock("Planks", 4, 0, rgb(106, 85, 61)))

registerBlock(6, api.simpleBlock("Wood", 4, 1, rgb(87, 63, 52)))

registerBlock(7, {
	name = "Leaves",
	color = rgb(91, 129, 48),
	getTextureCoord = function(side)
		return 4, 3
	end,
	isSolid = function()
		return false
	end
})

registerBlock(8, api.simpleBlock("Bricks", 7, 0, rgb(161, 149, 134)))

registerBlock(9, api.simpleBlock("Stone Bricks", 6, 3, rgb(129, 93, 70)))

registerBlock(10, {
	name = "Glass",
	color = rgb(216, 227, 231),
	getTextureCoord = function(side)
		return 1, 3
	end,
	isSolid = function()
		return false
	end
})
