#include "fluidSystem.h"
#include "../engineinit.h"

void gea::fluidSystem::Update(float deltaTime)
{

    for (auto &[entityID,component] : gea::EngineInit::registry.fluidSimComponent)
    {
        if(component.isActive == true)
        {
            spawnTimer += deltaTime;
            continue;
        }
        if(spawnTimer >= rate)
        {
            gea::EngineInit::registry.Transforms[entityID].mPosition = component.spawnPos;
            component.isActive = true;
            spawnTimer = 0;
        }

        spawnTimer += deltaTime;
    }
}
