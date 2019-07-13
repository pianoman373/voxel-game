local Inventory = {}
    
function Inventory.new(slots)
    local o = {}

    o.slotCount = slots
    o.contents = {}

    for i = 1,slots do
        o.contents[i] = {item = nil, count = 0}
    end
    
    setmetatable(o, {__index = Inventory})
    return o
end

function Inventory:setSlot(slotID, item, count)
    self.contents[slotID].item = item
    self.contents[slotID].count = count
end

function Inventory:getSlot(slotID)
    return self.contents[slotID]
end

function Inventory:addItems(item, count)
    for i = 1,self.slotCount do
        local slot = self.contents[i]

        if slot.item ~= nil then
            if slot.item:getID() == item:getID() and slot.count > 0 then
                slot.count = slot.count + count
                return
            end
        end
        
    end

    --fall back to setting the first nil slot
    for i = 1,self.slotCount do
        local slot = self.contents[i]

        if slot.item == nil or slot.count < 1 then
            slot.item = item
            slot.count = count
            return
        end
    end
end

function Inventory:takeItems(item, count)
    
    for i = 1,self.slotCount do
        local slot = self.contents[i]

        if slot.item ~= nil then
            if slot.item:getID() == item:getID() and slot.count > 0 then
                if slot.count < count then
                    
                elseif slot.count >= count then
                    slot.count = slot.count - count
                end

                return true
            end
        end
    end

    return false
end

function Inventory:isSlotEmpty(slotID)
    local slot = self.contents[slotID]

    if slot.count > 0 and slot.item then
        return false
    else
        return true
    end
end

function Inventory:printContents()
    for i = 1,self.slotCount do
        local slot = o.contents[i]

        if slot.count > 0 and slot.item then
            print("["..slot.item.." x"..slot.count.."]")
        else
            print("[empty]")
        end
    end
end

return Inventory