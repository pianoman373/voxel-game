local ffi = require("ffi")
ffi.cdef[[
    float ridgedNoise(float x, float y, int octaves, float frequency, float persistence);
    void setChunk(int x, int z, uint8_t *data);
    float noise3D(float x, float y, float z);
]]
local C = ffi.C

local math = require("math")

require("base:api.lua")

local size = 16*32

function clamp(val, lower, upper)
    if lower > upper then lower, upper = upper, lower end -- swap if boundaries supplied the wrong way
    return math.max(lower, math.min(upper, val))
end

--helper function
local function getHeight(x, z)
    local multiplier = clamp(x/128, 0, 1) * clamp(z/128, 0, 1) * clamp((size-z)/128, 0, 1) * clamp((size-x)/128, 0, 1)

    return multiplier * (C.ridgedNoise(x/10, z/10, 5, 0.01, 0.5) * 60 + 80)
end

local function getCave(x, y, z)
    return C.noise3D(x/100, y/100, z/100) > 0.6;
end

local topBlock = 1 --grass
local midBlock = 3 --dirt
local fillerBlock = 2 --stone
local waterBlock = 12

local heights = array2D(18) --heightmap includes adjacent chunk blocks

local function getSteep(x, z)
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
    return dot(finalNorm, vec3(0, 1, 0)) < 0.7
end


generateWorld = function(chunk_x, chunk_z, chunk, world)
    
    for x = 0, 17 do
        for z = 0, 17 do
            heights[x][z] = getHeight(x-1 + (chunk_x * 16), z-1 + (chunk_z * 16))
        end
    end

    for x = 0, 15 do
        for z = 0, 15 do
            local height = math.floor(heights[x+1][z+1])
            local steep = getSteep(x, z)

            for y = 0, math.max(height, 50) do

                if steep then
                    chunk:setBlockRaw(x, y, z, fillerBlock)
                else
                    if y < height - 5 then
                        chunk:setBlockRaw(x, y, z, fillerBlock)
                    elseif y < height - 1 then
                        chunk:setBlockRaw(x, y, z, midBlock)
                    elseif y == height then
                        chunk:setBlockRaw(x, y, z, topBlock)
                    elseif y < 50 then
                        chunk:setBlockRaw(x, y, z, waterBlock)
                    end
                end

            end
        end
    end
end