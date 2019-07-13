local math = require("math")
local noise2D = require("base:simplex2D.lua")
local arrays = require("base:arrays.lua")

local size = 16*128

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

    return multiplier * (ridgedNoise(x/5, z/5, 4, 0.01, 0.5) * 40 + 50)
end

local topBlock = api.getBlock("base:grass"):getID() --grass
local midBlock = api.getBlock("base:dirt"):getID() --dirt
local fillerBlock = api.getBlock("base:stone"):getID() --stone
local waterBlock = api.getBlock("base:water"):getID()
local shoreBlock = api.getBlock("base:sand"):getID()

local heights = arrays.array2D(18) --heightmap includes adjacent chunk blocks

local function getSteep(x, z)
    local height = heights[x+1][z+1]

    local lineNegX =  vecmath.normalize(vec3(x - 1, heights[x+2][z+1], z) - vec3(x, height, z))
    local linePosX =  vecmath.normalize(vec3(x + 1, heights[x][z+1], z) - vec3(x, height, z))

    local lineNegY =  vecmath.normalize(vec3(x, heights[x+1][z], z - 1) - vec3(x, height, z))
    local linePosY =  vecmath.normalize(vec3(x, heights[x+1][z+2], z + 1) - vec3(x, height, z))

    local norm1 = vecmath.cross(lineNegY, lineNegX)
    local norm2 = vecmath.cross(linePosX, lineNegY)

    local norm3 = vecmath.cross(linePosY, linePosX)
    local norm4 = vecmath.cross(lineNegX, linePosY)

    local finalNorm = (norm1 + norm2 + norm3 + norm4) / vec3(4, 4, 4)
    return dot(finalNorm, vec3(0, 1, 0)) < 0.7
end

local wood = api.getBlock("base:wood"):getID()
local leaves = api.getBlock("base:leaves"):getID()

local function placeTree(x, y, z, world)
    world:setBlockRaw(x, y, z, wood)
    world:setBlockRaw(x, y+1, z, wood)
    world:setBlockRaw(x, y+2, z, wood)
    world:setBlockRaw(x, y+3, z, wood)
    world:setBlockRaw(x, y+4, z, wood)
    world:setBlockRaw(x, y+5, z, wood)
    world:setBlockRaw(x, y+6, z, wood)

    world:setBlockRaw(x-1, y+3, z, leaves)
    world:setBlockRaw(x+1, y+3, z, leaves)
    world:setBlockRaw(x, y+3, z-1, leaves)
    world:setBlockRaw(x, y+3, z+1, leaves)


    world:setBlockRaw(x-1, y+4, z, leaves)
    world:setBlockRaw(x+1, y+4, z, leaves)
    world:setBlockRaw(x, y+4, z-1, leaves)
    world:setBlockRaw(x, y+4, z+1, leaves)
    world:setBlockRaw(x-1, y+4, z-1, leaves)
    world:setBlockRaw(x+1, y+4, z+1, leaves)
    world:setBlockRaw(x-1, y+4, z+1, leaves)
    world:setBlockRaw(x+1, y+4, z-1, leaves)


    world:setBlockRaw(x-1, y+5, z, leaves)
    world:setBlockRaw(x+1, y+5, z, leaves)
    world:setBlockRaw(x, y+5, z-1, leaves)
    world:setBlockRaw(x, y+5, z+1, leaves)
    world:setBlockRaw(x-1, y+5, z-1, leaves)
    world:setBlockRaw(x+1, y+5, z+1, leaves)
    world:setBlockRaw(x-1, y+5, z+1, leaves)
    world:setBlockRaw(x+1, y+5, z-1, leaves)


    world:setBlockRaw(x-1, y+6, z, leaves)
    world:setBlockRaw(x+1, y+6, z, leaves)
    world:setBlockRaw(x, y+6, z-1, leaves)
    world:setBlockRaw(x, y+6, z+1, leaves)

    world:setBlockRaw(x, y+7, z, leaves)
    world:setBlockRaw(x-1, y+7, z, leaves)
    world:setBlockRaw(x+1, y+7, z, leaves)
    world:setBlockRaw(x, y+7, z-1, leaves)
    world:setBlockRaw(x, y+7, z+1, leaves)

    world:setBlockRaw(x, y+8, z, leaves)
    world:setBlockRaw(x, y+9, z, leaves)
    world:setBlockRaw(x, y+10, z, leaves)

end


decorateChunk = function(chunk_x, chunk_z, world)
    for i = 0, math.random(0, 4) do
        local x = chunk_x*16 + math.random(0, 15)
        local z = chunk_z*16 + math.random(0, 15)

        if noise2D(x/255, z/255) > 0 then
            for y = 255, 0, -1 do
                if world:getBlock(x, y, z):getID() == 0 then

                elseif world:getBlock(x, y, z):getID() == topBlock then
                    placeTree(x, y+1, z, world)
                else
                    break
                end
            end
        end
    end
end


generateChunk = function(chunk_x, chunk_z, chunk)
    local data = internal.getChunkData(chunk)

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

                if y >= 53 and steep then
                    data[index] = fillerBlock
                else
                    if y < height - 5 then
                        data[index] = fillerBlock
                    elseif y < height then
                        if y < 53 then
                            data[index] = shoreBlock
                        else
                            data[index] = midBlock
                        end
                    elseif y == height then
                        if y < 53 then
                            data[index] = shoreBlock
                        else
                            data[index] = topBlock
                        end
                    elseif y < 50 then
                        data[index] = waterBlock
                    end
                end

            end
        end
    end
end