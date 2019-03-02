print("Running Lua code...")

require("api")

local ffi = require("ffi")
ffi.cdef[[
float ridgedNoise(float x, float y, int octaves, float frequency, float persistence);

void setChunk(int x, int z, uint8_t *data);

int printf(const char *fmt, ...);

float noise3D(float x, float y, float z);
]]
local C = ffi.C

--helper function
function getHeight(x, z)
    return C.ridgedNoise(x/10, z/10, 5, 0.01, 0.5) * 80 + 80
end

function getCave(x, y, z)
	return C.noise3D(x/100, y/100, z/100) > 0.6;
end

local topBlock = 1 --grass
local midBlock = 3 --dirt
local fillerBlock = 2 --stone

local heights = array2D(18) --heightmap includes adjacent chunk blocks

local generateChunk = function(chunk_x, chunk_z, chunk)
    for x = 0, 17 do
       for z = 0, 17 do
           heights[x][z] = getHeight(x-1 + (chunk_x * 16), z-1 + (chunk_z * 16))
       end
    end

    for x = 0, 15 do
        for z = 0, 15 do
            local height = heights[x+1][z+1]

            local lineNegX =  normalize(vec3(x - 1, heights[x+2][z+1], z) - vec3(x, height, z));
            local linePosX =  normalize(vec3(x + 1, heights[x][z+1], z) - vec3(x, height, z));

            local lineNegY =  normalize(vec3(x, heights[x+1][z], z - 1) - vec3(x, height, z));
            local linePosY =  normalize(vec3(x, heights[x+1][z+2], z + 1) - vec3(x, height, z));

            local norm1 = cross(lineNegY, lineNegX);
            local norm2 = cross(linePosX, lineNegY);

            local norm3 = cross(linePosY, linePosX);
            local norm4 = cross(lineNegX, linePosY);

            local finalNorm = (norm1 + norm2 + norm3 + norm4) / vec3(4, 4, 4)
            local steep = dot(finalNorm, vec3(0, 1, 0)) < 0.7

            for y = 0, 255 do
			local index = (y * 16 * 16) + (x * 16) + z;

            	if not getCave(x + (chunk_x * 16), y, z + (chunk_z * 16)) then
            		

	                local actualHeight = y

	                if actualHeight < height then
	                    if steep then
	                        chunk[index] = fillerBlock
	                    else
	                        if actualHeight < height - 5 then
	                           chunk[index] = fillerBlock
	                        elseif actualHeight < height - 1 then
	                            chunk[index] = midBlock
	                        else
	                            chunk[index] = topBlock
	                        end
	                    end
	                else
	                	chunk[index] = 0
	                end
            	else
            		chunk[index] = 0
            	end
            end
        end
    end

    return chunk
end

api.initServer = function()
	local chunk = ffi.new("uint8_t[?]", 16*16*256)

	for x = 0, 31 do
		for z = 0, 31 do
			generateChunk(x, z, chunk)
			C.setChunk(x, z, chunk)
		end
	end
end

api.registerBlock(0, {
    name = "Air",
    solid = false,
    textures = {
        {0, 0}
    }
})

api.registerBlock(1, {
    name = "Grass",
    textures = {
        {3, 0},
        {3, 0},
        {0, 0},
        {2, 0},
        {3, 0},
        {3, 0}
    }
})

api.registerBlock(2, {
    name = "Stone",
    textures = {
        {1, 0}
    }
})

api.registerBlock(3, {
    name = "Dirt",
    textures = {
        {2, 0}
    }
})

api.registerBlock(4, {
    name = "Cobblestone",
    textures = {
        {0, 1}
    }
})

api.registerBlock(5, {
    name = "Planks",
    textures = {
        {4, 0}
    }
})

api.registerBlock(6, {
    name = "Wood",
    textures = {
        {4, 1}
    }
})

api.registerBlock(7, {
    name = "Leaves",
    textures = {
        {4, 3}
    },
    solid = false
})

api.registerBlock(8, {
    name = "Glowstone",
    textures = {
        {9, 6}
    }
})

api.registerBlock(9, {
    name = "Iron",
    textures = {
        {6, 1}
    }
})

api.registerBlock(10, {
    name = "Gold",
    textures = {
        {7, 1}
    }
})