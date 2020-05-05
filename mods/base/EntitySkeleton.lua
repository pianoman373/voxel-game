local Skeleton = {
    name = "Skeleton",
    walkTime = 40
}

function Skeleton:new()
    self.entity.width = 0.8
    self.entity.height = 1.8
    self.entity.depth = 0.8
end

function Skeleton:pickNewDirection()
    local direction = vec2(0, 0)

    direction.x = (math.random() * 2) - 1
    direction.y = (math.random() * 2) - 1

    direction = vecmath.normalize(direction) * 0.1
    
    if math.random() > 0.5 then
        self.entity.xVelocity = direction.x
        self.entity.zVelocity = direction.y
    else
        self.entity.xVelocity = 0
        self.entity.zVelocity = 0
    end

    self.entity.xDirection = direction.x
    self.entity.zDirection = direction.y

    self.walkTime = math.random(20, 100)
end

function Skeleton:tick()
    if self.walkTime == 0 then
        self:pickNewDirection()
    else
        self.walkTime = self.walkTime - 1
    end

    if self.entity.collidedHorizontal and self.entity.onGround then
        self.entity.yVelocity = 0.4
    end
end

return Skeleton