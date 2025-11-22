#include "PhysicsSystem.h"
#include "../engineinit.h"
#include "../core.h"
#include "string.h"
namespace gea
{
float frictionCooefficient = 0.25;


void gea::PhysicsSystem::update(float deltaTime)
{

    gea::terrainInfo terrainInfo;
    if(!terrainMesh)
    {
        for (auto &[entityID,component] : gea::EngineInit::registry.Transforms)
        {
            if(gea::EngineInit::registry.Transforms[entityID].name == "Terrain")
            {
                //get the mesh of the terrain
                terrainMesh = &gea::EngineInit::registry.Meshes[entityID];
                continue;
            }
        }
    }

    for(auto &[entityID,component] : gea::EngineInit::registry.Physics)
    {

        // find barrysentric coordinates of this entity

            terrainInfo = ComputeBarrysentricCoord(gea::EngineInit::registry.Transforms[entityID].mPosition,terrainMesh);

        //friction cooefficient changes at a certain point to simulate change in friction
        if(terrainInfo.pos <= -12)
            frictionCooefficient = 1;

        //Update physics variables
        glm::vec3 oldPosition = gea::EngineInit::registry.Transforms[entityID].mPosition;

        component.mAccelerationVector     = computeAccelerationVector(terrainInfo.normal) ;
        component.mVelocityVector        += component.mAccelerationVector * deltaTime;
        component.mRollingDirectionVector = computeRotationVector(terrainInfo.normal,component .mVelocityVector);
        component.mBarrySentricCoord      = terrainInfo.pos;



        if (terrainInfo.pos == 0.0f && glm::length(component.mVelocityVector) < 0.001f) // if we're outside the terrain or moving slow enough, shut it down.
            component.mVelocityVector = glm::vec3(0);


        //this raidus, with every other radius in the scene, except terrain
        for(auto &[entityID2,component2] : gea::EngineInit::registry.Transforms)
        {
            //avoid self and avoid terrain
            if(component2.name == "Terrain")
                continue;
            if(gea::EngineInit::registry.Transforms[entityID].mPosition == component2.mPosition)
                continue;

            if(sphereCollisionDetection(
                                        gea::EngineInit::registry.Transforms[entityID].mPosition,
                                        component2.mPosition,
                                        gea::EngineInit::registry.SphereCollision[entityID].radius,
                                        gea::EngineInit::registry.SphereCollision[entityID2].radius))
            {
                //Project collision normal?
                glm::vec3 collisionNormal = glm::normalize(component2.mPosition - gea::EngineInit::registry.Transforms[entityID].mPosition);
                component.mVelocityVector = component.mVelocityVector - 2.0f * glm::dot(component.mVelocityVector, collisionNormal) * collisionNormal;
            }
        }


        //Doing both changes in physics and transform simultaniously as we dont have a system in place, and making one is unnecessary use of time
        //Note: for Gameengine, seperate the above code with the one below

        //use physics variables to update sphere rotation and position, making it roll on the surface
        //Update its velocity only within the domain of the terrain

        //Change in position
        glm::vec3 frictionForce = computeFriction(frictionCooefficient, terrainInfo.normal, component.mVelocityVector );
        component.mVelocityVector += frictionForce * deltaTime;
        gea::EngineInit::registry.Transforms[entityID].mPosition  += component.mVelocityVector * deltaTime; // speed calculated with friction


        if(terrainInfo.pos != 0)
        {
            float targetY = component.mBarrySentricCoord + gea::EngineInit::registry.SphereCollision[entityID].radius;

            if(gea::EngineInit::registry.Transforms[entityID].mPosition.y < targetY)
                gea::EngineInit::registry.Transforms[entityID].mPosition.y = targetY;
        }

        //Change in rotation
        float rotasjonsVinkel = glm::length(gea::EngineInit::registry.Transforms[entityID].mPosition - oldPosition)/
                                gea::EngineInit::registry.SphereCollision[entityID].radius;
        rotasjonsVinkel *= 10.0f; // Higher number feels more correct.
        if(!(component.mVelocityVector == glm::vec3(0)))
        {
            gea::EngineInit::registry.Transforms[entityID].mRotation  += rotasjonsVinkel * component.mRollingDirectionVector;
            //qDebug("speed: %f ", entityPhysicsComponent.mVelocityVector);
         }
    }
}

glm::vec3 gea::PhysicsSystem::computeAccelerationVector(glm::vec3 normal)
{
    glm::vec3 gravity = glm::vec3(0,GRAVITAIONAL_ACCELERATION,0);
    glm::vec3 tangent = gravity - glm::dot(gravity,normal) * normal;
    return tangent;
}

glm::vec3 gea::PhysicsSystem::computeVelocityVector(glm::vec3 oldVelocityVector, glm::vec3 accelerationVector)
{
    return oldVelocityVector + accelerationVector;
}

glm::vec3 gea::PhysicsSystem::computeRotationVector(glm::vec3 normalVector, glm::vec3  velocityVector)
{
    //dont bother computing if the lenght is too short
    if(glm::length(velocityVector) < 0.001)
        return glm::vec3(0);
    if(glm::length(glm::cross(velocityVector,normalVector)) < 0.001) //return [0,0,0] if the lenght of the cross product is too low
        return  glm::vec3(0);

    //Not the same as the notes, yet the opposite gives wrong rotation
    return normalize(glm::cross(normalVector,velocityVector));
}

glm::vec3 gea::PhysicsSystem::computeFriction(float frictioncooefficient, glm::vec3 normal, glm::vec3 velocity)
{
    glm::vec3 tangent = velocity - glm::dot(velocity,normal) * normal;
    glm::vec3 friction = -tangent * frictioncooefficient;
    return friction;
}

gea::terrainInfo gea::PhysicsSystem::ComputeBarrysentricCoord(glm::vec3 Position, gea::Mesh* terrainMesh)
{
    glm::vec2 p(Position.x, Position.z);

    for (int i = 0; i < terrainMesh->indices.size() - 2; i += 3)
    {
            const Vertex& v0 = terrainMesh->Vertices[terrainMesh->indices[i]];
            const Vertex& v1 = terrainMesh->Vertices[terrainMesh->indices[i + 1]];
            const Vertex& v2 = terrainMesh->Vertices[terrainMesh->indices[i + 2]];

            glm::vec2 a(v0.pos.x, v0.pos.z);
            glm::vec2 b(v1.pos.x, v1.pos.z);
            glm::vec2 c(v2.pos.x, v2.pos.z);

            // Beregn areal av trekant ABC
            float areaABC = (b - a).x * (c - a).y - (b - a).y * (c - a).x;

            // Unngå deling på null
            if (fabs(areaABC) < 1e-6f) continue;

            // Beregn barysentriske koordinater
            float lambda1 = ((b - p).x * (c - p).y - (b - p).y * (c - p).x) / areaABC;
            float lambda2 = ((c - p).x * (a - p).y - (c - p).y * (a - p).x) / areaABC;
            float lambda3 = 1.0f - lambda1 - lambda2;

            // Sjekk om punktet ligger i trekanten
            if (lambda1 >= 0.0f && lambda2 >= 0.0f && lambda3 >= 0.0f)
            {
                // Interpoler høyde (Y)
                float height = lambda1 * v0.pos.y + lambda2 * v1.pos.y + lambda3 * v2.pos.y;

                // Beregn geometrisk normal fra 3D-punktene
                glm::vec3 pointA(v0.pos.x, v0.pos.y, v0.pos.z);
                glm::vec3 pointB(v1.pos.x, v1.pos.y, v1.pos.z);
                glm::vec3 pointC(v2.pos.x, v2.pos.y, v2.pos.z);
                glm::vec3 normal = glm::cross(pointB - pointA, pointC - pointA);
                normal = glm::normalize(normal);
                return terrainInfo{height, normal};
            }
        }

        // Returner standardverdi hvis punktet ikke ligger i noen trekant
        return terrainInfo{0.f, glm::vec3(.0f, 0.0f, 0.0f)};
    }
}

bool gea::PhysicsSystem::sphereCollisionDetection(glm::vec3 a, glm::vec3 b, float radiusA, float radiusB)
{
    glm::vec3 vector = b - a;
    return glm::length(vector) <= radiusA + radiusB;
}
