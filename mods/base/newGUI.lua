
local newGUI = {}

local function getDimensions(constraints, panelWidth, panelHeight)
    local width = 128
    local height = 128
    local x = 0
    local y = 0

    x = panelWidth * constraints.x
    y = panelHeight * constraints.y
    width = panelWidth * constraints.width
    height = panelHeight * constraints.height

    return x, y, width, height
end

local panel = {}

panel.__index = panel


-- user friendly render function
function panel:render(panelWidth, panelHeight)
    for _, child in ipairs(self.children) do
        x, y, width, height = getDimensions(child.constraints, panelWidth, panelHeight)

        child.type.render(x, y, width, height, child.data)
    end
end

function panel:addChild(childType, constraints, data)
    table.insert(self.children, {type=childType, constraints=constraints, data=data})
end

function panel:addPanel(constraints)
    
end



function newGUI.panel()
    return setmetatable({children = {}}, panel)
end


newGUI.sprite = {}

function newGUI.sprite.render(x, y, width, height, data)
    api.renderSprite(x - (width/2), y - (width/2), width, height, data.color.r, data.color.g, data.color.b, data.color.a)
end

return newGUI