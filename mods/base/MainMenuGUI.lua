local gui = require("base:gui.lua")

local MainMenuGUI = {}


function MainMenuGUI.new()
    local o = {}

    o.addressBox = TextBox:new("localhost:3737")

    setmetatable(o, {__index = MainMenuGUI})
    return o
end

local background = api.getTexture("base:resources/textures/background.png")

function MainMenuGUI:render(width, height)
    api.renderSpriteTexture(0, 0, width, height, 0, 0, 1, 1, background)

    self.addressBox:render(0, 150, 200, 30)

    gui.button(0, 300, 300, 50, "Singleplayer", function()
        api.input.setMouseGrabbed(true)
        api.connectToIntegratedServer()
    end)

    gui.button(0, 200, 300, 50, "Multiplayer", function()
        api.input.setMouseGrabbed(true)
        local t={}
        for str in string.gmatch(t1.content, "([^"..":".."]+)") do
            table.insert(t, str)
        end

        api.connectToServer(t[1], tonumber(t[2]))
    end)
end

return MainMenuGUI