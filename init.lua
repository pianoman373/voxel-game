EnumDirection = {
	POSITIVE_X = 0,
    NEGATIVE_X = 1,
    POSITIVE_Y = 2,
    NEGATIVE_Y = 3,
    POSITIVE_Z = 4,
    NEGATIVE_Z = 5
}

function simpleBlock(blockName, texX, texY)
	return {
		name = blockName,
		getTextureCoord = function(side)
			return texX, texY
		end,
		isSolid = function()
			return true
		end
	}
end

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

registerBlock(2, simpleBlock("Stone", 1, 0))

registerBlock(3, simpleBlock("Dirt", 2, 0))

registerBlock(4, simpleBlock("Cobblestone", 0, 1))

registerBlock(5, simpleBlock("Planks", 4, 0))

registerBlock(6, simpleBlock("Wood", 4, 1))

registerBlock(7, {
	name = "Leaves",
	getTextureCoord = function(side)
		return 4, 3
	end,
	isSolid = function()
		return false
	end
})

registerBlock(8, simpleBlock("Bricks", 7, 0))

registerBlock(9, simpleBlock("Stone Bricks", 6, 3))

registerBlock(10, {
	name = "Glass",
	getTextureCoord = function(side)
		return 1, 3
	end,
	isSolid = function()
		return false
	end
})
