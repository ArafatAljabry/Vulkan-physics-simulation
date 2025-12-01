#include "RenderSystem.h"
#include "../BSplineCruve.h"
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

    if(!UpdateTracker)
        return;

    for(auto &[entityID, component] : gea::EngineInit::registry.Tracker)
    {
        if(!component.isTracking)
            continue;

        glm::vec3 trackedPos = gea::EngineInit::registry.Transforms[component.enitityToTrack].mPosition;

        bool found = false;
        for(auto &entity : mRenderer->entityRenderData)
        {
            if(entity.entityID == entityID)
            {
                found = true;

                // Add new control point
                component.controlPoints.push_back(trackedPos);

                // Only compute spline if we have enough points
                if(component.controlPoints.size() >= 4) // degree + 1
                {
                    BSplineCurve spline;
                    spline.degree = 3;
                    spline.controlPoints = component.controlPoints;

                    // Generate proper knot vector for de Boor algorithm
                    int n = (int)spline.controlPoints.size() - 1; // last control point index
                    int d = spline.degree;

                    // Knot vector size: n + d + 2
                    spline.t.resize(n + d + 2);

                    // Create clamped uniform knot vector
                    // First d+1 knots are 0
                    for(int i = 0; i <= d; i++)
                        spline.t[i] = 0.0f;

                    // Middle knots are uniformly spaced
                    for(int i = d + 1; i <= n; i++)
                        spline.t[i] = (float)(i - d);

                    // Last d+1 knots are max value
                    float maxKnot = (float)(n - d + 1);
                    for(int i = n + 1; i < (int)spline.t.size(); i++)
                        spline.t[i] = maxKnot;

                    // Debug: Print knot vector for first few iterations
                    if(component.controlPoints.size() <= 6)
                    {

                        QString knotStr = "";
                        for(float k : spline.t)
                            knotStr += QString::number(k) + " ";

                    }

                    // Clear and rebuild geometry
                    entity.vertices.clear();
                    entity.indices.clear();

                    // Sample the spline
                    // Valid parameter range is [t[d], t[n+1]]
                    float uMin = spline.t[d];
                    float uMax = spline.t[n + 1];
                    int totalSamples = 100;



                    for(int s = 0; s <= totalSamples; s++)
                    {
                        float u = uMin + (uMax - uMin) * (s / (float)totalSamples);

                        // Make sure we don't evaluate exactly at the last knot
                        if(s == totalSamples && u >= uMax)
                            u = uMax - 0.0001f;

                        glm::vec3 point = spline.EvaluateBSpline(u);

                        // Debug first and last points
                        if(s == 0 || s == totalSamples)
                            qDebug("Sample %d: u=%.3f -> (%.2f, %.2f, %.2f)",
                                   s, u, point.x, point.y, point.z);

                        Vertex vert {};
                        vert.pos = point;
                        vert.color = {1.0f, 0.0f, 0.0f};
                        vert.normal = {0.0f, 1.0f, 0.0f};
                        vert.texCoord = {0.0f, 0.0f};

                        entity.vertices.push_back(vert);
                    }

                    qDebug("Generated %d vertices", (int)entity.vertices.size());

                    // Create sequential indices for line strip
                    for(uint32_t i = 0; i < entity.vertices.size(); i++)
                    {
                        entity.indices.push_back(i);
                    }

                    // Update GPU buffers
                    if(entity.vertexBuffer == VK_NULL_HANDLE)
                    {
                        mRenderer->createVertexBuffer(entity);
                        mRenderer->createIndexBuffer(entity);
                    }
                    else
                    {
                        mRenderer->updateVertexBuffer(entity);
                        mRenderer->updateIndexBuffer(entity);
                    }
                }
                else
                {
                    qDebug("Waiting for more control points: %d/4",
                           (int)component.controlPoints.size());
                }
                break;
            }
        }

        if(!found)
        {
            qDebug("Creating new tracker entity with ID: %d", entityID);

            gea::EntityRenderData newERD;
            newERD.entityID = entityID;
            newERD.topology = 4; // Line strip
            newERD.mipLevels = 1;
            newERD.vertexBuffer = VK_NULL_HANDLE;
            newERD.indexBuffer = VK_NULL_HANDLE;

            if(!mRenderer->entityRenderData.empty())
            {
                newERD.textureImage = mRenderer->entityRenderData[0].textureImage;
                newERD.textureImageView = mRenderer->entityRenderData[0].textureImageView;
                newERD.textureSampler = mRenderer->entityRenderData[0].textureSampler;
                newERD.textureImageMemory = mRenderer->entityRenderData[0].textureImageMemory;
            }

            mRenderer->entityRenderData.push_back(newERD);
            mRenderer->needsDescriptorRebuild = true;
        }
    }
}
} // End of namespace gea
