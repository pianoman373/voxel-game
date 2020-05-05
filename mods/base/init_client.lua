local gui = require("base:gui.lua")
local string = require("string")
local PlayerController = require("base:PlayerController.lua")
local InventoryGUI = require("base:inventoryGUI.lua")
local HudGUI = require("base:HudGUI.lua")
local MainMenuGUI = require("base:MainMenuGUI.lua")
local math = require("math")
local EntitySkeleton = require("base:EntitySkeleton.lua")

local inventory = false
local inventoryGUI = InventoryGUI:new(PlayerController.inventory)
local hudGUI = HudGUI:new(PlayerController)
local mainMenuGUI = MainMenuGUI:new()



api.registerEntity("base:skeleton", EntitySkeleton)


api.registerPlayerController(PlayerController);

api.getTexture("base:resources/textures/background.png")


api.registerEventHandler("gui_ingame", function(width, height)
    if hideUI then
        return
    end

    if api.input.isKeyPressed(api.input.KEY_TAB) then
        if inventory then
            inventory = false
            api.input.setMouseGrabbed(true)
        else
            inventory = true
            api.input.setMouseGrabbed(false)
        end
    end

    if inventory then
        api.renderSpriteColor(0, 0, width, height, 0.0, 0.0, 0.0, 0.8)
        inventoryGUI:render(width, height)
    else
        hudGUI:render(width, height)
    end
end)

api.registerEventHandler("gui_main_menu", function(width, height)
    mainMenuGUI:render(width, height)
end)