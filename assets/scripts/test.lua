
function init()
	Graphics.LoadImage("player", "player.png")
	Sound.LoadSound("jump", "sounds/jump.wav")

	local player = ECS.CreateEntity()
	ECS.AddComponent(player, Transform(vec2(5, 5)))
	ECS.AddComponent(player, Rigidbody(vec2(5,5), vec2(0.1,0.1)))
	ECS.AddComponent(player, Sprite("player", 1, vec2(20,20)))
	ECS.AddComponent(player, Health(100))
end

function update()
	if(Input.KeyJustPressed(KEYBOARD.A)) then
	print("A")
	end
	if(Input.KeyJustPressed(KEYBOARD.D)) then
	Sound.Play("jump")
	end

	if(Input.KeyJustPressed(KEYBOARD.ESC)) then
	Shutdown()
	end
end
