#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/exponential.hpp>
#include <cornjam/collision.h>
#include <cornjam/grid.h>
#include <cornjam/inputmanager.h>
#include <misc/globals.h>


class Entity {
    protected:
    glm::vec2 Direction;
    float DirectionRad;
    public:
    glm::vec3 Position;
    glm::vec2 Velocity;
    
    glm::vec2 ScaleLocal;
    glm::vec2 ScaleGlobal;

    void SetDirection(float directionRad) {
        DirectionRad = directionRad;
        Direction = glm::vec2(std::cos(directionRad), std::sin(directionRad));
    }

    void SetDirection(glm::vec2 direction) {
        Direction = direction;
        DirectionRad = glm::atan(direction.y, direction.x);
    }
};

class Player : public Entity {
    public:
    glm::vec2 Resistance;
    glm::vec2 Speed, TerminalSpeed;
    AABBHitbox Hitbox;
    int PlayerNum;

    Player(int playerNum, glm::vec3 position = glm::vec3(0.0), float directionRad = 0.0, glm::vec2 speed = glm::vec2(0.0), glm::vec2 velocity = glm::vec2(0.0), glm::vec2 terminalSpeed = glm::vec2(1.0), glm::vec2 resistance = glm::vec2(1.0), glm::vec2 scaleLocal = glm::vec2(1.0), glm::vec2 scaleGlobal = glm::vec2(1.0), glm::vec2 hitboxPosition = glm::vec2(0.0), glm::vec2 hitboxSize = glm::vec2(1.0)) : Hitbox(hitboxPosition + glm::vec2(position.x, position.y), hitboxSize) {
        Position = position;
        Resistance = resistance;
        Speed = speed;
        TerminalSpeed = terminalSpeed;
        DirectionRad = directionRad;
        Direction = glm::vec2(std::cos(DirectionRad), std::sin(DirectionRad));
        ScaleLocal = scaleLocal;
        ScaleGlobal = scaleGlobal;
        PlayerNum = playerNum;
    }

    void VeloUpdate() { //If anything is farther than searchRadius, discard it from collision testing.
        //First, clamp the velocities
        Velocity.x = glm::clamp(Velocity.x, -TerminalSpeed.x, TerminalSpeed.x);

        //X axis

        Position.x += Velocity.x * (float)(DeltaTime);
        Hitbox.Origin.x = Position.x;

        if (Position.x <= 0) Position.x = 0;
        if (Position.x >= WIDTH - ScaleLocal.x) Position.x = WIDTH - ScaleLocal.x;

        Hitbox.Origin.x = Position.x; //Update hitbox
        
        //Y axis

        Position.y += Velocity.y * (float)(DeltaTime);

        Hitbox.Origin.y = Position.y; //Update hitbox

        //Update velocities for next loop

        Velocity.x = Velocity.x * glm::pow(Resistance.x, DeltaTime * 60);
        Velocity.y = 0; //nuh uh 1.0 resistance
    }

    glm::mat4 GetTransformMatrix() {
        glm::mat4 Matrix(1.0f);

        Matrix = glm::translate(Matrix, glm::vec3(Position.x, Position.y, 0.0));
        Matrix = glm::scale(Matrix, glm::vec3(ScaleGlobal.x, ScaleGlobal.y, 1.0));
        Matrix = glm::rotate(Matrix, DirectionRad, glm::vec3(0.0, 0.0,1.0));
        Matrix = glm::scale(Matrix, glm::vec3(ScaleLocal.x, ScaleLocal.y, 1.0));
        
        return Matrix;
    }

    void KeyboardUpdate (InputManager &input) {
        if (PlayerNum == 1) {

            if (input.isActionPressed(Action::P1Left)) {
                Velocity.x += -Speed.x * (DeltaTime * 60);
            }

            if (input.isActionPressed(Action::P1Right)) {
                Velocity.x += Speed.x * (DeltaTime * 60);
            }
            
        }

        if (PlayerNum == 2) {

            if (input.isActionPressed(Action::P2Left)) {
                Velocity.x += -Speed.x * (DeltaTime * 60);
            }

            if (input.isActionPressed(Action::P2Right)) {
                Velocity.x += Speed.x * (DeltaTime * 60);
            }
            
        }

    }
};

