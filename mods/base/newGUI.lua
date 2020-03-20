
local newGUI = {}

local function getDimensions(constraints, panelX, panelY, panelWidth, panelHeight)
    local width = 128
    local height = 128
    local x = 0
    local y = 0

    x = (panelX - (panelWidth / 2)) + (panelWidth * constraints.x)
    y = (panelY - (panelHeight / 2)) + (panelHeight * constraints.y)
    width = panelWidth * constraints.width
    height = panelHeight * constraints.height

    return x, y, width, height
end

local panel = {}

panel.__index = panel


-- user friendly render function
function panel:renderToScreen(panelWidth, panelHeight)
    for _, child in ipairs(self.children) do
        cx, cy, cwidth, cheight = getDimensions(child.constraints, panelWidth / 2, panelHeight / 2, panelWidth, panelHeight)

        child.type.render(cx, cy, cwidth, cheight, child.data)
    end
end

-- internal render function
function panel.render(x, y, width, height, data)
    for _, child in ipairs(data.children) do
        cx, cy, cwidth, cheight = getDimensions(child.constraints, x, y, width, height)

        child.type.render(cx, cy, cwidth, cheight, child.data)
    end
end

function panel:addChild(childType, constraints, data)
    table.insert(self.children, {type=childType, constraints=constraints, data=data})
end

function panel:addPanel(constraints)
    local panelInstance = setmetatable({children = {}}, panel)

    table.insert(self.children, {type=panel, constraints=constraints, data=panelInstance})

    return panelInstance
end



function newGUI.panel()
    return setmetatable({children = {}}, panel)
end


newGUI.sprite = {}

function newGUI.sprite.render(x, y, width, height, data)
    api.renderSprite(x - (width/2), y - (height/2), width, height, data.color.r, data.color.g, data.color.b, data.color.a)
end

newGUI.button = {}

local font = api.getFont("base:resources/HelvetiPixel.ttf")

function newGUI.drawText(text, x, y, r, g, b, a)
    api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

function newGUI.drawTextCentered(text, x, y, r, g, b, a)
    local textSize = font:getTextSize(text)

    x = x - (textSize.x/2)
    y = y - (textSize.y/2)

    api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

function newGUI.button.render(x, y, width, height, data)
    api.renderSprite(x - (width/2), y - (height/2), width, height, data.color.r, data.color.g, data.color.b, data.color.a)

    newGUI.drawTextCentered(data.text, x, y, 1.0, 1.0, 1.0, 1.0)
end

return newGUI