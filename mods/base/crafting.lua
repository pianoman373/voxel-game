local table = require("table")

local crafting = {}

local recipes = {}

function crafting.addRecipe(group, recipe)
    if recipes[group] == nil then
        recipes[group] = {}
    end

    table.insert(recipes[group], recipe)
end

function crafting.getRecipes(group)
    return recipes[group]
end

return crafting