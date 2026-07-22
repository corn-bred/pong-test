#include "scenes.h"
#include "game.h"          
#include <cornjam/inputmanager.h>  
#include <misc/globals.h>

//PlayingScene

void PlayingScene::Init() {
    P1 = new Player(1, glm::vec3(WIDTH / 2 - 100, 30, 0.0), 0.0, glm::vec2(1000.0, 0.0), glm::vec2(0.0), glm::vec2(1000.0, 0.0), glm::vec2(0.7), glm::vec2(100, 100), glm::vec2(1.0), glm::vec2(0.0), glm::vec2(100, 100));
    P2 = new Player(2, glm::vec3(WIDTH / 2 - 100, HEIGHT - 10 - 30, 0.0), 0.0, glm::vec2(1000.0, 0.0), glm::vec2(0.0), glm::vec2(1000.0, 0.0), glm::vec2(0.7), glm::vec2(100, 10), glm::vec2(1.0), glm::vec2(0.0), glm::vec2(100, 10));
    
    mainVBO = new VertexBuffer(quadData, sizeof(quadData), GL_STATIC_DRAW);
    mainVBO->addAttribute(0, 2, GL_FLOAT, 4, 0);
    mainVBO->addAttribute(1, 2, GL_FLOAT, 4, 2);

    mainShader = new Shader("src/shaders/main.vert", "src/shaders/main.frag");

    ball = new Ball(glm::vec3(WIDTH / 2, HEIGHT / 2 + 63, 0.0), 0.0, glm::vec2(50.0), glm::vec2(300.0), glm::vec2(50.0), glm::vec2(1.0), glm::vec2(10), glm::vec2(1.0), glm::vec2(0.0), glm::vec2(10));
}

void PlayingScene::Update() {
    P1->KeyboardUpdate(input);
    P2->KeyboardUpdate(input);

    P1->VeloUpdate();
    P2->VeloUpdate();
    ball->VeloUpdate(P1->Hitbox, P2->Hitbox, P1->Velocity, P2->Velocity);
}

void PlayingScene::Render() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Player 1

    mainShader->use();
    
    glm::mat4 P1Model = P1->GetTransformMatrix();
    mainShader->setMat4("model", P1Model);
    
    glm::mat4 View = glm::mat4(1.0);
    mainShader->setMat4("view", View);
    
    mainShader->setMat4("projection", Projection);
    
    mainShader->setBool("isSolidColour", true);
    mainShader->setVec3("Colour", glm::vec3(1.0));

    mainVBO->bind();
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //Player 2
    
    glm::mat4 P2Model = P2->GetTransformMatrix();
    mainShader->setMat4("model", P2Model);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);

    //Ball

    glm::mat4 BallModel = ball->GetTransformMatrix();
    mainShader->setMat4("model", BallModel);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void PlayingScene::Exit() {
    delete P1;
    delete P2;
    delete mainShader;
    delete mainVBO;
    delete ball;
    P1 = nullptr;
    P2 = nullptr;
    mainShader = nullptr;
    mainVBO = nullptr;
    ball = nullptr;
}