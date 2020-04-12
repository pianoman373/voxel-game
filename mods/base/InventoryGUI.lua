local gui = require("base:gui.lua")
local crafting = require("base:crafting.lua")
local item = require("base:item.lua")

crafting.addRecipe("base:inventory", {
    input = {item.get("base:wood"), 1},
    output = {item.get("base:planks"), 8},
})

crafting.addRecipe("base:inventory", {
    input = {item.get("base:planks"), 1},
    output = {item.get("base:stick"), 4},
})

local InventoryGUI = {}



local inventoryTexture = api.getTexture("base:resources/textures/inventory.png")


function InventoryGUI.new(inventory)
    -- local o = {}

    -- o.inventory = inventory

    -- o.itemSlots = {}

    -- for i = 1,12*3 do
    --     o.itemSlots[i] = ItemSlot:new(inventory:getSlot(i))
    -- end

    -- setmetatable(o, {__index = InventoryGUI})
    -- return o
end

local selectedRecipe = nil

local function renderRecipeButton(x, y, recipe)
    
    -- if api.input.isMouseButtonPressed(0) then
    --     if gui.isMouseInBox(x, y, 77*4, 16*4) then
    --         selectedRecipe = recipe
    --     end
    -- end

    -- if gui.isMouseInBox(x, y, 77*4, 16*4) then
    --     api.renderSprite(x, y, 77*4, 16*4, 1, 1, 1, 0.3)
    -- else
    --     api.renderSprite(x, y, 77*4, 16*4, 1, 1, 1, 0.1)
    -- end

    -- recipe.output[1]:draw(x + 32, y + 32)

    -- gui.drawText(recipe.output[1].name, x + 64, y + 20, 1, 1, 1, 1)
    
end

function InventoryGUI:render(width, height)
    -- local x = (width/2) - 207*2
    -- local y = (height/2) - 207*2

    -- api.renderSpriteTexture(x, y, 207*4, 207*4, 0, 0, 1, 1, inventoryTexture)

    -- for i = 1,12 do
    --     self.itemSlots[i]:render(x + 3*4 + (i-1)*17*4, y + 19*4, 14*4, 14*4)
    -- end

    -- for i = 1,12 do
    --     self.itemSlots[i+12]:render(x + 3*4 + (i-1)*17*4, y + 45*4, 14*4, 14*4)
    -- end

    -- for i = 1,12 do
    --     self.itemSlots[i+24]:render(x + 3*4 + (i-1)*17*4, y + 62*4, 14*4, 14*4)
    -- end
    


    -- local i = 0
    -- for index, recipe in pairs(crafting.getRecipes("base:inventory")) do
    --     renderRecipeButton(x + 3*4, y + 171*4 - (i*17*4), recipe)

    --     i = i + 1
    -- end

    
    

    -- if selectedRecipe ~= nil then
    --     gui.button(x + 135*4, y + 90*4, 150, 50, "Craft", function()
    --         if self.inventory:takeItems(selectedRecipe.input[1], selectedRecipe.input[2]) then
    --             self.inventory:addItems(selectedRecipe.output[1], selectedRecipe.output[2])
    --         end
    --     end)

    --     --api.renderBlockItem(selectedRecipe.output[1]:getID(), x + 115*4, y + 180*4, 30)
    --     gui.drawText(selectedRecipe.output[1].name .. "  x" .. selectedRecipe.output[2], x + 122*4, y + 177*4, 1, 1, 1, 1)

    --     gui.drawTextCentered("Ingredients", x + 125*4, y + 167*4, 1, 1, 1, 1)

    --     --api.renderBlockItem(selectedRecipe.input[1]:getID(), x + 115*4, y + 155*4, 30)
    --     gui.drawText(selectedRecipe.input[1].name .. "  x" .. selectedRecipe.input[2], x + 122*4, y + 152*4, 1, 1, 1, 1)
        
    -- end
    
end




return InventoryGUI