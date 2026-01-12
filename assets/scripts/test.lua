
function init()
	Graphics.LoadImage("player", "player.png")
end

function update()
	if(Input.KeyIsDown(KEYBOARD.A)) then
	print("A")
	end

	if(Input.KeyIsDown(KEYBOARD.ESC)) then
	Shutdown()
	end
end
