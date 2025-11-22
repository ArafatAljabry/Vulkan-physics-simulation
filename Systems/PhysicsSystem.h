#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "System.h"
#include "glm/glm.hpp"
#include <vector>
#include "../Vertex.h"
#include "../Components/Components.h"

#define GRAVITAIONAL_ACCELERATION -9.81
namespace gea
{

    struct terrainInfo
    {
        float pos{-1};
        glm::vec3 normal;
    };
    class PhysicsSystem : public System
    {
    public:

        PhysicsSystem(){};
        ~PhysicsSystem(){};

        void update(float deltaTime);

        glm::vec3 computeAccelerationVector(glm::vec3 normal);
        glm::vec3 computeVelocityVector(glm::vec3 oldVelocityVector, glm::vec3 accelerationVector);
        glm::vec3 computeRotationVector(glm::vec3 normalVector, glm::vec3 velocityVector);
        glm::vec3 computeFriction(float frictioncooefficient, glm::vec3 normal, glm::vec3 velocity);
        /// @brief Given a position and a vector of vertices representing the terrain,
        ///        computes the barrysentric coordinates and returns the height and normal at that position.
        /// @param Position
        /// @param terrainVerticies
        terrainInfo ComputeBarrysentricCoord(glm::vec3 Position, gea::Mesh* terrainMesh);

        bool sphereCollisionDetection(glm::vec3 a, glm::vec3 b, float radiusA, float radiusB);

        gea::Mesh* terrainMesh{nullptr};

    };
} // End of namespace gea
#endif // PHYSICSSYSTEM_H