class Ball : public Entity {
    public:
    glm::vec2 Resistance;
    glm::vec2 Speed, TerminalSpeed;
    int CollisionAxes[2] = {0, 0};
    AABBHitbox Hitbox;

    Ball(glm::vec3 position = glm::vec3(0.0), float directionRad = 0.0, glm::vec2 speed = glm::vec2(0.0), glm::vec2 velocity = glm::vec2(0.0), glm::vec2 terminalSpeed = glm::vec2(1.0), glm::vec2 resistance = glm::vec2(1.0), glm::vec2 scaleLocal = glm::vec2(1.0), glm::vec2 scaleGlobal = glm::vec2(1.0), glm::vec2 hitboxPosition = glm::vec2(0.0), glm::vec2 hitboxSize = glm::vec2(1.0)) : Hitbox(hitboxPosition + glm::vec2(position.x, position.y), hitboxSize) {
        Position = position;
        Resistance = resistance;
        Speed = speed;
        Velocity = velocity;
        TerminalSpeed = terminalSpeed;
        DirectionRad = directionRad;
        Direction = glm::vec2(std::cos(DirectionRad), std::sin(DirectionRad));
        ScaleLocal = scaleLocal;
        ScaleGlobal = scaleGlobal;
    }

    void VeloUpdate(AABBHitbox P1, AABBHitbox P2, glm::vec2 VeloP1, glm::vec2 VeloP2) { //If anything is farther than searchRadius, discard it from collision testing.
        //First, clamp the velocities

        //X axis

        CollisionAxes[0] = 0;
        Position.x += Velocity.x * (float)(DeltaTime);
        Hitbox.Origin.x = Position.x;

        if (Position.x <= 0) {Position.x = 0; CollisionAxes[0] = 1;}
        if (Position.x >= WIDTH - ScaleLocal.x) {Position.x = WIDTH - ScaleLocal.x; CollisionAxes[0] = 1;}

        if (Collision(Hitbox, P1))  {
            float Snapped = snapToSurfaceX(Hitbox, P1, Velocity.x, VeloP1.x);
            Position.x += Snapped;

            CollisionAxes[0] = 1;  
        }

        if (Collision(Hitbox, P2))  {
            float Snapped = snapToSurfaceX(Hitbox, P2, Velocity.x, VeloP2.x);
            Position.x += Snapped;

            CollisionAxes[0] = 1;  
        }

        Hitbox.Origin.x = Position.x; //Update hitbox
        
        //Y axis

        CollisionAxes[1] = 0;
        Position.y += Velocity.y * (float)(DeltaTime);
        Hitbox.Origin.y = Position.y;

        if (Position.y <= 0) {Position.y = 0; CollisionAxes[1] = 1;}
        if (Position.y >= HEIGHT - ScaleLocal.y) {Position.y = HEIGHT - ScaleLocal.y; CollisionAxes[1] = 1;}

        if (Collision(Hitbox, P1))  {
            float Snapped = snapToSurfaceY(Hitbox, P1, Velocity.y, VeloP1.y);
            Position.y += Snapped;

            CollisionAxes[1] = 1;  
        }

        if (Collision(Hitbox, P2))  {
            float Snapped = snapToSurfaceY(Hitbox, P2, Velocity.y, VeloP2.y);
            Position.y += Snapped;

            CollisionAxes[1] = 1;  
        }

        Hitbox.Origin.y = Position.y; //Update hitbox

        //Update velocities for next loop

        if (CollisionAxes[0]) Velocity.x *= -1;
        if (CollisionAxes[1]) Velocity.y *= -1;
    }

    glm::mat4 GetTransformMatrix() {
        glm::mat4 Matrix(1.0f);

        Matrix = glm::translate(Matrix, glm::vec3(Position.x, Position.y, 0.0));
        Matrix = glm::scale(Matrix, glm::vec3(ScaleGlobal.x, ScaleGlobal.y, 1.0));
        Matrix = glm::rotate(Matrix, DirectionRad, glm::vec3(0.0, 0.0,1.0));
        Matrix = glm::scale(Matrix, glm::vec3(ScaleLocal.x, ScaleLocal.y, 1.0));
        
        return Matrix;
    }

    void KeyboardUpdate (InputManager &input) {}
};