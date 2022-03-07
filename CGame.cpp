//
// Created by david on 09.07.2021.
//

#include "CGame.h"
#include "rsc.h"

CGame::CGame() {
    for(int i = 0 ; i < 31 ; i++){
        for(int k = 0 ; k < 10 ; k ++ ){
            gameBoard[i][k] = 0;
        }
    }


    bricks.push_back(brick{"I",{
                                   {true, true, true, true},
                                   {false,false,false,false}
                           } });

    bricks.push_back(brick{"L",{
                                   {true, false, false, false},
                                   {true, true, true,   false}
                           } });

    bricks.push_back(brick{"L",{
                                   {false, false, true, false},
                                   {true, true, true,   false}
                           } });
    bricks.push_back(brick{"SQUARE",{
                                   {true, true, false, false},
                                   {true, true, false, false}
                           } });
    bricks.push_back(brick{"S",{
                                   {false, true, true, false},
                                   {true, true, false, false}
                           } });
    bricks.push_back(brick{"Z",{
                                   {true, true, false, false},
                                   {false, true, true, false}
                           } });
    bricks.push_back(brick{"TRIANGLE",{
                                   {false, true, false, false},
                                   {true, true, true, false}
                           } });


}

bool CGame::init() {
    //Initialization flag
    bool success = true;
    //Sans = TTF_OpenFont("Sans.ttf", 24);

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "TETRIS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            int imgFlags = IMG_INIT_PNG;
            if( !( IMG_Init( imgFlags ) & imgFlags ) )
            {
                printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                success = false;
            }
            else
                gScreenSurface = SDL_GetWindowSurface( gWindow ); //Get window surface

        }
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }


    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
    if(gRenderer==NULL) printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );

    return success;
}

bool CGame::loadMedia() {
    //Loading success flag
    bool success = true;
    if(TTF_Init() == -1) {
        printf("[ERROR] TTF_Init() Failed with: %s\n", TTF_GetError());
        exit(2);
    }

    HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(GM_FONT), RT_FONT);
    HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
    void* pMyBinaryData = ::LockResource(myResourceData);

    SDL_RWops* fnt = SDL_RWFromConstMem(pMyBinaryData, SizeofResource(NULL, myResource)); ;
  gFont =          TTF_OpenFontRW(fnt, 0, 28);

    if(!gFont){
        printf("[ERROR] TTF_OpenFont() Failed with: %s\n", TTF_GetError());
        exit(2);
    }
    //Load splash image



    //spr_block = loadSurface(blck);

    cout << BLCK_PNG << endl;

    spr_block = loadSurface(BLCK_PNG);
    spr_gamefield = loadSurface (GM_PNG);

    if( spr_block == NULL || spr_gamefield == NULL)
    {
        printf( "Unable to load image %s! SDL Error:\n", SDL_GetError() );
        success = false;

    }



    gScreenSurface = SDL_GetWindowSurface( gWindow );

    spr_blck = SDL_CreateTextureFromSurface (gRenderer, spr_block);
    spr_gmdfield = SDL_CreateTextureFromSurface (gRenderer, spr_gamefield);


     myResource = ::FindResource(NULL, MAKEINTRESOURCE(GM_MUSIC), RT_RCDATA);
     myResourceData = ::LoadResource(NULL, myResource);
     pMyBinaryData = ::LockResource(myResourceData);

    SDL_RWops* msc = SDL_RWFromConstMem(pMyBinaryData, SizeofResource(NULL, myResource)); ;

    themeMusic = Mix_LoadMUS_RW(msc, 0);
    Mix_VolumeMusic(1);
   // themeMusic = Mix_LoadMUS( "Tetris_theme.ogg" );


    Mix_PlayMusic( themeMusic, -1 );
    return success;
}

