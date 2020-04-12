
local gui = {}

local type_pixels = 1
local type_percent = 2
local type_aspect = 3

local font = api.getFont("base:resources/Roboto-Regular.ttf")

function drawText(text, x, y, r, g, b, a)
    --api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

function drawTextCentered(text, x, y, r, g, b, a)
    local textSize = font:getTextSize(text)

    x = x - (textSize.x/2)
    y = y - (textSize.y/2)

    --api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

function pixels(val)
    return {value=val, type=type_pixels}
end

function percent(val)
    return {value=val, type=type_percent}
end

function aspect(val)
    return {value=val, type=type_aspect}
end

function rgba(r, g, b, a)
    return {r, g, b, a}
end

local function drawOrigin(x, y)
    api.renderSprite(x-6, y-1, 12, 2, 1, 1, 1, 1, 0)
    api.renderSprite(x-1, y-6, 2, 12, 1, 1, 1, 1, 0)
    
    api.renderSprite(x-3, y-3, 6, 6, 1, 1, 1, 1, 0)
    api.renderSprite(x-1, y-1, 2, 2, 0.1, 0.1, 0.1, 1, 0)
end

local function getOriginOffset(origin)
    local offsetX = 0.5
    local offsetY = 0.5

    --print(origin)

    if origin:find('l') then
        offsetX = 0
    end
    if origin:find('r') then
        offsetX = 1
    end

    if origin:find('b') then
        offsetY = 0
    end
    if origin:find('t') then
        offsetY = 1
    end

    return offsetX, offsetY
end

function getScreenDimensions(panel, parentX, parentY, parentWidth, parentHeight)
    local x = 0
    local y = 0
    local width = 0
    local height = 0

    if panel.x.type == type_pixels then
        x = panel.x.value
    end
    if panel.x.type == type_percent then
        x = panel.x.value*0.01*parentWidth
    end

    if panel.y.type == type_pixels then
        y = panel.y.value
    end
    if panel.y.type == type_percent then
        y = panel.y.value*0.01*parentHeight
    end

    if panel.width.type == type_pixels then
        width = panel.width.value
    end
    if panel.width.type == type_percent then
        width = panel.width.value*0.01*parentWidth
    end

    if panel.width.type == type_pixels then
        width = panel.width.value
    end
    if panel.width.type == type_percent then
        width = panel.width.value*0.01*parentWidth
    end

    if panel.height.type == type_pixels then
        height = panel.height.value
    end
    if panel.height.type == type_percent then
        height = panel.height.value*0.01*parentHeight
    end

    if panel.height.type == type_aspect then
        height = width * panel.height.value
    end
    if panel.width.type == type_aspect then
        width = height * panel.width.value
    end

    local xOff, yOff = getOriginOffset(panel.origin)
    local xAnchor, yAnchor = getOriginOffset(panel.anchor)

    if xAnchor > 0.5 then x = -x end
    if yAnchor > 0.5 then y = -y end

    x = x - (xOff*width)
    y = y - (yOff*height)

    return x+parentX + (xAnchor * parentWidth), y+parentY + (yAnchor * parentHeight), width, height
end

local function isMouseInBox(mouseX, mouseY, x, y, width, height)
    if mouseX > x and mouseX < x+width and mouseY > y and mouseY < y+height then
        return true
    end

    return false
end

UIelement = {}
UIelement.x = percent(0)
UIelement.y = percent(0)
UIelement.width = percent(100)
UIelement.height = percent(100)
UIelement.origin = 'bl'
UIelement.anchor = 'bl'
UIelement.isHovered = false
UIelement.roundRadius = 0
UIelement.active = true

UIelement.onClick = function(self) end
UIelement.onFocusLost = function(self) end

function UIelement:new(t)
    local o = setmetatable(t or {}, {__index=self})

    return o
end

function UIelement:draw(x, y, width, height)
    
end

function UIelement:render(screenWidth, screenHeight)
    local cur = api.input.getCursorPos()
    local mouseX = cur.x
    local mouseY = cur.y

    local isClick = api.input.isMouseButtonPressed(0)

    self:internalLoop1(0, 0, screenWidth, screenHeight, mouseX, mouseY)

    self:internalLoop2(0, 0, screenWidth, screenHeight, isClick)

end

function UIelement:internalLoop1(parentX, parentY, parentWidth, parentHeight, mouseX, mouseY, parent)
    if not self.active then return end

    local x, y, width, height = getScreenDimensions(self, parentX, parentY, parentWidth, parentHeight)

    if isMouseInBox(mouseX, mouseY, x, y, width, height) then
        self.isHovered = true

        if parent then
            parent.isHovered = false
        end
    else
        self.isHovered = false
    end

    if self.children then
        for k, v in pairs(self.children) do
            v:internalLoop1(x, y, width, height, mouseX, mouseY, self)
        end
    end

end

