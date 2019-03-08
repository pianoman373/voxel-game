print("Running Lua code...")

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

api.registerBlock(11, {
    name = "Bricks",
    textures = {
        {7, 0}
    }
})

api.registerBlock(12, {
    name = "Thing",
    textures = {
        {8, 0}
    }
})

hotbarSelectorPos = 0

local function renderHotbar(width, height, img)
    local blockSize = 50

    hotbarSelectorPos = hotbarSelectorPos - api.getScroll();

    if hotbarSelectorPos < 0 then
        hotbarSelectorPos = 9
    end
    if hotbarSelectorPos > 11 then
        hotbarSelectorPos = 0
    end

    local xOffset = (width/2) - (blockSize * 6)

    for i = 0, 11 do
    	api.renderSpriteTexture(xOffset + (blockSize * i), height-blockSize,     blockSize, blockSize,           (1/8)*1, (1/8)*1, (1/8)*2, (1/8)*2,  img)

        api.renderBlockItem(i + 1, xOffset + (blockSize * i) + (blockSize/2), height-(blockSize/2), 25);
    end

    api.renderSpriteTexture(xOffset - (blockSize) + (hotbarSelectorPos * blockSize), height-(blockSize*2),     blockSize*3, blockSize*3,           (1/8)*2, (1/8)*0, (1/8)*5, (1/8)*3,  img)

end

local function renderInventory(width, height, img)
    local originX = 0
    local originY = 0

    local blockSize = 50

    for y = 0, 12 do
        for x = 0, 9 do
            api.renderSpriteTexture((width/2) - (blockSize*4.5) + (blockSize * x), (blockSize*y) + (blockSize*1.5),     blockSize, blockSize,           (1/8)*1, (1/8)*1, (1/8)*2, (1/8)*2,  img)

    --        api.renderBlockItem(x + 5, (width/2) + (blockSize * x) - (blockSize/2), height-(blockSize/2), 25);
        end
    end

end

api.registerEventHandler("renderGUI", function(width, height)
    local img = api.getTexture("base:textures/GUI.png")

    local crosshairLength = 20
    local crosshairThickness = 3

    -- crosshair
    api.renderSpriteColor((width/2) - (crosshairLength/2), (height/2) - (crosshairThickness/2),	20.0, 3.0,	1.0, 1.0, 1.0, 1.0)
    api.renderSpriteColor((width/2) - (crosshairThickness/2), (height/2) - (crosshairLength/2),	3.0, 20.0,	1.0, 1.0, 1.0, 1.0)

    if api.isKeyDown(258) then
        api.renderSpriteColor(0, 0, width, height, 0.0, 0.0, 0.0, 0.8);
        renderInventory(width, height, img)
    end

    renderHotbar(width, height, img)


end)

api.registerEventHandler("client_init", function()
    print("Being initialized by the client!")

    api.registerTexture("base:textures/GUI.png")
end)