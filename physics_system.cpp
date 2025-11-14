#include "physics_system.h"
#include <QtMath>
physics_system::physics_system() {}

QVector3D physics_system::simulatePhysics(TerrainInfo info, VisualObject* obj)
{

    QVector3D g(0.0f,-9.81f,0.0f);

    QVector3D tyngdeKraftNormal = QVector3D::dotProduct(g, info.normal) *  info.normal;
    QVector3D tyngdeKraftTangent = g - tyngdeKraftNormal;



    QVector3D velocityDirection = QVector3D(v.x(), 0.0f, v.z()).normalized(); // velocity direction
    QVector3D FrictionDecelerationVec = -velocityDirection * (.15 / 1);  // friction opposes motion


    v += tyngdeKraftTangent * deltaTime;
    v += FrictionDecelerationVec * deltaTime;


    QVector3D updatedPos = obj->getPosition();
    updatedPos += v * deltaTime;



    return updatedPos;
}
