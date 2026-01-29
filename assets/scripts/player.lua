function Start(self)
    -- self.entity is the entity ID this script is attached to
    self.rb = ECS.GetRigidbody(self.entity)
    print("Player controller started for entity: " .. self.entity)
end

function Update(self)
    if Input.KeyHoldingDown(KEYBOARD.A) then
        self.rb.velocity.x = self.rb.velocity.x - 0.1
    end
    if Input.KeyHoldingDown(KEYBOARD.D) then
        self.rb.velocity.x = self.rb.velocity.x + 0.1
    end
    if Input.KeyHoldingDown(KEYBOARD.W) then
        self.rb.velocity.y = self.rb.velocity.y + 0.1
    end
    if Input.KeyHoldingDown(KEYBOARD.S) then
        self.rb.velocity.y = self.rb.velocity.y - 0.1
    end
    if Input.KeyJustPressed(KEYBOARD.SPACE) then
        Sound.Play("jump")
    end
end