function UIelement:internalLoop2(parentX, parentY, parentWidth, parentHeight, isClick)
    if not self.active then return end

    local x, y, width, height = getScreenDimensions(self, parentX, parentY, parentWidth, parentHeight)
    local xOff, yOff = getOriginOffset(self.origin)

    

    self:draw(x, y, width, height)

    if self.isHovered then
        drawOrigin(x+(xOff*width), y+(yOff*height))

        if isClick then
            self:onClick()
        end
    else
        if isClick then
            self:onFocusLost()
        end
    end

    if self.children then
        for k, v in pairs(self.children) do
            v:internalLoop2(x, y, width, height, isClick)
        end
    end
end

Panel = UIelement:new()
Panel.color = rgba(0.0, 0.0, 0.0, 0.0)


function Panel:new(t)
    local o = setmetatable(t or {}, {__index=self})

    o.children = {}

    return o
end

function Panel:draw(x, y, width, height)
    if self.image then
        api.renderSpriteTexture(0, 0, width, height, 0, 0, 1, 1, self.image, self.roundRadius)
    else
        api.renderSprite(x, y, width, height, self.color[1], self.color[2], self.color[3], self.color[4], self.roundRadius)
    end
    
end

function Panel:addChild(child)
    table.insert(self.children, child)

    return child
end


Button = UIelement:new()
Button.color = rgba(0.5, 0.5, 0.5, 1.0)
Button.hoverColor = rgba(0.6, 0.6, 0.6, 1.0)
Button.text = "button"

function Button:new(t)
    local o = setmetatable(t or {}, {__index=self})

    

    return o
end

function Button:draw(x, y, width, height)
    
    if self.isHovered then
        api.renderSprite(x, y, width, height, self.hoverColor[1], self.hoverColor[2], self.hoverColor[3], self.hoverColor[4], self.roundRadius)
    else
        api.renderSprite(x, y, width, height, self.color[1], self.color[2], self.color[3], self.color[4], self.roundRadius)
    end

    drawTextCentered(self.text, x + (width/2), y + (height/2), 0, 0, 0, 1)
end

Label = UIelement:new()
Label.color = rgba(0.0, 0.0, 0.0, 1.0)
Label.text = "label"

function Label:new(t)
    local o = setmetatable(t or {}, {__index=self})

    return o
end

function Label:draw(x, y, width, height)
    local textSize = font:getTextSize(self.text)

    self.width = pixels(textSize.x)
    self.height = pixels(textSize.y)

    drawText(self.text, x, y, self.color[1], self.color[2], self.color[3], self.color[4])
end

CheckBox = UIelement:new()
CheckBox.color = rgba(0.2, 0.2, 0.2, 1.0)
CheckBox.hoverColor = rgba(0.1, 0.1, 0.1, 1.0)
CheckBox.checkedColor = rgba(0.5, 0.9, 0.5, 1.0)
CheckBox.text = "CheckBox"
CheckBox.checked = false

function CheckBox:new(t)
    local o = setmetatable(t or {}, {__index=self})

    return o
end

function CheckBox:onClick()
    self.checked = not self.checked
end

function CheckBox:draw(x, y, width, height)
    if self.isHovered then
        api.renderSprite(x, y, width, height, self.hoverColor[1], self.hoverColor[2], self.hoverColor[3], self.hoverColor[4], self.roundRadius)
    else
        api.renderSprite(x, y, width, height, self.color[1], self.color[2], self.color[3], self.color[4], self.roundRadius)
    end

    if self.checked then
        api.renderSprite(x + (width*0.2), y + (height*0.2), width*0.6, height*0.6, self.checkedColor[1], self.checkedColor[2], self.checkedColor[3], self.checkedColor[4], self.roundRadius)
    end

    local textSize = font:getTextSize(self.text)

    drawText(self.text, x + width + textSize.x/2, y + height/2 - textSize.y/2, self.color[1], self.color[2], self.color[3], self.color[4])
end


TextBox = UIelement:new()
TextBox.editing = false
TextBox.content = ""
TextBox.cursorTimer = 0
TextBox.cursorBlink = true
TextBox.cursorPosition = 1

function TextBox:onClick()
    self.editing = true
end

function TextBox:onFocusLost()
    self.editing = false
end

function TextBox:draw(x, y, width, height)

    local margin = 2

    api.renderSprite(x - margin, y - margin, width + margin*2, height + margin*2, 0.9, 0.9, 0.9, 1, self.roundRadius)

    if self.editing then
        api.renderSprite(x, y, width, height, 0.0, 0.0, 0.0, 1, self.roundRadius)
    else
        api.renderSprite(x, y, width, height, 1.1, 0.1, 0.1, 1, self.roundRadius)
    end

    api.renderSprite(x, y, width, height, 0.1, 0.1, 0.1, 1, self.roundRadius)

    drawText(self.content, x, y, 1.0, 1.0, 1.0, 1.0)

    local textSize = font:getTextSize(self.content:sub(1, -self.cursorPosition))
    local textWidth = textSize.x
    local textHeight = textSize.y

    
    if self.editing then
        self.cursorTimer = self.cursorTimer + api.getDelta()

        if self.cursorTimer > 0.5 then
            self.cursorBlink = not self.cursorBlink
            self.cursorTimer = 0
        end

        if self.cursorBlink then
            api.renderSprite(x + textWidth, y, 2, textHeight, 1.0, 1.0, 1.0, 1.0, 0)
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


return gui