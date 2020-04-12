local gui = require("base:gui.lua")
local string = require("string")
local PlayerController = require("base:PlayerController.lua")
local InventoryGUI = require("base:inventoryGUI.lua")
local HudGUI = require("base:HudGUI.lua")
local MainMenuGUI = require("base:MainMenuGUI.lua")
local math = require("math")

local inventory = false


local inventoryGUI = InventoryGUI.new(PlayerController.inventory)
local hudGUI = HudGUI.new(PlayerController)


local Skeleton = {
    name = "Skeleton",
    walkTime = 40
}

function Skeleton:new()
    self.entity.width = 0.8
    self.entity.height = 1.8
    self.entity.depth = 0.8
end

function Skeleton:pickNewDirection()
    local direction = vec2(0, 0)

    direction.x = (math.random() * 2) - 1
    direction.y = (math.random() * 2) - 1

    direction = vecmath.normalize(direction) * 0.1
    
    if math.random() > 0.5 then
        self.entity.xVelocity = direction.x
        self.entity.zVelocity = direction.y
    else
        self.entity.xVelocity = 0
        self.entity.zVelocity = 0
    end

    self.entity.xDirection = direction.x
    self.entity.zDirection = direction.y

    self.walkTime = math.random(20, 100)
end

function Skeleton:tick()
    if self.walkTime == 0 then
        self:pickNewDirection()
    else
        self.walkTime = self.walkTime - 1
    end

    if self.entity.collidedHorizontal and self.entity.onGround then
        self.entity.yVelocity = 0.4
    end
end

api.registerEntity("base:skeleton", Skeleton)


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
    MainMenuGUI.render(width, height)
end)