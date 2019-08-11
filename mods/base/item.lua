local item = {}

local registry = {}

local Item = {}

Item.__index = Item

Item.texture = nil
Item.name = "Unnamed"

function Item:draw(x, y)
    if self.texture then
        --api.renderBlockItem(2, x, y, 30)
        api.renderSpriteTexture(x - 30, y - 30, 60, 60, 0, 0, 1, 1, api.getTexture(self.texture))
    end
end

function Item:onUse(player, itemSlot)

end

local BlockItem = {}

BlockItem.__index = BlockItem

BlockItem.block = nil
BlockItem.name = "Unnamed"

function BlockItem:draw(x, y)
    api.renderBlockItem(self.block:getID(), x, y, 30)
end

function BlockItem:onUse(player, itemSlot)
    local blockPosition = vec3i()
    local blockNormal = vec3i()
    local hit = player.world:raycastBlocks(player.position + vec3(0, 0.7, 0), player.direction, 10, blockPosition, blockNormal)

    player.world:setBlock(blockPosition.x + blockNormal.x, blockPosition.y + blockNormal.y, blockPosition.z + blockNormal.z, self.block)

    itemSlot.count = itemSlot.count - 1
end


function item.register(id, item)
    setmetatable(item, Item)
    registry[id] = item
end

function item.registerBlockItem(id)
    local o = {}

    o.block = api.getBlock(id)
    o.name = o.block.table.name

    setmetatable(o, BlockItem)

    registry[id] = o
end

function item.get(id)
    return registry[id]
end


return item