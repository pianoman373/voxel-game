EnumDirection = {
	POSITIVE_X = 0,
    NEGATIVE_X = 1,
    POSITIVE_Y = 2,
    NEGATIVE_Y = 3,
    POSITIVE_Z = 4,
    NEGATIVE_Z = 5
}

api = {}

function api.simpleBlock(blockName, texX, texY, color)
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

function rgb(r, g, b)
	return {r / 255, g / 255, b / 255}
end
