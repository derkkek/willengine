
function start()
	Graphics.LoadImage("player", "player.png")
	Sound.LoadSound("jump", "sounds/jump.wav")

	player = ECS.CreateEntity()
	ECS.AddComponent(player, Transform(vec2(5, 5)))
	ECS.AddComponent(player, Rigidbody(vec2(5,5), vec2(0.1,0.1)))
	ECS.AddComponent(player, Sprite("player", 1, vec2(20,20)))
	ECS.AddComponent(player, Health(100))

	rb = ECS.GetRigidbody(player)
end

function update()
	if(Input.KeyHoldingDown(KEYBOARD.A)) then
	rb.velocity.x = rb.velocity.x - 0.1
	end
	if(Input.KeyHoldingDown(KEYBOARD.D)) then
	rb.velocity.x = rb.velocity.x + 0.1
	end
	
	if(Input.KeyHoldingDown(KEYBOARD.W)) then
	rb.velocity.y = rb.velocity.y + 0.1
	end
	
	if(Input.KeyHoldingDown(KEYBOARD.S)) then
	rb.velocity.y = rb.velocity.y - 0.1
	end

	if(Input.KeyJustPressed(KEYBOARD.SPACE)) then
	rb.velocity.y = rb.velocity.y + 0.3
	Sound.Play("jump")
	end

	if(Input.KeyJustPressed(KEYBOARD.ESC)) then
	Shutdown()
	end
end
