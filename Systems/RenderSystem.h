#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "System.h"
#include "../Renderer.h"

namespace gea
{

class RenderSystem : public System
{
public:

    RenderSystem() {};
    ~RenderSystem() {};
    void init(Renderer* targetWindow);
    void render();
    Renderer *getRenderer() const{return mRenderer;}
    void Update(float deltaTime);


    VkDevice device;
    VkDescriptorPool descriptorPool;
    size_t numInstances;


    Renderer* mRenderer{nullptr};

    //Turn tracker on and off
    bool UpdateTracker;
};

} // End of namespace gea

#endif // RENDERSYSTEM_H
