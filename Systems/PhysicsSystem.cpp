#include "PhysicsSystem.h"
#include "../engineinit.h"
#include "../core.h"
#include "string.h"
namespace gea
{

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

    if(!PhysicsActive)
        return;
    for(auto &[entityID,component] : gea::EngineInit::registry.Physics)
    {

        //If object is part of fluid simulation, skip it until it is active to avoid broken computation
        if(gea::EngineInit::registry.fluidSimComponent.find(entityID) != gea::EngineInit::registry.fluidSimComponent.end())
            if(!gea::EngineInit::registry.fluidSimComponent[entityID].isActive)
                continue;

        // find barrysentric coordinates of this entity
        glm::vec3 entityPos = gea::EngineInit::registry.Transforms[entityID].mPosition;
        terrainInfo = optimizedBarrysentriCoordinates(entityPos, terrainMesh, 6); /* new and more optimized version was created,
                                                                                    since the old one is heavy as amount of object increases*/

        //friction cooefficient changes at a pre-determined height to simulate change in friction
        if(terrainInfo.pos <= -12)
            component.fricionCooefficienet = 1;



                            /******************************* Update physics component /********************************/
        glm::vec3 oldPosition = gea::EngineInit::registry.Transforms[entityID].mPosition;
        glm::vec3 frictionForce = computeFriction(component.fricionCooefficienet, terrainInfo.normal, component.mVelocityVector );


        component.mAccelerationVector     = computeAccelerationVector(terrainInfo.normal) + frictionForce ;
        component.mVelocityVector        += component.mAccelerationVector * deltaTime;
        component.mRollingDirectionVector = computeRotationVector(terrainInfo.normal,component .mVelocityVector);
        component.mBarrySentricCoord      = terrainInfo.pos;


                            /******************************* Collision detection and response*******************************/
        //only check collision if the entity has a collision component
        if(gea::EngineInit::registry.SphereCollision.find(entityID) != gea::EngineInit::registry.SphereCollision.end())
        {
            for(auto &[entityID2,component2] : gea::EngineInit::registry.Transforms)
            {
                //avoid self and avoid terrain
                if(component2.name == "Terrain")
                    continue;
                if(entityID == entityID2)
                    continue;
                // make sure the other entity has a collision component too
                if(gea::EngineInit::registry.SphereCollision.find(entityID2) == gea::EngineInit::registry.SphereCollision.end())
                    continue;
                if(sphereCollisionDetection(
                                            gea::EngineInit::registry.Transforms[entityID].mPosition,    // This entitys position
                                            component2.mPosition,                                        // Other entities position
                                            gea::EngineInit::registry.SphereCollision[entityID].radius,
                                            gea::EngineInit::registry.SphereCollision[entityID2].radius))
                {
                    if(component.collisionOn)
                    {
                        //Project collision normal?
                        glm::vec3 collisionNormal = glm::normalize(component2.mPosition - gea::EngineInit::registry.Transforms[entityID].mPosition);
                        component.mVelocityVector = component.mVelocityVector - 2.0f * glm::dot(component.mVelocityVector, collisionNormal) * collisionNormal;
                    }
                }
            }
        }


        //Doing both changes in physics and transform simultaniously as we dont have a system in place, and making one is unnecessary use of time
        //NOTE: for Gameengine, seperate the above code with the one below ( note to self)

        //use physics variables to update sphere rotation and position, making it roll on the surface
        //Update its velocity only within the domain of the terrain

        //Ensure the ball is still on the terrain
        if (terrainInfo.pos = 0.0f && glm::length(component.mVelocityVector) < 0.001f) // if we're outside the terrain or moving slow enough, shut it down.
            component.mVelocityVector = glm::vec3(0);

        //Update entity position
        gea::EngineInit::registry.Transforms[entityID].mPosition  += component.mVelocityVector * deltaTime; // speed calculated with friction

                                      /******************************* Update height /********************************/

        // keep the entity above the terain.
        if(terrainInfo.pos != 0 || terrainInfo.normal != glm::vec3(0.f)) // 0 Is a default value when computing barrysentric coordinates.
        {
            // Barrysentric coordinates + radius is will keep the ball above terrain
            float targetY = component.mBarrySentricCoord + gea::EngineInit::registry.SphereCollision[entityID].radius;
            auto& entityY = gea::EngineInit::registry.Transforms[entityID].mPosition.y;
            entityY = targetY;
        }


                                     /******************************* Update rotation /********************************/
        //Change in rotation
        float rotasjonsVinkel = glm::length(gea::EngineInit::registry.Transforms[entityID].mPosition - oldPosition)/
                                gea::EngineInit::registry.SphereCollision[entityID].radius;
        rotasjonsVinkel *= 10.0f; // Scaling it feels more correct.

        //only rotate if we are moving
        if(!(component.mVelocityVector == glm::vec3(0)))
        {
            gea::EngineInit::registry.Transforms[entityID].mRotation  += rotasjonsVinkel * component.mRollingDirectionVector;
         }
    }
}

