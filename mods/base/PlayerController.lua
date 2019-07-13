local math = require("math")
local Inventory = require("base:Inventory.lua")

local movementSpeed = 7.0
local xRot = 0
local yRot = 0
local lastpos = vec2(0, 0)
local mouseSensitivity = 8.0
local animationCooldown = 0.0
local blockOutlineColor = vec3(0, 0, 0)
local lastGrabbed = false


local PlayerController = {}

PlayerController.inventory = Inventory.new(12*3)
PlayerController.selectedSlot = 1

PlayerController.inventory:addItems(api.getBlock("base:cobblestone"), 16)
PlayerController.inventory:addItems(api.getBlock("base:dirt"), 2)
PlayerController.inventory:addItems(api.getBlock("base:grass"), 3)
PlayerController.inventory:addItems(api.getBlock("base:cobblestone"), 3)

function PlayerController.update(player, delta)
    player.velocity.y = player.velocity.y - 22.9 * delta
    
    if api.input.isMouseGrabbed() then
        if not lastGrabbed then
            lastpos = api.input.getCursorPos()
        end

        local offset = api.input.getCursorPos() - lastpos

        local xOffset = -offset.x / mouseSensitivity
        local yOffset = offset.y / mouseSensitivity

        yRot = yRot + (yOffset)
        xRot = xRot + (xOffset)

        if yRot > 89.9 then
            yRot = 89.9
        end
        if yRot < -89.9 then
            yRot = -89.9
        end

        player.direction.x = math.sin(math.rad(xRot)) * math.cos(math.rad(yRot))
        player.direction.z = math.cos(math.rad(xRot)) * math.cos(math.rad(yRot))
        player.direction.y = math.sin(math.rad(yRot))

        lastpos = api.input.getCursorPos();

        player.direction = vecmath.normalize(player.direction)
        local right = vecmath.cross(player.direction, vec3(0, 1, 0))

        local multiplier = 1

        if api.input.isKeyDown(api.input.KEY_LEFT_SHIFT) then
            multiplier = 10
        end

        if api.input.isKeyDown(api.input.KEY_SPACE) then
            player.velocity.y = 10
        end

        if api.input.isKeyDown(api.input.KEY_W) then
            local dir = vec3(player.direction)
            dir.y = 0
            dir = vecmath.normalize(dir)
            player.velocity = player.velocity + (dir * movementSpeed * multiplier)
        end
        if api.input.isKeyDown(api.input.KEY_S) then
            local dir = vec3(player.direction)
            dir.y = 0
            dir = vecmath.normalize(dir)
            player.velocity = player.velocity - (dir * movementSpeed * multiplier)
        end
        if api.input.isKeyDown(api.input.KEY_A) then
            local dir = vec3(right)
            dir.y = 0
            dir = vecmath.normalize(dir)
            player.velocity = player.velocity - (dir * movementSpeed * multiplier)
        end
        if api.input.isKeyDown(api.input.KEY_D) then
            local dir = vec3(right)
            dir.y = 0
            dir = vecmath.normalize(dir)
            player.velocity = player.velocity + (dir * movementSpeed * multiplier)
        end

        animationCooldown = animationCooldown - delta

        if animationCooldown < 0 then
            animationCooldown = 0
        end

        local blockPosition = vec3i()
        local blockNormal = vec3i()
        local hit = player.world:raycastBlocks(player.position + vec3(0, 0.7, 0), player.direction, 10, blockPosition, blockNormal)

        api.renderDebugAABB(vec3(blockPosition.x, blockPosition.y, blockPosition.z), vec3(blockPosition.x + 1, blockPosition.y + 1, blockPosition.z + 1), blockOutlineColor)

        if api.input.isMouseButtonDown(1) and animationCooldown == 0 and hit then
            local slot = PlayerController.inventory:getSlot(PlayerController.selectedSlot)

            if slot.item ~= nil and slot.count > 0 then
                slot.count = slot.count - 1

                player.world:setBlock(blockPosition.x + blockNormal.x, blockPosition.y + blockNormal.y, blockPosition.z + blockNormal.z, slot.item)
                animationCooldown = 0.2
            end

            
        end
        if api.input.isMouseButtonDown(0) and animationCooldown == 0 and hit then
            local block = player.world:getBlock(blockPosition.x, blockPosition.y, blockPosition.z)

            PlayerController.inventory:addItems(block, 1)

            player.world:breakBlock(blockPosition.x, blockPosition.y, blockPosition.z)
            animationCooldown = 0.2
        end
    end

    lastGrabbed = api.input.isMouseGrabbed()
end

return PlayerController