void CGame::close() {
    //Deallocate surface
    SDL_FreeSurface( spr_block);
    SDL_FreeSurface(spr_gamefield);
    spr_block = NULL;
    spr_gamefield = NULL;

    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;


    Mix_FreeMusic( themeMusic );
    themeMusic = NULL;



    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

bool CGame::isRunning() const {
    return !quit;
}

void CGame::loop() {
    fpsTimer.start();

    //Calculate and correct fps
    float avgFPS = countedFrames / ( fpsTimer.getTicks() / 1000.f );
    if( avgFPS > 2000000 )
    {
        avgFPS = 0;
    }

    handleEvents();

    if(generateBrick){
        createBlock();
        rotateBlock();
        generateBrick = false;
    }

    if(rotate){
        rotate = false;
        (++dir)%=4;
        cout << dir <<endl;

        clearActiveBlock();
        rotateBlock();

    }

    //Clear screen
    SDL_RenderClear( gRenderer );

    SDL_Rect rct;
    rct.x=0; rct.y=0; rct.w = spr_gamefield->w; rct.h = spr_gamefield->h;

    //Render texture to screen
    SDL_RenderCopy( gRenderer, spr_gmdfield, nullptr, &rct);

    if(tick > tickSpeed) {
        moveBlockDown();
        score++;

        // if( ! checkIfPlaced() )
        //      moveBlockDown();
    //     else
        //      placeBlock()

        tick = 0;
    }
    else {
        tick++;
    }

    clearActiveBlock();
    rotateBlock();

    DrawBoard();

    DrawScore();

    SDL_RenderPresent( gRenderer );

    //If frame finished early
    int frameTicks = capTimer.getTicks();
    if( frameTicks < SCREEN_TICKS_PER_FRAME )
    {
        //Wait remaining time
        SDL_Delay( SCREEN_TICKS_PER_FRAME - frameTicks );
    }
}

void CGame::handleEvents() {
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            quit = true;
        }


        if (e.type == SDL_KEYUP){
            if(e.key.keysym.sym == SDLK_DOWN){
                tickSpeed = 30;
            }
        }

        if  (e.type == SDL_KEYDOWN){

            if(e.key.keysym.sym == SDLK_DOWN){
                tickSpeed = 1;
            }

            if(e.key.keysym.sym == SDLK_RIGHT){
                x++;
                //Check if it didn't create collision.
                moved =  1;
            }
            if(e.key.keysym.sym == SDLK_LEFT){
                x--;
                moved = -1;
            }

            if(e.key.keysym.sym == SDLK_UP && e.key.repeat == 0){
                //for(int i = 0; i < 10 ; i++)
                 //   gameBoard[30][i] = 1;

                 cout << " ----------------- " << endl;
                for(int i = 0; i <31 ; i++){
                    for(int k = 0; k < 10 ; k ++){
                        cout << gameBoard[i][k] << "   ";
                    }
                    cout << endl;
                }
                cout << " ----------------- " << endl;
            }


            if(e.key.keysym.sym == SDLK_RETURN && e.key.repeat == 0){
                //rotate;
                rotate = true;
            }


            if(e.key.keysym.sym == SDLK_F1 && e.key.repeat ==0){

                createBlock();
                clearActiveBlock();
                rotateBlock();
            }
        }
    }
}

SDL_Surface *CGame::loadSurface(unsigned short path) {
        //The final optimized image
        SDL_Surface* optimizedSurface = NULL;

        //Load image at specified path
        HRSRC myResource = ::FindResource(NULL, MAKEINTRESOURCE(path), RT_RCDATA);
        HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
        void* pMyBinaryData = ::LockResource(myResourceData);

        // cout << sizeof(pMyBinaryData) << endl;

        SDL_RWops *image = SDL_RWFromConstMem(pMyBinaryData, SizeofResource(NULL, myResource));

        SDL_Surface* loadedSurface = IMG_Load_RW(image, 0);
        // SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
        if( loadedSurface == NULL )
        {
         //   printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
        }
        else
        {
            //Convert surface to screen format
            optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, 0 );
            if( optimizedSurface == NULL )
            {
          //      printf( "Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
            }

            //Get rid of old loaded surface
            SDL_FreeSurface( loadedSurface );
        }

        return optimizedSurface;
    }

SDL_Texture *CGame::loadTexture(std::string path) {
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}

