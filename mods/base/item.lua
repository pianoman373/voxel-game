local item = {}

local registry = {}

function item.register(id, item)
    registry[id] = item
end

function item.get(id)
    return registry[id]
end


item.register("base:block", {
    draw = function(itemdata, x, y)
        api.renderBlockItem(itemdata.block:getID(), x, y, 30)
    end
})


return item