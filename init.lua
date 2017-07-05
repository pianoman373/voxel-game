EnumDirection = {
	POSITIVE_X = 0,
    NEGATIVE_X = 1,
    POSITIVE_Y = 2,
    NEGATIVE_Y = 3,
    POSITIVE_Z = 4,
    NEGATIVE_Z = 5
}

registerBlock(0, {
	name = "Air",
	getTextureCoord = function(side)
		return 0, 0
	end,
	isSolid = function()
		return false
	end
})

registerBlock(1, {
	name = "Grass",
	getTextureCoord = function(side)
		if side == EnumDirection.POSITIVE_Y then
			return 3, 0
		end
		if side == EnumDirection.NEGATIVE_Y then
			return 4, 0
		end
		return 5, 0
	end,
	isSolid = function()
		return true
	end
})

registerBlock(2, {
	name = "Stone",
	getTextureCoord = function(side)
		return 0, 0
	end,
	isSolid = function()
		return true
	end
})

registerBlock(3, {
	name = "Dirt",
	getTextureCoord = function(side)
		return 4, 0
	end,
	isSolid = function()
		return true
	end
})

registerBlock(4, {
	name = "Cobblestone",
	getTextureCoord = function(side)
		return 2, 0
	end,
	isSolid = function()
		return true
	end
})

registerBlock(5, {
	name = "Planks",
	getTextureCoord = function(side)
		return 6, 0
	end,
	isSolid = function()
		return true
	end
})
