local class = require("base:class.lua")

local gui = {}



api.getTexture("base:resources/textures/GUI.png")
local font = api.getFont("base:resources/HelvetiPixel.ttf")

function gui.drawText(text, x, y, r, g, b, a)
    api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

function gui.drawTextCentered(text, x, y, r, g, b, a)
    local textSize = font:getTextSize(text)

    x = x - (textSize.x/2)
    y = y - (textSize.y/2)

    api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

function gui.isMouseInBox(x, y, width, height)
    local cur = api.input.getCursorPos()

    if cur.x > x and cur.x < x+width and cur.y > y and cur.y < y+height then
        return true
    end

    return false
end

function gui.button(x, y, width, height, text, onClick)

    if gui.isMouseInBox(x, y, width, height) then
        if api.input.isMouseButtonDown(0) then
            api.renderSprite(x, y, width, height, 0.4, 0.4, 0.4, 1)
        else
            api.renderSprite(x, y, width, height, 0.5, 0.5, 0.5, 1)
        end

        if api.input.isMouseButtonPressed(0) then
            onClick()
        end
    else
        api.renderSprite(x, y, width, height, 0.6, 0.6, 0.6, 1)
    end

    gui.drawTextCentered(text, x + (width/2), y + (height/2))
end

TextBox = class("TextBox")

function TextBox:initialize (text)
    self.editing = false
    self.content = text or ""
    self.cursorTimer = 0
    self.cursorBlink = true
    self.cursorPosition = 1
end

function TextBox:render(x, y, width, height)
    local hovered = gui.isMouseInBox(x, y, width, height)

    if hovered and api.input.isMouseButtonDown(0) and not self.editing then
        self.editing = true
    end

    if not hovered and api.input.isMouseButtonDown(0) then
        self.editing = false
        
    end

    local margin = 2

    api.renderSprite(x - margin, y - margin, width + margin*2, height + margin*2, 0.9, 0.9, 0.9, 1)

    if self.editing then
        api.renderSprite(x, y, width, height, 0.0, 0.0, 0.0, 1)
    else
        api.renderSprite(x, y, width, height, 1.1, 0.1, 0.1, 1)
    end

    api.renderSprite(x, y, width, height, 0.1, 0.1, 0.1, 1)

    gui.drawText(self.content, x, y, 1.0, 1.0, 1.0, 1.0)

    local textWidth, textHeight = font:getTextSize(self.content:sub(1, -self.cursorPosition))

    
    if self.editing then
        self.cursorTimer = self.cursorTimer + api.getDelta()

        if self.cursorTimer > 0.5 then
            self.cursorBlink = not self.cursorBlink
            self.cursorTimer = 0
        end

        if self.cursorBlink then
            api.renderSprite(x + textWidth, y, 2, textHeight, 1.0, 1.0, 1.0, 1.0)
        end

        if api.input.isKeyPressed(api.input.KEY_BACKSPACE) then

            if self.cursorPosition == 1 then
                self.content = self.content:sub(1, -2)
            else
                self.content = self.content:sub(1, -self.cursorPosition-1) .. self.content:sub(-self.cursorPosition+1, -1)
            end
            
        end

        if api.input.isKeyPressed(api.input.KEY_LEFT) and self.cursorPosition <= self.content:len() then
            self.cursorPosition = self.cursorPosition + 1
            self.cursorTimer = 0
            self.cursorBlink = true
        end

        if api.input.isKeyPressed(api.input.KEY_RIGHT) and self.cursorPosition > 1 then
            self.cursorPosition = self.cursorPosition - 1
            self.cursorTimer = 0
            self.cursorBlink = true
        end
        
        local char = api.input.getCharPresses()

        if self.cursorPosition == 1 then
            self.content = self.content .. char
        else
            self.content = self.content:sub(1, -self.cursorPosition) .. char .. self.content:sub(-self.cursorPosition+1, -1)
        end
    end
    
end

local function isEmpty(itemStack)
    if itemStack ~= nil and itemStack.item ~= nil and itemStack.count > 0 then
        return false
    else
        return true
    end
end

ItemSlot = class("ItemSlot")

grabbedItemstack = {
    item = nil,
    count = 0,
}

function ItemSlot:initialize(itemStack)
    self.itemStack = itemStack
    self.grabbed = false
end

function ItemSlot:render(x, y, width, height)
    local cur = api.input.getCursorPos()
    
    if gui.isMouseInBox(x, y, width, height) then
        api.renderSprite(x, y, width, height, 0.5, 0.5, 0.5, 0.5)

        if api.input.isMouseButtonPressed(0) then
            if not isEmpty(grabbedItemstack) then

                if isEmpty(self.itemStack) then
                    self.itemStack.item = grabbedItemstack.item
                    self.itemStack.count = grabbedItemstack.count
                    grabbedItemstack.item = nil
                    grabbedItemstack.count = 0
                else
                    local item = self.itemStack.item
                    local count = self.itemStack.count

                    self.itemStack.item = grabbedItemstack.item
                    self.itemStack.count = grabbedItemstack.count

                    grabbedItemstack.item = item
                    grabbedItemstack.count = count
                end
                
            else
                self.grabbed = true
                grabbedItemstack.item = self.itemStack.item
                grabbedItemstack.count = self.itemStack.count
                self.itemStack.item = nil
                self.itemStack.count = 0
            end
        end
    end
    
    if self.grabbed then

        if not isEmpty(grabbedItemstack)  then
            --api.renderBlockItem(grabbedItemstack.item:getID(), cur.x, cur.y, 30)
            grabbedItemstack.item:draw(cur.x, cur.y)

            gui.drawText(tostring(grabbedItemstack.count), cur.x, cur.y, 1, 1, 1, 1)
        else
            self.grabbed = false
        end
    else
        if not isEmpty(self.itemStack) then
            --api.renderBlockItem(self.itemStack.item:getID(), x + (width/2), y + (height/2), 30)
            self.itemStack.item:draw(x + (width/2), y + (height/2))

            gui.drawText(tostring(self.itemStack.count), x + (width/2), y + (height/2), 1, 1, 1, 1)
        end
    end
end



return gui