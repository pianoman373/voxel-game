local math = require("math")
local noise2D = require("base:simplex2D.lua")

local ffi = require("ffi")
ffi.cdef[[
unsigned char* getChunkData(void *c)
]]



require("base:api.lua")

local size = 16*32

local function ridgedNoise(x, y, octaves, frequency, persistence)
    local total = 0.0
    local maxAmplitude = 0.0
    local amplitude = 1.0
    for i = 0, octaves do
        total = total + ((1.0 - math.abs(noise2D(x * frequency, y * frequency))) * 2.0 - 1.0) * amplitude
        frequency = frequency * 2.0
        maxAmplitude = maxAmplitude + amplitude
        amplitude = amplitude * persistence
    end

    return total / maxAmplitude
end

function clamp(val, lower, upper)
    if lower > upper then lower, upper = upper, lower end -- swap if boundaries supplied the wrong way
    return math.max(lower, math.min(upper, val))
end

--helper function
local function getHeight(x, z)
    local multiplier = clamp(x/128, 0, 1) * clamp(z/128, 0, 1) * clamp((size-z)/128, 0, 1) * clamp((size-x)/128, 0, 1)

    return multiplier * (ridgedNoise(x/5, z/5, 4, 0.01, 0.5) * 40 + 60)
end

local topBlock = 1 --grass
local midBlock = 3 --dirt
local fillerBlock = 2 --stone
local waterBlock = 12

local heights = array2D(18) --heightmap includes adjacent chunk blocks

local function getSteep(x, z)
    local height = heights[x+1][z+1]

    local lineNegX =  normalize(vec3(x - 1, heights[x+2][z+1], z) - vec3(x, height, z))
    local linePosX =  normalize(vec3(x + 1, heights[x][z+1], z) - vec3(x, height, z))

    local lineNegY =  normalize(vec3(x, heights[x+1][z], z - 1) - vec3(x, height, z))
    local linePosY =  normalize(vec3(x, heights[x+1][z+2], z + 1) - vec3(x, height, z))

    local norm1 = cross(lineNegY, lineNegX)
    local norm2 = cross(linePosX, lineNegY)

    local norm3 = cross(linePosY, linePosX)
    local norm4 = cross(lineNegX, linePosY)

    local finalNorm = (norm1 + norm2 + norm3 + norm4) / vec3(4, 4, 4)
    return dot(finalNorm, vec3(0, 1, 0)) < 0.7
end

local function placeTree(x, y, z, world)
    world:setBlockRaw(x, y, z, 6)
    world:setBlockRaw(x, y+1, z, 6)
    world:setBlockRaw(x, y+2, z, 6)
    world:setBlockRaw(x, y+3, z, 6)
    world:setBlockRaw(x, y+4, z, 6)
    world:setBlockRaw(x, y+5, z, 6)
    world:setBlockRaw(x, y+6, z, 6)

    world:setBlockRaw(x-1, y+3, z, 7)
    world:setBlockRaw(x+1, y+3, z, 7)
    world:setBlockRaw(x, y+3, z-1, 7)
    world:setBlockRaw(x, y+3, z+1, 7)


    world:setBlockRaw(x-1, y+4, z, 7)
    world:setBlockRaw(x+1, y+4, z, 7)
    world:setBlockRaw(x, y+4, z-1, 7)
    world:setBlockRaw(x, y+4, z+1, 7)
    world:setBlockRaw(x-1, y+4, z-1, 7)
    world:setBlockRaw(x+1, y+4, z+1, 7)
    world:setBlockRaw(x-1, y+4, z+1, 7)
    world:setBlockRaw(x+1, y+4, z-1, 7)


    world:setBlockRaw(x-1, y+5, z, 7)
    world:setBlockRaw(x+1, y+5, z, 7)
    world:setBlockRaw(x, y+5, z-1, 7)
    world:setBlockRaw(x, y+5, z+1, 7)
    world:setBlockRaw(x-1, y+5, z-1, 7)
    world:setBlockRaw(x+1, y+5, z+1, 7)
    world:setBlockRaw(x-1, y+5, z+1, 7)
    world:setBlockRaw(x+1, y+5, z-1, 7)


    world:setBlockRaw(x-1, y+6, z, 7)
    world:setBlockRaw(x+1, y+6, z, 7)
    world:setBlockRaw(x, y+6, z-1, 7)
    world:setBlockRaw(x, y+6, z+1, 7)

    world:setBlockRaw(x, y+7, z, 7)
    world:setBlockRaw(x-1, y+7, z, 7)
    world:setBlockRaw(x+1, y+7, z, 7)
    world:setBlockRaw(x, y+7, z-1, 7)
    world:setBlockRaw(x, y+7, z+1, 7)

    world:setBlockRaw(x, y+8, z, 7)
    world:setBlockRaw(x, y+9, z, 7)
    world:setBlockRaw(x, y+10, z, 7)

end


decorateChunk = function(chunk_x, chunk_z, world)
    for i = 0, math.random(0, 4) do
        local x = chunk_x*16 + math.random(0, 15)
        local z = chunk_z*16 + math.random(0, 15)

        if noise2D(x/255, z/255) > 0 then
            for y = 255, 0, -1 do
                if world:getBlock(x, y, z):getID() == 0 then

                elseif world:getBlock(x, y, z):getID() == 1 then
                    placeTree(x, y+1, z, world)
                else
                    break
                end

            end
        end



    end
end


generateChunk = function(chunk_x, chunk_z, chunk)
    local data = ffi.C.getChunkData(chunk)

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
                local index = (y * 16 * 16) + (x * 16) + z;

                if steep then
                    --chunk:setBlockRaw(x, y, z, fillerBlock)
                    data[index] = fillerBlock
                else
                    if y < height - 5 then
--                        chunk:setBlockRaw(x, y, z, fillerBlock)
                        data[index] = fillerBlock
                    elseif y < height then
--                        chunk:setBlockRaw(x, y, z, midBlock)
                        data[index] = midBlock
                    elseif y == height then
--                        chunk:setBlockRaw(x, y, z, topBlock)
                        data[index] = topBlock
                    elseif y < 50 then
--                        chunk:setBlockRaw(x, y, z, waterBlock)
                        data[index] = waterBlock
                    end
                end

            end
        end
    end
end