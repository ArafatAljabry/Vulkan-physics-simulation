#ifndef FLUIDSYSTEM_H
#define FLUIDSYSTEM_H

#include "System.h"
namespace gea
{

    class fluidSystem : public System
    {
    public:
        fluidSystem(){};
        ~fluidSystem(){};

        //these need not be private
        float rate{40};
        float spawnTimer{0};
        void Update(float deltaTime);


    };

} // end of namespace gea
#endif // FLUIDSYSTEM_H