void CGame::DrawBoard() {
    int offset = 1;

    SDL_Rect rct;
    rct.w = spr_block->w;
    rct.h = spr_block->h;

    for(int i = 1 ; i < 31 ; i ++){
        for(int k = 0 ; k < 10 ; k++){
            if(gameBoard[i][k] != 0){
                rct.x = (k * 16) + offset;
                rct.y = ((i-1) * 16);
                SDL_RenderCopy(gRenderer, spr_blck, nullptr, &rct);
            }
        }
    }
}

void CGame::createBlock() {
    x=4; y=3; dir = 0;

    currentBrick = bricks[(rand() % 7)];

        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 2; k++) {

                if (!isValid(y+k, x+i,currentBrick.shape[k][i] )){
                    cout << "GAME OVER" << endl ; exit(2);
                }
            }
        }
}

void CGame::clearActiveBlock() {
    for(int i = 0 ; i < 31; i++){
        for(int k = 0 ; k < 10 ; k++ ){
            if(gameBoard[i][k] == 2) gameBoard[i][k] = 0;
        }
    }
}

void CGame::rotateBlock() {
    int offsetX = 0;
    int offsetY = 0;
    bool okay = true;

    if(currentBrick.name=="SQUARE"){
       // if(dir )
    }

    do {
        clearActiveBlock();
        okay = true;
        offsetX = 0;
        offsetY = 0;

        if (dir == 0) {
            if (currentBrick.name == "I") {
                offsetX = -1;
            }
            for (int i = 0; i < 4; i++) {
                for (int k = 0; k < 2; k++) {

                    if(!isValid((y + k + offsetY),(x + i + offsetX), (currentBrick.shape[k][i] * 2)))
                        okay = false;

                    if ( currentBrick.shape[k][i] )
                        if(y+k+offsetY >= 0)
                         gameBoard[y + k + offsetY][x + i + offsetX] =  2;
                }
            }
        }

        if (dir == 2) {
            if (currentBrick.name == "SQUARE") {
                offsetX = -1;
                offsetY = -1;
            }
            if (currentBrick.name == "S" || currentBrick.name == "Z") {
                offsetY = -1;
            }
            if (currentBrick.name == "I") {
                offsetY = -2;
            }

            for (int i = 3; i >= 0; i--) {
                for (int k = 1; k >= 0; k--) {

                    if(!isValid((+2 + y + (-k) + offsetY),(2 + x + (-i) + offsetX), (currentBrick.shape[k][i] * 2)))
                        okay = false;

                    if (currentBrick.shape[k][i])
                        if(+2 + y + (-k) + offsetY >= 0)
                        gameBoard[+2 + y + (-k) + offsetY][2 + x + (-i) + offsetX] =  2;
                }
            }
        }
        if (dir == 1) {
            if (currentBrick.name == "SQUARE") {
                offsetX = -1;
            }
            if (currentBrick.name == "S" || currentBrick.name == "Z") {
                offsetY = -1;
            }
            if (currentBrick.name == "I") {
                offsetX = -1;
                offsetY = -1;
            }

            for (int i = 1; i >= 0; i--) {
                for (int k = 3; k >= 0; k--) {

                    if(!isValid((y + k + offsetY),(1 + x + i + offsetX), (currentBrick.shape[1 - i][k] * 2)))
                        okay = false;

                    if (currentBrick.shape[1 - i][k] )
                        if(y + k + offsetY >= 0)
                            gameBoard[y + k + offsetY][1 + x + i + offsetX] = 2;
                }
            }
        }
        if (dir == 3) {
            if (currentBrick.name == "SQUARE") {
                offsetY = -1;
            }
            if (currentBrick.name == "S" || currentBrick.name == "Z") {
                offsetY = -1;
            }
            if (currentBrick.name == "I") {
                offsetX = 1;
            }
            for (int i = 1; i >= 0; i--) {
                for (int k = 3; k >= 0; k--) {

                    if(!isValid((-1 + y + k + offsetY),(x + i + offsetX), (currentBrick.shape[i][3 - k] * 2)))
                        okay = false;

                    if(currentBrick.shape[i][3 - k])
                        if ( -1 + y + k + offsetY >= 0)
                            gameBoard[-1 + y + k + offsetY][x + i + offsetX] =  2;
                }
            }
        }
    }while (!okay);
}

