#include <iostream>
#include <SDL2/SDL.h>
#include "CGame.h"



int main(int argc, char *argv[]) {
    CGame game;

    float a = 1.2;
    cout << a + 2 << endl;


    if ( !  game.init()  ){
        cout << "Failed to init" << endl;
    }else{
        if ( ! game.loadMedia() ) {
            cout << "Failed to load media " << endl;
        }else{
            while(game.isRunning()){
                game.loop();
            }

        }


    }

    game.close();


    return 0;
}
