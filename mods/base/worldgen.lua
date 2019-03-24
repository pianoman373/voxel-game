local ffi = require("ffi")
ffi.cdef[[
    float ridgedNoise(float x, float y, int octaves, float frequency, float persistence);
    void setChunk(int x, int z, uint8_t *data);
    float noise3D(float x, float y, float z);
]]
local C = ffi.C

require("base:api.lua")

--helper function
local function getHeight(x, z)
    return C.ridgedNoise(x/10, z/10, 5, 0.01, 0.5) * 80 + 120
end

local function getCave(x, y, z)
    return C.noise3D(x/100, y/100, z/100) > 0.6;
end

local topBlock = 1 --grass
local midBlock = 3 --dirt
local fillerBlock = 2 --stone

local heights = array2D(18) --heightmap includes adjacent chunk blocks


generateWorld = function(chunk_x, chunk_z, chunk)
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

            for y = 0, height do
                if not getCave(x + (chunk_x * 16), y, z + (chunk_z * 16)) then


                    if steep then
                        chunk:setBlockRaw(x, y, z, fillerBlock)
                    else
                        if y < height - 5 then
                            chunk:setBlockRaw(x, y, z, fillerBlock)
                        elseif y < height - 1 then
                            chunk:setBlockRaw(x, y, z, midBlock)
                        else
                            chunk:setBlockRaw(x, y, z, topBlock)
                        end
                    end
                end
            end
        end
    end
end