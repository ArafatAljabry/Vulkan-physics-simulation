#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "../Vertex.h"

//Need namespace, since we start to get naming collisions with other code
namespace gea
{

//Add new component types here
enum class ComponentTypes
{
    Transform,
    Mesh,
    Texture,
    Camera,
    Physics
};


//Below are examples of actual components
//Systems need each component to know what entity it belongs to

struct Component
{
    short mEntityID{-1};

};

struct Transform : Component
{
    std::string name;
    glm::vec3 mPosition{0.0f, 0.0f, 0.0f};
    glm::vec3 mRotation{0.0f, 0.0f, 0.0f};
    glm::vec3 mScale{1.0f, 1.0f, 1.0f};
};

struct Mesh : Component
{
    short mMesh{-1};        //index into array of all meshes loaded
    std::string path;

    //Need these two for the physics system to find barrycoord
    std::vector<Vertex>   Vertices;
    std::vector<uint32_t> indices;
    /*********************************************************/
    int topology{1};
};


struct Texture : Component
{
    short mTexture{-1};        //index into array of all textures loaded
    std::string path;
};

struct Camera: Component
{
    glm::mat4x4 mProjectionMatrix{}; //How things will look in the viewport
    glm::mat4x4 mViewMatrix{};       //Actual position of camera

    //NOTE: Should this part be its own component? Like a seperate transform for camera
    glm::vec3 Position{-10.0f,1.0f,0.0f};  //Camera position
    glm::vec3 Direction {0.0f,0.0f,-1.0f}; //Forward vector
    glm::vec3 Right{1.0f,0.0f,0.0f};
    glm::vec3 Up {0.0f,1.0f,0.0f};  //Up vector


    float mSpeed{0.01f};          //Camera will move at this speed
    float mRotationSpeed{1.0f}; // Camera will rotate at this speed
    float mPitch{0.5f};
    float mYaw{.5f};
    float mRoll{0.f};

    //Lens
    //NOTE: Own camera lens component?
    double mFieldOfView{60.f}; //In degrees
    double mAspect{1.57142857f}; //TODO: must be a better way to get the aspect ratio
    double mNearplane{0.1f};
    double farplane {10000.0f};

    bool isActive{false};

};

struct Physics: Component
{
    glm::vec3 mVelocityVector{0}; // initalizes as [0,0,0]
    glm::vec3 mAccelerationVector{0};
    glm::vec3 mRollingDirectionVector{0};
    float mBarrySentricCoord{0};

    bool collisionOn{true};

};

// New struct to hold per-entity rendering data
struct EntityRenderData : Component
{
    int topology{1};
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    uint32_t mipLevels;

    uint32_t entityID;  // To track which entity this belongs to
};
struct fluidSimComponent : Component
{
    bool isActive{false};
    glm::vec3 spawnPos{0.0f};
};
struct SphereCollision : Component
{
     float radius{1}; // we're only dealing with sphere collision, anything else is outside the frustum for now
};

struct Tracker : Component
{
    size_t enitityToTrack{};
    bool isTracking{false};
    std::vector<glm::vec3> controlPoints; // For B-spline computation
};

//TODO: SoundSource - Expand sound component
// struct SoundSource
// {

// };

}   //namespace gea

#endif // COMPONENTS_H
