print("Hello from GUI!")

local gui = {}



api.getTexture("base:resources/textures/GUI.png")
local font = api.getFont("base:resources/HelvetiPixel.ttf")

local inputCache = ""
local inputOn = false

local function isMouseInBox(x, y, width, height)
    local curX, curY = api.input.getCursorPos()

    if curX > x and curX < x+width and curY > y and curY < y+height then
        return true
    end

    return false
end

local data = {}

api.registerEventHandler("key_press", function(key)
    if inputOn then

    end
end)

api.registerEventHandler("char_press", function(char)
    if inputOn then
        inputCache = inputCache .. char
        print(inputCache)
    end
end)


gui.text = function(text, x, y, r, g, b, a)
    api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)


end

gui.textCentered = function(text, x, y, r, g, b, a)
    textWidth, textHeight = font:getTextSize(text)

    x = x - (textWidth/2)
    y = y - (textHeight/2)

    api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

gui.button = function(text, x, y, width, height)
    if not data[text] then
        data[text] = {
            isButtonClicked = false
        }
    end

    local object = data[text]
    local ret = false

    if isMouseInBox(x, y, width, height) then

        if api.input.isMouseButtonDown(0) then
            if not object.isButtonClicked then
                object.isButtonClicked = true
            end

            api.renderSprite(x, y, width, height, 0.4, 0.4, 0.4, 1)
        else
            if object.isButtonClicked then
                object.isButtonClicked = false

                ret = true
            end


            api.renderSprite(x, y, width, height, 0.5, 0.5, 0.5, 1)
        end


    else
        if object.isButtonClicked then
            object.isButtonClicked = false
        end

        api.renderSprite(x, y, width, height, 0.6, 0.6, 0.6, 1)
    end

    gui.textCentered(text, x + (width/2), y + (height/2))

    return ret
end

gui.textBox = function(label, content, x, y, width)
    if not data[label] then
        data[label] = {
            isEditing = false,
            timer = 0.0,
            cursorBlink = false
        }
    end

    local object = data[label]

    textWidth, textHeight = font:getTextSize(label)

    local margin = 2

    if isMouseInBox(x + textWidth+10, y, width, textHeight) then

        if api.input.isMouseButtonDown(0) and not object.isEditing then
            object.isEditing = true
            inputOn = true
        end

    else
        if api.input.isMouseButtonDown(0) and object.isEditing then
            object.isEditing = false
            inputOn = false
        end
    end

    gui.text(label, x, y)



    api.renderSprite(x + textWidth+10 - margin, y - margin, width + margin*2, textHeight + margin*2, 0.9, 0.9, 0.9, 1)


    if object.isEditing then
        api.renderSprite(x + textWidth+10, y, width, textHeight, 0.1, 0.1, 0.1, 1)

        object.timer = object.timer + api.getDelta()

        if object.timer > 0.5 then
            object.cursorBlink = not object.cursorBlink
            object.timer = object.timer - 0.5
        end

        content = content .. inputCache
        inputCache = ""
    else
        api.renderSprite(x + textWidth+10, y, width, textHeight, 0.0, 0.0, 0.0, 1)
    end



    if object.cursorBlink then
        contentWidth, contentHeight = font:getTextSize(content)

        api.renderSprite(x + textWidth+10 + contentWidth, y + (contentHeight*0.1), 2, contentHeight - (contentHeight*0.2), 1, 1, 1, 1)
    else
        
    end

    gui.text(content, x + textWidth+10, y)


    return content
end



return gui