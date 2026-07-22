#include <cornjam/collision.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

AABBHitbox::AABBHitbox(glm::vec2 origin = glm::vec2(0.0), glm::vec2 scale = glm::vec2(0.0)) : Origin(origin), Scale(scale) {}

bool Collision(AABBHitbox &a, AABBHitbox &b) {
    glm::vec2 aMin = a.Origin; glm::vec2 aMax= a.Origin + a.Scale;
    glm::vec2 bMin = b.Origin; glm::vec2 bMax= b.Origin + b.Scale;
    return ((aMin.x < bMax.x && aMax.x > bMin.x) &&
           (aMin.y < bMax.y && aMax.y > bMin.y));
}

float snapToSurfaceX(AABBHitbox &a, AABBHitbox &b, float xVelocityA, float xVelocityB) {
    glm::vec2 aMin = a.Origin; glm::vec2 aMax = a.Origin + a.Scale;
    glm::vec2 bMin = b.Origin; glm::vec2 bMax = b.Origin + b.Scale;
    //a>0 & b<0 = <
    //a>0 & b>0 & a>b = <
    //a>0 & b>0 & b>a = >
    //a<0 & b>0 = >
    //a<0 & b<0 & a<b = >
    //a<0 & b<0 & b<a = <
    if (xVelocityA > 0 && xVelocityB <= 0) {
        return (bMin.x - aMax.x);
    } else if (xVelocityA < 0 && xVelocityB >= 0) {
        return (bMax.x - aMin.x);
    } else if (xVelocityA > 0 && xVelocityB > 0) {
        if (xVelocityA > xVelocityB) {
            return (bMin.x - aMax.x);
        } else if (xVelocityA < xVelocityB) {
            return (bMax.x - aMin.x);
        } else return 0;
    } else if (xVelocityA < 0 && xVelocityB < 0) {
        if (xVelocityA < xVelocityB) {
            return (bMax.x - aMin.x);
        } else if (xVelocityA > xVelocityB){
            return (bMin.x - aMax.x);
        } else return 0;
    } else return 0;
}

float snapToSurfaceY(AABBHitbox &a, AABBHitbox &b, float yVelocityA, float yVelocityB) {
    glm::vec2 aMin = a.Origin; glm::vec2 aMax= a.Origin + a.Scale;
    glm::vec2 bMin = b.Origin; glm::vec2 bMax= b.Origin + b.Scale;
    if (yVelocityA > 0 && yVelocityB < 0) {
        return (bMin.y - aMax.y);
    } else if (yVelocityA < 0 && yVelocityB > 0) {
        return (bMax.y - aMin.y);
    } else if (yVelocityA > 0 && yVelocityB > 0) {
        if (yVelocityA > yVelocityB) {
            return (bMin.y - aMax.y);
        } else if (yVelocityA < yVelocityB) {
            return (bMax.y - aMin.y);
        } else return 0;
    } else if (yVelocityA < 0 && yVelocityB < 0) {
        if (yVelocityA < yVelocityB) {
            return (bMax.y - aMin.y);
        } else if (yVelocityA > yVelocityB) {
            return (bMin.y - aMax.y);
        } else return 0;
    } else return 0;
}