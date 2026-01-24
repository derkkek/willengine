#pragma once

namespace willeditor
{
    enum PlayState
    {
        Stopped,
        Playing,
        Paused
    };

    struct EntityEditorState 
    {
        char entityID[128] = "";

        // Component flags (which components to include)
        bool hasTransform = true;
        bool hasRigidbody = false;
        bool hasSprite = false;
        bool hasBoxCollider = false;
        bool hasHealth = false;
        bool hasScript = false;

        // Transform
        float transformX = 0.0f;
        float transformY = 0.0f;

        // Rigidbody
        float rbPosX = 0.0f, rbPosY = 0.0f;
        float rbVelX = 0.0f, rbVelY = 0.0f;

        // Sprite
        char spriteID[128] = "";
        float spriteAlpha = 1.0f;
        float spriteWidth = 20.0f, spriteHeight = 20.0f;

        // BoxCollider
        float colliderWidth = 20.0f, colliderHeight = 20.0f;

        // Health
        float healthAmount = 100.0f;

        // Script
        char scriptName[256] = "";
    };

}