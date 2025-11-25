#include "RenderSystem.h"
#include "../core.h"
#include "../Renderer.h"

namespace gea
{

void RenderSystem::init(Renderer*  targetWindow)
{
    QLog("Creating a renderer","white");

    targetWindow->setTitle("Renderer");
    targetWindow->setWidth(1100);
    targetWindow->setHeight(700);
    targetWindow->initVulkan();
    mRenderer = targetWindow;
    if(mRenderer)
        QLog("renderer created", "white");
    //Trigger first frame
    targetWindow->requestUpdate();
}

void RenderSystem::Update(float deltaTime)
{
    if(mRenderer == nullptr)
        return;

    for(auto &[entityID, component] : gea::EngineInit::registry.Tracker)
    {
        if(component.isTracking != true)
            continue;

        glm::vec3 trackedPos = gea::EngineInit::registry.Transforms[component.enitityToTrack].mPosition;

        bool found = false;
        for(auto &entity : mRenderer->entityRenderData)
        {
            if(entity.entityID == entityID)
            {
                found = true;

                bool shouldAddPoint = entity.vertices.empty();
                if(!entity.vertices.empty())
                {
                    glm::vec3 lastPos = entity.vertices.back().pos;
                    float distance = glm::length(trackedPos - lastPos);
                    shouldAddPoint = (distance > 0.01f);
                }

                if(shouldAddPoint)
                {
                    Vertex vert {};
                    vert.pos = trackedPos;
                    vert.color = {1.0f, 0.0f, 0.0f};
                    vert.normal = {0.0f, 0.0f, 0.0f};
                    vert.texCoord = {0.0f, 0.0f};

                    entity.vertices.push_back(vert);

                    if(entity.vertices.size() >= 2)
                    {
                        uint32_t prevIndex = static_cast<uint32_t>(entity.vertices.size() - 2);
                        uint32_t currIndex = static_cast<uint32_t>(entity.vertices.size() - 1);
                        entity.indices.push_back(prevIndex);
                        entity.indices.push_back(currIndex);

                        if(entity.vertexBuffer == VK_NULL_HANDLE)
                        {
                            mRenderer->createVertexBuffer(entity);
                            mRenderer->createIndexBuffer(entity);
                            // DON'T recreate descriptors here - will happen in drawFrame
                        }
                        else
                        {
                            mRenderer->updateVertexBuffer(entity);
                            mRenderer->updateIndexBuffer(entity);
                        }
                    }
                }
                break;
            }
        }

        // Create new tracker entity if not found
        if(!found)
        {
            qDebug("Creating new tracker entity %d", entityID);

            gea::EntityRenderData newERD;
            newERD.entityID = entityID;
            newERD.topology = 2; // LINE_LIST
            newERD.mipLevels = 1;

            newERD.vertexBuffer = VK_NULL_HANDLE;
            newERD.vertexBufferMemory = VK_NULL_HANDLE;
            newERD.indexBuffer = VK_NULL_HANDLE;
            newERD.indexBufferMemory = VK_NULL_HANDLE;

            // Borrow texture from first entity
            if(!mRenderer->entityRenderData.empty())
            {
                newERD.textureImage = mRenderer->entityRenderData[0].textureImage;
                newERD.textureImageView = mRenderer->entityRenderData[0].textureImageView;
                newERD.textureSampler = mRenderer->entityRenderData[0].textureSampler;
                newERD.textureImageMemory = VK_NULL_HANDLE;
            }

            mRenderer->entityRenderData.push_back(newERD);

            // CRITICAL: Flag that descriptors need rebuild
            mRenderer->needsDescriptorRebuild = true;

            qDebug("Tracker entity %d added, descriptors need rebuild", entityID);
        }
    }
}
} // End of namespace gea