glm::vec3 gea::PhysicsSystem::computeAccelerationVector(glm::vec3 normal)
{
    glm::vec3 gravity = glm::vec3(0,GRAVITAIONAL_ACCELERATION,0);
    glm::vec3 tangent = gravity - glm::dot(gravity,normal) * normal;
    return tangent;
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

gea::terrainInfo gea::PhysicsSystem:: ComputeBarrysentricCoord(glm::vec3 Position, gea::Mesh* terrainMesh)
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

gea::terrainInfo gea::PhysicsSystem::optimizedBarrysentriCoordinates(glm::vec3 Position, gea::Mesh* terrainMesh, int gridResolution)
{
    if(!runOnce)
    {
        bool hasSetOriginalVertex = 0;
        for( auto& vertex : terrainMesh->Vertices)
        {
            if(!hasSetOriginalVertex) {
                minX=vertex.pos.x;
                minZ=vertex.pos.z;
                maxX=vertex.pos.x;
                maxZ=vertex.pos.z;
                hasSetOriginalVertex=1;
            }
            if(vertex.pos.x < minX)
                minX = vertex.pos.x;
            if(vertex.pos.z < minZ)
                minZ = vertex.pos.z;


            if(vertex.pos.z > maxZ)
                maxZ = vertex.pos.z;
            if(vertex.pos.x > maxX)
                maxX = vertex.pos.x;
        }
        runOnce = true;
        distanceZ = maxZ - minZ;
        distanceX = maxX - minX;
    }

    float indexXfloat = (Position.x - minX) / distanceX * 84;
    int indiceX = glm::floor(indexXfloat);

    float indexZfloat = (Position.z - minZ) / distanceZ * 17;
    int indiceZ = glm::floor(indexZfloat);

    if(indiceX < 0 || indiceX >= 84 || indiceZ < 0 || indiceZ >= 17)
    {
        qDebug(" out of bound");
        return terrainInfo{0.f, glm::vec3(0.f)};
    }

    int cellIndex = indiceX + indiceZ * 84;
    auto terr = barrysentriCoordinatesAndNormal(Position, cellIndex, terrainMesh);
    return terr;
}

gea::terrainInfo gea::PhysicsSystem::barrysentriCoordinatesAndNormal(glm::vec3 Position, int indice, gea::Mesh* terrainMesh)
{
    glm::vec2 p(Position.x, Position.z);
    int baseIndex = indice * 6; // 2 triangler per celle * 3 vertex per triangel

    if(baseIndex + 5 >= terrainMesh->indices.size()) {
        qDebug("Index out of range");
        return terrainInfo{0.f, glm::vec3(0.f)};
    }

    for(int tri = 0; tri < 2; ++tri)
    {
        int triBase = baseIndex + tri * 3;
        const Vertex& v0 = terrainMesh->Vertices[terrainMesh->indices[triBase]];
        const Vertex& v1 = terrainMesh->Vertices[terrainMesh->indices[triBase + 1]];
        const Vertex& v2 = terrainMesh->Vertices[terrainMesh->indices[triBase + 2]];

        glm::vec2 a(v0.pos.x, v0.pos.z);
        glm::vec2 b(v1.pos.x, v1.pos.z);
        glm::vec2 c(v2.pos.x, v2.pos.z);

        float areaABC = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
        if(fabs(areaABC) < 1e-6f) continue;

        float lambda1 = ((b - p).x * (c - p).y - (b - p).y * (c - p).x) / areaABC;
        float lambda2 = ((c - p).x * (a - p).y - (c - p).y * (a - p).x) / areaABC;
        float lambda3 = 1.0f - lambda1 - lambda2;

        if(lambda1 >= 0.f && lambda2 >= 0.f && lambda3 >= 0.f)
        {
            float height = lambda1 * v0.pos.y + lambda2 * v1.pos.y + lambda3 * v2.pos.y;
            glm::vec3 normal = glm::normalize(glm::cross(v1.pos - v0.pos, v2.pos - v0.pos));
            return terrainInfo{height, normal};
        }
    }

    qDebug("Point not inside any triangle");
    return ComputeBarrysentricCoord(Position,terrainMesh);
}
