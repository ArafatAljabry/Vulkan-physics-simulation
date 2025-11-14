#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "VisualObject.h"
#include <string>

class HeightMap : public VisualObject
{
public:
    HeightMap();

    void makeTerrain(std::string heightMapImage);

    void makeTerrain(unsigned char* textureData, int width, int height);
    float barysentricCoordFromTerrain(const QVector3D& positionXZ);

private:
    int mWidth{2};
    int mHeight{4};
	int mChannels{ 0 };


};

#endif // HEIGHTMAP_H