void CGame::moveBlockDown() {
    if ( !placeDown() ) {
        y = y + 1;
    }
    else{
            checkBoard();
            createBlock();
        }
}

void CGame::DrawScore() {
    SDL_Color White = {255, 255, 255};
    string str_score = "Score: " + to_string(score);
    SDL_Surface* surfaceMessage =
            TTF_RenderText_Solid(gFont, str_score.c_str() , White);

    SDL_Texture* Message = SDL_CreateTextureFromSurface(gRenderer, surfaceMessage);


    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 3;  //controls the rect's x coordinate
    Message_rect.y = 475; // controls the rect's y coordinte
    Message_rect.w = str_score.length() * 10; // controls the width of the rect
    Message_rect.h = 30; // controls the height of the rect
    SDL_RenderCopy(gRenderer, Message, NULL, &Message_rect);

// Don't forget to free your surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

bool CGame::isValid(int y, int x, int value) {

   // cout << "X: " << x << "  |  " << value << endl;

    if (value == 0) return true;
    if (gameBoard[y][x] == 1){
        if(moved == 1)  CGame::x--;
        else
        if(moved ==-1)  CGame::x++;
        else{
            CGame::y--;
            tick = 0;
        }
        moved = 0;

        return false;
    }

    if(y > 31){
        CGame::y--;
        return false;
    }
    if (y < 1){
        CGame::y++;
        return false;
    }
    if (x > 9){
        CGame::x--;
        return false;
    }
    if (x < 0){
        CGame::x++;
        return false;
    }


    /// X CORRECTION
    /* NOT IMPORTANT WHEN ROTATING
    for (int i  = 0 ; i < 10 ; i++){
        if (gameBoard[y][i] == 1){

            return false;
        }
    }*/

    /// Y CORRECTION

    return true;
}

bool CGame::placeDown() {
    bool result = false;
    ///check dolek
    for ( int k = 0 ; k < 10 ; k++){
        if (gameBoard[30][k] == 2){
            result = true;
            //cout << "PLACED" << endl;
            for ( int j = 0 ; j < 10 ; j++)
                for (int u = 0 ; u < 31 ; u++){
                    if ( gameBoard[u][j]  == 2 )
                        gameBoard[u][j]  = 1;
                }
            return result;
        }
            for ( int i = 0 ; i < 30 ; i++){
                ///check kolizi
                if (gameBoard[i][k] == 2){
                   if(gameBoard[i+1][k] == 1){
                       result = true;
                       for ( int j = 0 ; j < 10 ; j++)
                           for (int u = 0 ; u < 31 ; u++){
                               if ( gameBoard[u][j]  == 2 )
                                   gameBoard[u][j]  = 1;
                           }
                       return result;
                   }
                }
        }
    }

    return result;
}



bool CGame::checkBoard() {
    int cnt = 0;
    int y = -1;

    for(int i = 30; i >=0 ; i--){
        bool check = true;

        for(int k = 0; k < 10 ; k++){
                if(gameBoard[i][k] != 1 ){
                    check = false;
                    break;
                }
        }

        if ( check ){
            removeRow(i);
            cnt++;
            i++;
        }

    }
    score += cnt*100;
    return false;
}

void CGame::removeRow(int row) {
    cout << row << endl;
    for(int i = row ; i > 0 ; i--){
        for(int k = 0 ; k < 10 ; k++){
           // gameBoard[i+1][k] = gameBoard[i][k];

           if(i==row)gameBoard[i][k] = 0;

            gameBoard[i][k] = gameBoard[i-1][k];
        }
    }
}


LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}



void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
    mPausedTicks = 0;
}


void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

    //Clear tick variables
    mStartTicks = 0;
    mPausedTicks = 0;
}

void LTimer::pause()
{
    //If the timer is running and isn't already paused
    if( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
        mStartTicks = 0;
    }
}


void LTimer::unpause()
{
    //If the timer is running and paused
    if( mStarted && mPaused )
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}


Uint32 LTimer::getTicks()
{
    //The actual timer time
    Uint32 time = 0;

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}


bool LTimer::isStarted()
{
    //Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
    //Timer is running and paused
    return mPaused && mStarted;
}
