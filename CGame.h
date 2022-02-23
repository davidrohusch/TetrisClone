//
// Created by david on 09.07.2021.
//

#ifndef TETRIS_CGAME_H
#define TETRIS_CGAME_H


    #include <SDL2/SDL.h>
    #include <SDL2/SDL_ttf.h>
    #include <SDL2/SDL_mixer.h>
    #include <SDL2/SDL_image.h>



#include <windows.h>



    #include <stdio.h>

    #include <iostream>

    #include <vector>

    const int SCREEN_WIDTH = 162;
    const int SCREEN_HEIGHT = 503;
    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;


    using namespace std;



class LTimer
{
public:
    //Initializes variables
    LTimer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unpause();

    //Gets the timer's time
    Uint32 getTicks();

    //Checks the status of the timer
    bool isStarted();
    bool isPaused();

private:
    //The clock time when the timer started
    Uint32 mStartTicks;

    //The ticks stored when the timer was paused
    Uint32 mPausedTicks;

    //The timer status
    bool mPaused;
    bool mStarted;
};


    struct brick{
        string name;

        bool shape[2][4];

    };


class CGame{
private:
    //TTF_Font* Sans;
    TTF_Font  *gFont = NULL;
    int moved = 0;
    int score = 0;
    int gameBoard[31][10];
    vector<brick> bricks;

    //Main loop flag
    bool quit = false;

    //Event handler
    SDL_Event e;

    void handleEvents () ;

    Mix_Music *themeMusic = NULL;

    int test = 0;
    int dir = 0;
    int x , y;


    void DrawBoard();
    void DrawScore();

    void moveBlockDown();
    void clearActiveBlock();
    void rotateBlock();

    bool isValid(int y, int x, int value) ;

    bool placeDown();



public:
    SDL_Surface *spr_block;
    SDL_Surface *spr_gamefield;

    SDL_Texture *spr_gmdfield = NULL;
    SDL_Texture *spr_blck = NULL;

    SDL_Window *gWindow = NULL;
    SDL_Surface *gScreenSurface = NULL;
    SDL_Renderer* gRenderer = NULL;

    brick currentBrick ;
    bool generateBrick = true;

    int tickSpeed = 30;
    int tick = 0;

    bool rotate = false;


    //The frames per second timer
    LTimer fpsTimer;

    //The frames per second cap timer
    LTimer capTimer;



    //Start counting frames per second
    int countedFrames = 0;

    bool checkBoard();

    void removeDown (int y, int cnt);

    void createBlock();

public:
    CGame();

    //Starts up SDL and creates window
    bool init();

    //Loads media
    bool loadMedia();

    //Frees media and shuts down SDL
    void close();

    bool isRunning () const;

    void loop () ;


    SDL_Surface* loadSurface(unsigned short path );


    //Loads individual image as texture
    SDL_Texture* loadTexture( std::string path );


    void removeRow(int row);
};






#endif //TETRIS_CGAME_H
