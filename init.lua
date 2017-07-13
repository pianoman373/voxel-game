EnumDirection = {
	POSITIVE_X = 0,
    NEGATIVE_X = 1,
    POSITIVE_Y = 2,
    NEGATIVE_Y = 3,
    POSITIVE_Z = 4,
    NEGATIVE_Z = 5
}

function simpleBlock(blockName, texX, texY, color)
	return {
		name = blockName,
		color = color,
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
	color = {0.0, 0.0, 0.0},
	getTextureCoord = function(side)
		return 0, 0
	end,
	isSolid = function()
		return false
	end
})

registerBlock(1, {
	name = "Grass",
	color = {78/255, 111/255, 40/255},
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

registerBlock(2, simpleBlock("Stone", 1, 0, {138/255, 128/255, 115/255}))

registerBlock(3, simpleBlock("Dirt", 2, 0, {73/255, 51/255, 41/255}))

registerBlock(4, simpleBlock("Cobblestone", 0, 1, {148/255, 138/255, 129/255}))

registerBlock(5, simpleBlock("Planks", 4, 0, {106/255, 85/255, 61/255}))

registerBlock(6, simpleBlock("Wood", 4, 1, {87/255, 63/255, 52/255}))

registerBlock(7, {
	name = "Leaves",
	color = {91/255, 129/255, 48/255},
	getTextureCoord = function(side)
		return 4, 3
	end,
	isSolid = function()
		return false
	end
})

registerBlock(8, simpleBlock("Bricks", 7, 0, {161/255, 149/255, 134/255}))

registerBlock(9, simpleBlock("Stone Bricks", 6, 3, {129/255, 93/255, 70/255}))

registerBlock(10, {
	name = "Glass",
	color = {216/255, 227/255, 231/255},
	getTextureCoord = function(side)
		return 1, 3
	end,
	isSolid = function()
		return false
	end
})
