print("Running Lua code...")


--
--api.registerBlock(0, {
--    name = "Air",
--    solid = false,
--    textures = {
--        {0, 0}
--    }
--})

api.registerBlock("base:stone", {
    name = "Stone",
    textures = {
        {1, 0}
    }
})

api.registerBlock("base:dirt", {
    name = "Dirt",
    textures = {
        {2, 0}
    }
})

api.registerBlock("base:grass", {
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



api.registerBlock("base:cobblestone", {
    name = "Cobblestone",
    textures = {
        {0, 1}
    }
})

api.registerBlock("base:planks", {
    name = "Planks",
    textures = {
        {4, 0}
    }
})

api.registerBlock("base:wood", {
    name = "Wood",
    textures = {
        {4, 1}
    }
})

api.registerBlock("base:leaves", {
    name = "Leaves",
    textures = {
        {4, 3}
    },
    solid = false
})

api.registerBlock("base:glowstone", {
    name = "Glowstone",
    textures = {
        {9, 6}
    },
    lightLevel = 15
})

api.registerBlock(9, {
    name = "Iron",
    textures = {
        {6, 1}
    }
})

api.registerBlock("base:gold", {
    name = "Gold",
    textures = {
        {7, 1}
    }
})

api.registerBlock("base:bricks", {
    name = "Bricks",
    textures = {
        {7, 0}
    }
})

api.registerBlock("base:water", {
    name = "Water",
    textures = {
        {14, 0}
    }
})