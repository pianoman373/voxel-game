print("Running Lua code...")

api.registerBlock("base:stone", {
    name = "Stone",
    textures = {
        "base:resources/blocks/stone.png"
    }
})

api.registerBlock("base:dirt", {
    name = "Dirt",
    textures = {
        "base:resources/blocks/dirt.png"
    }
})

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



api.registerBlock("base:cobblestone", {
    name = "Cobblestone",
    textures = {
        "base:resources/blocks/cobblestone.png"
    }
})

api.registerBlock("base:planks", {
    name = "Planks",
    textures = {
        "base:resources/blocks/planks.png"
    }
})

api.registerBlock("base:wood", {
    name = "Wood",
    textures = {
        "base:resources/blocks/wood.png"
    }
})

api.registerBlock("base:leaves", {
    name = "Leaves",
    textures = {
        "base:resources/blocks/leaves.png"
    },
    solid = false
})

api.registerBlock("base:glowstone", {
    name = "Glowstone",
    textures = nil,
    lightLevel = 15
})

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

api.registerBlock("base:bricks", {
    name = "Bricks",
    textures = nil
})

api.registerBlock("base:water", {
    name = "Water",
    isLiquid = true,
    solid = false
})