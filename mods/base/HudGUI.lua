local gui = require("base:gui.lua")
local class = require("base:class.lua")


local HudGUI = class("HudGUI")

local img = api.getTexture("base:resources/textures/GUI.png")


function HudGUI:initialize(player)
    self.player = player
end

function HudGUI:render(width, height)
    local blockSize = 60
    local crosshairLength = 20
    local crosshairThickness = 3

    -- crosshair
    api.renderSpriteColor((width/2) - (crosshairLength/2), (height/2) - (crosshairThickness/2),	20.0, 3.0,	1.0, 1.0, 1.0, 1.0)
    api.renderSpriteColor((width/2) - (crosshairThickness/2), (height/2) - (crosshairLength/2),	3.0, 20.0,	1.0, 1.0, 1.0, 1.0)

    self.player.selectedSlot = self.player.selectedSlot - api.input.getScroll();

    if self.player.selectedSlot < 1 then
        self.player.selectedSlot = 12
    end
    if self.player.selectedSlot > 12 then
        self.player.selectedSlot = 1
    end

    local xOffset = (width/2) - (blockSize * 6)

    for i = 0, 11 do
        api.renderSpriteTexture(xOffset + (blockSize * i), 0,     blockSize, blockSize,           (1/8)*1, (1/8)*1, (1/8)*2, (1/8)*2,  img)

        if not self.player.inventory:isSlotEmpty(i + 1) then
            local slot = self.player.inventory:getSlot(i + 1)
            --api.renderBlockItem(slot.item:getID(), xOffset + (blockSize * i) + (blockSize/2), blockSize / 2, 30)
            slot.item:draw(xOffset + (blockSize * i) + (blockSize/2), blockSize / 2)

            gui.drawText(tostring(slot.count), xOffset + (blockSize * i) + (blockSize/2), blockSize / 2, 1, 1, 1, 1)
        end
        
    end

    api.renderSpriteTexture(xOffset - (blockSize) + ((self.player.selectedSlot-1) * blockSize), -(blockSize),     blockSize*3, blockSize*3,           (1/8)*2, (1/8)*0, (1/8)*5, (1/8)*3,  img)
end


return HudGUI