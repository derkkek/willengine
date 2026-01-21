Scene ={
    entities ={
        [0] =
        {
            id = {entityID = "player"},

            components = {
                transform = {x = 5 , y = 5},
                rigidbody = {x = 5, y = 5, x_vel = 0.1, y_vel = 0.1},
                sprite = {sprite_id = "player", sprite_alpha = 1, width = 20, height = 20},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 100}
            }
        },
        {
            id = {entityID = "enemy"},
            
            components = {
                transform = {x = 20 , y = 5},
                rigidbody = {x = 20, y = 5, x_vel = 0.0, y_vel = 0.0},
                sprite = {sprite_id = "enemy", sprite_alpha = 1, width = 20, height = 20},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 100}
            }
        }

    }
}