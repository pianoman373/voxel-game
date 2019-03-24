print("Hello from GUI!")

local gui = {}



api.registerTexture("base:textures/GUI.png")
api.registerFont("base:HelvetiPixel.ttf")


local font = api.getFont("base:HelvetiPixel.ttf")

gui.text = function(x, y, text, r, g, b, a)
    api.renderText(x+2, y-2, text, font, 0, 0, 0, a or 1)
    api.renderText(x, y, text, font, r or 1, g or 1, b or 1, a or 1)
end

gui.button = function(x, y, text)

end



return gui