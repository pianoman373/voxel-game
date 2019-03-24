local gui = require("base:gui.lua")


local hotbarSelectorPos = 0

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
        api.renderSpriteTexture(xOffset + (blockSize * i), 0,     blockSize, blockSize,           (1/8)*1, (1/8)*1, (1/8)*2, (1/8)*2,  img)

        api.renderBlockItem(i + 1, xOffset + (blockSize * i) + (blockSize/2), blockSize / 2, 25);
    end

    api.renderSpriteTexture(xOffset - (blockSize) + (hotbarSelectorPos * blockSize), -(blockSize),     blockSize*3, blockSize*3,           (1/8)*2, (1/8)*0, (1/8)*5, (1/8)*3,  img)

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



    gui.text(0, 100, "Hello World!")
end)

api.registerEventHandler("client_init", function()
    print("Being initialized by the client!")


end)