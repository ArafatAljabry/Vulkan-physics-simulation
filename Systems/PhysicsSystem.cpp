#include "PhysicsSystem.h"
#include "../engineinit.h"
void gea::PhysicsSystem::update(float deltaTime)
{
    terrainInfo newYandNormal;
    for(auto &[entityID,component] : gea::EngineInit::registry.Transforms)
    {
        //terrainInfo newYAndNormal =  ComputeBarrysentricCoord();
    }
}

glm::vec3 gea::PhysicsSystem::computeAccelerationVector(glm::vec3 normal)
{
    return normal * (float)GRAVITAIONAL_ACCELERATION;
}

glm::vec3 gea::PhysicsSystem::computeVelocityVector(glm::vec3 oldVelocityVector, glm::vec3 accelerationVector)
{
    return oldVelocityVector + accelerationVector;
}

glm::vec3 gea::PhysicsSystem::computeRotationVector(glm::vec3 normalVector, glm::vec3 velocityVector)
{
    return glm::cross(normalVector,velocityVector);
}

gea::terrainInfo gea::PhysicsSystem::ComputeBarrysentricCoord(glm::vec3 Position, gea::Mesh terrainMesh)
{
    glm::vec2 p(Position.x, Position.z);

    for (int i = 0; i < terrainMesh.indices.size() - 2; i += 3)
    {
        const Vertex& v0 = terrainMesh.Vertices[terrainMesh.indices[i]];
        const Vertex& v1 = terrainMesh.Vertices[terrainMesh.indices[i + 1]];
        const Vertex& v2 = terrainMesh.Vertices[terrainMesh.indices[i + 2]];

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
    return terrainInfo{0.0f, glm::vec3(0.0f, 1.0f, 0.0f)};
}
