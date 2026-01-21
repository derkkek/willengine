
function start()
	--Graphics.LoadImage("player", "player.png")
	--Sound.LoadSound("jump", "sounds/jump.wav")

	player = ECS.CreateEntity()
	ECS.AddComponent(player, Transform(vec2(5, 5)))
	ECS.AddComponent(player, Rigidbody(vec2(5,5), vec2(0.1,0.1)))
	ECS.AddComponent(player, Sprite("player", 1, vec2(20,20)))
	ECS.AddComponent(player, BoxCollider(vec2(20,20), false))
	--ECS.AddComponent(player, Health(100))

	--print(ECS.HasBoxCollider(player))
	--rb = ECS.GetRigidbody(player)
end

function update()

end
