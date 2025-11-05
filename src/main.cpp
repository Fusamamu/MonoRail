#include "Engine.h"

int main()
{
    for (int dy = -1; dy <= 0; ++dy){
        for (int dz = -1; dz <= 0; ++dz){

            int tx, ty, tz;

            if (dz == -1){
                for (int dx = 0; dx > -2; --dx){
                    std::cout << dx << ", " << dz << ", " << dy << std::endl;
                }
            }
            else{
                for (int dx = -1; dx <= 0; ++dx){


                    std::cout << dx << ", " << dz << ", " << dy << std::endl;
                }
            }
        }
    }

    Engine _engine;
    _engine.init();
    _engine.update();
    _engine.quit();

    return 0;
}
