local item = require("base:item.lua")


api.registerBlock("base:stone", {
    name = "Stone",
    textures = {
        "base:resources/blocks/stone.png"
    },
    onPlace = function(world, x, y, z)
        print("placing!")
        world:setBlock(x, y + 1, z, api.getBlock("base:dirt"))
    end
})
item.registerBlockItem("base:stone")

api.registerBlock("base:dirt", {
    name = "Dirt",
    textures = {
        "base:resources/blocks/dirt.png"
    }
})
item.registerBlockItem("base:dirt")

api.registerBlock("base:grass", {
    name = "Grass",
    textures = {
        "base:resources/blocks/grass.png",
        "base:resources/blocks/grass.png",
        "base:resources/blocks/grass-top.png",
        "base:resources/blocks/dirt.png",
        "base:resources/blocks/grass.png",
        "base:resources/blocks/grass.png"
    }
})
item.registerBlockItem("base:grass")



api.registerBlock("base:cobblestone", {
    name = "Cobblestone",
    textures = {
        "base:resources/blocks/cobblestone.png"
    }
})
item.registerBlockItem("base:cobblestone")

api.registerBlock("base:planks", {
    name = "Planks",
    textures = {
        "base:resources/blocks/planks.png"
    }
})
item.registerBlockItem("base:planks")

api.registerBlock("base:wood", {
    name = "Wood",
    textures = {
        "base:resources/blocks/wood.png"
    }
})
item.registerBlockItem("base:wood")

api.registerBlock("base:leaves", {
    name = "Leaves",
    textures = {
        "base:resources/blocks/leaves.png"
    },
    solid = false
})
item.registerBlockItem("base:leaves")

api.registerBlock("base:glowstone", {
    name = "Glowstone",
    textures = nil,
    lightLevel = 15
})
item.registerBlockItem("base:glowstone")

api.registerBlock("base:sand", {
    name = "Sand",
    textures = {
        "base:resources/blocks/sand.png"
    },
})

api.registerBlock("base:gold", {
    name = "Gold",
    textures = nil
})
item.registerBlockItem("base:gold")

api.registerBlock("base:bricks", {
    name = "Bricks",
    textures = nil
})
item.registerBlockItem("base:bricks")

api.registerBlock("base:water", {
    name = "Water",
    isLiquid = true,
    solid = false
})



item.register("base:stick", {
    texture = "base:resources/items/stick.png",
    name = "Stick"
})

item.register("base:skeleton_spawner", {
    texture = "base:resources/items/skeleton_spawner.png",
    name = "Skeleton Spawner",
    onUse = function(self, player, itemSlot)
        local blockPosition = vec3i()
        local blockNormal = vec3i()
        local hit = player.world:raycastBlocks(player.position + vec3(0, 0.7, 0), player.direction, 10, blockPosition, blockNormal)
        
        player.world:spawnEntity("base:skeleton", blockPosition.x + blockNormal.x, blockPosition.y + blockNormal.y + 2, blockPosition.z + blockNormal.z)
    end
})