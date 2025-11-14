#ifndef PHYSICS_SYSTEM_H
#define PHYSICS_SYSTEM_H

#include"VisualObject.h"
class physics_system
{
private:

    QVector3D v;
    float deltaTime{0.016};

public:

    physics_system();


    QVector3D simulatePhysics(TerrainInfo info, VisualObject* obj);

    bool turnON{false};
};

#endif // PHYSICS_SYSTEM_H
