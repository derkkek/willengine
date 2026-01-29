Scene = {
    entities = {
        {
            id = {entityID = "enemy3"},

            components = {
                transform = {x = -20 , y = 75},
                rigidbody = {x = -20, y = 75, x_vel = 0, y_vel = 0},
                sprite = {sprite_id = "enemy_ship", sprite_alpha = 1, width = 10, height = 10},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 100},
                script = {name = "enemy"}
            }
        },
        {
            id = {entityID = "player"},

            components = {
                transform = {x = 0 , y = 0},
                rigidbody = {x = 0, y = 0, x_vel = 0, y_vel = 0},
                sprite = {sprite_id = "player_ship", sprite_alpha = 1, width = 10, height = 10},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 259},
                script = {name = "player"}
            }
        },
        {
            id = {entityID = "enemy0"},

            components = {
                transform = {x = 0 , y = 75},
                rigidbody = {x = 0, y = 75, x_vel = 0, y_vel = 0},
                sprite = {sprite_id = "enemy_ship", sprite_alpha = 1, width = 10, height = 10},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 100},
                script = {name = "enemy"}
            }
        },
        {
            id = {entityID = "enemy1"},

            components = {
                transform = {x = 20 , y = 75},
                rigidbody = {x = 20, y = 75, x_vel = 0, y_vel = 0},
                sprite = {sprite_id = "enemy_ship", sprite_alpha = 1, width = 10, height = 10},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 100},
                script = {name = "enemy"}
            }
        },
        {
            id = {entityID = "enemy2"},

            components = {
                transform = {x = 40 , y = 75},
                rigidbody = {x = 40, y = 75, x_vel = 0, y_vel = 0},
                sprite = {sprite_id = "enemy_ship", sprite_alpha = 1, width = 10, height = 10},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 100},
                script = {name = "enemy"}
            }
        },
        {
            id = {entityID = "enemy4"},

            components = {
                transform = {x = -40 , y = 75},
                rigidbody = {x = -40, y = 75, x_vel = 0, y_vel = 0},
                sprite = {sprite_id = "enemy_ship", sprite_alpha = 1, width = 10, height = 10},
                box_collider = {width = 20, height = 20, isCollided = false},
                health = {amount = 100},
                script = {name = "enemy"}
            }
        }

    }
}