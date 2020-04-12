local gui = require("base:gui.lua");

local MainMenuGUI = {}


local background = api.getTexture("base:resources/textures/background.png")

local singleplayerPanel = {}
local optionsPanel = {}


--===================MAIN PANEL=======================--

local mainPanel = Panel:new({
    x = percent(0),
    y = percent(0),
    width = percent(100),
    height = percent(100),
    origin = 'bl',
    color = rgba(0.1, 0.1, 0.1, 1),
    image = background,
})


local sidePanel = mainPanel:addChild(Panel:new({
    x = percent(5),
    y = percent(50),
    width = pixels(300),
    height = pixels(500),
    origin = 'l',
    color = rgba(0.3, 0.3, 0.3, 1),
    roundRadius = 8,
}))

local topLabel = mainPanel:addChild(Label:new({
    x = percent(0),
    y = percent(2),
    text = "Cube Quest",
    origin = 't',
    anchor = 't',
}))

local button1 = sidePanel:addChild(Button:new({
    x = percent(0),
    y = percent((1 / 5) * 100 * 1),
    width = percent(80),
    height = pixels(64),
    origin = '',
    anchor = 't',
    color = rgba(0.5, 0.9, 0.5, 1.0),
    hoverColor = rgba(0.6, 1.0, 0.6, 1.0),
    roundRadius = 8,
    text = "Singleplayer",
    onClick = function() 
        singleplayerPanel.active = true
        optionsPanel.active = false
        fillWorldList()
    end,
}))

local button2 = sidePanel:addChild(Button:new({
    x = percent(0),
    y = percent((1 / 5) * 100 * 2),
    width = percent(80),
    height = pixels(64),
    origin = '',
    anchor = 't',
    color = rgba(0.5, 0.9, 0.5, 1.0),
    hoverColor = rgba(0.6, 1.0, 0.6, 1.0),
    roundRadius = 8,
    text = "Multiplayer",
}))

local button3 = sidePanel:addChild(Button:new({
    x = percent(0),
    y = percent((1 / 5) * 100 * 3),
    width = percent(80),
    height = pixels(64),
    origin = '',
    anchor = 't',
    color = rgba(0.5, 0.9, 0.5, 1.0),
    hoverColor = rgba(0.6, 1.0, 0.6, 1.0),
    roundRadius = 8,
    text = "Options",
    onClick = function() 
        singleplayerPanel.active = false
        optionsPanel.active = true
    end,
}))

local button4 = sidePanel:addChild(Button:new({
    x = percent(0),
    y = percent((1 / 5) * 100 * 4),
    width = percent(80),
    height = pixels(64),
    origin = '',
    anchor = 't',
    color = rgba(0.5, 0.9, 0.5, 1.0),
    hoverColor = rgba(0.6, 1.0, 0.6, 1.0),
    roundRadius = 8,
    text = "Quit",
}))




--=====================================SINGLEPLAYER_PANEL=================================--

singleplayerPanel = mainPanel:addChild(Panel:new({
    x = percent(10),
    y = percent(0),
    width = percent(50),
    height = pixels(500),
    origin = 'r',
    anchor = 'r',
    color = rgba(0.3, 0.3, 0.3, 1),
    roundRadius = 8,
    active = false
}))

local createWorldName = singleplayerPanel:addChild(TextBox:new({
    x = percent((1 / 3) * 100 * 0 + 2),
    y = percent(5),
    width = percent((100.0 / 3.0)*2 - 4),
    height = pixels(64),
    origin = 'bl',
    anchor = 'bl',
    content = "New World",
}))

local createWorldButton = singleplayerPanel:addChild(Button:new({
    x = percent((1 / 3) * 100 * 2 + 2),
    y = percent(5),
    width = percent(100.0 / 3.0 - 4),
    height = pixels(64),
    origin = 'bl',
    anchor = 'bl',
    text = "Create World",
    color = rgba(0.5, 0.9, 0.5, 1.0),
    hoverColor = rgba(0.6, 1.0, 0.6, 1.0),
    onClick = function()
        local world = WorldGenerator.new("test", 4)
        world:generate()
    end,
}))

local worldListPanel = singleplayerPanel:addChild(Panel:new({
    x = percent(5),
    y = pixels(32),
    width = percent(90),
    height = pixels(350),
    origin = 'tl',
    anchor = 'tl',
    color = rgba(0.2, 0.2, 0.2, 1),
    roundRadius = 8,
}))

--===============================================OPTIONS_PANEL==========================================--

optionsPanel = mainPanel:addChild(Panel:new({
    x = percent(10),
    y = percent(0),
    width = percent(50),
    height = pixels(500),
    origin = 'r',
    anchor = 'r',
    color = rgba(0.3, 0.3, 0.3, 1),
    roundRadius = 8,
    active = false
}))

local checkbox1 = optionsPanel:addChild(CheckBox:new({
    x = percent(10),
    y = percent(10),
    width = pixels(32),
    height = pixels(32),
    origin = 'tl',
    anchor = 'tl',
    roundRadius = 8,
    text = "Shadows"
}))

function fillWorldList()
    worldListPanel.children = {}
    local dirList = api.getSaveList()

    local i = 0
    for _, filename in ipairs(dirList)  do
        print(filename)

        local loadWorldButton = worldListPanel:addChild(Button:new({
            x = percent(1),
            y = pixels(70*i + 8),
            width = percent(70.0),
            height = pixels(64),
            origin = 'tl',
            anchor = 'tl',
            text = filename,
            color = rgba(0.5, 0.9, 0.5, 1.0),
            hoverColor = rgba(0.6, 1.0, 0.6, 1.0),
            onClick = function()
                api.connectToIntegratedServer(filename)
            end,
        }))

        i = i + 1
    end
end


function MainMenuGUI.render(width, height)
    mainPanel:render(width, height)
end

return MainMenuGUI