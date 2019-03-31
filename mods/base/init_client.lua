local gui = require("base:gui.lua")
local string = require("string")


hotbarSelectorPos = 0
local inventory = false
local hideUI = false

api.registerTexture("base:textures/background.png")

local function renderHotbar(width, height, img)
    local blockSize = 50

    hotbarSelectorPos = hotbarSelectorPos - api.input.getScroll();

    if hotbarSelectorPos < 0 then
        hotbarSelectorPos = 11
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

api.registerEventHandler("gui_ingame", function(width, height)
    if hideUI then
        return
    end


    local img = api.getTexture("base:textures/GUI.png")

    local crosshairLength = 20
    local crosshairThickness = 3

    -- crosshair
    api.renderSpriteColor((width/2) - (crosshairLength/2), (height/2) - (crosshairThickness/2),	20.0, 3.0,	1.0, 1.0, 1.0, 1.0)
    api.renderSpriteColor((width/2) - (crosshairThickness/2), (height/2) - (crosshairLength/2),	3.0, 20.0,	1.0, 1.0, 1.0, 1.0)

    if inventory then
        api.renderSpriteColor(0, 0, width, height, 0.0, 0.0, 0.0, 0.8)
        renderInventory(width, height, img)
    end

    renderHotbar(width, height, img)

    gui.text("Framerate: " .. string.format("%.1f",api.getFramerate()), 0, height - 30)
    gui.text("Frametime: " .. string.format("%.1f",api.getDelta()*1000) .. "ms", 0, height - 50)

end)

local address = "localhost:3737"

api.registerEventHandler("gui_main_menu", function(width, height)
    local background = api.getTexture("base:textures/background.png")

    api.renderSpriteTexture(0, 0, width, height, 0, 0, 1, 1, background)

    gui.textCentered("Cube Quest", width/2, height - 100)


    if gui.button("Singleplayer", 0, 300, 300, 50) then
        api.connectToIntegratedServer()
    end

    if gui.button("Multiplayer", 0, 200, 300, 50) then
        api.connectToServer("localhost", 1000)
    end

    address = gui.textBox("Address", address, 0, 150, 200)
end)

api.registerEventHandler("client_init", function()
    print("Being initialized by the client!")
end)

api.registerEventHandler("key_press", function(key)
    if key == api.input.KEY_TAB then
        inventory = not inventory
    end

    if key == api.input.KEY_F2 then
        hideUI = not hideUI
    end

    if key == api.input.KEY_1 then
        hotbarSelectorPos = 0
    end
    if key == api.input.KEY_2 then
        hotbarSelectorPos = 1
    end
    if key == api.input.KEY_3 then
        hotbarSelectorPos = 2
    end
    if key == api.input.KEY_4 then
        hotbarSelectorPos = 3
    end
    if key == api.input.KEY_5 then
        hotbarSelectorPos = 4
    end
    if key == api.input.KEY_6 then
        hotbarSelectorPos = 5
    end
    if key == api.input.KEY_7 then
        hotbarSelectorPos = 6
    end
    if key == api.input.KEY_8 then
        hotbarSelectorPos = 7
    end
    if key == api.input.KEY_9 then
        hotbarSelectorPos = 8
    end
    if key == api.input.KEY_0 then
        hotbarSelectorPos = 9
    end
    if key == api.input.KEY_MINUS then
        hotbarSelectorPos = 10
    end
    if key == api.input.KEY_EQUAL then
        hotbarSelectorPos = 11
    end


end)