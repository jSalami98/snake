/**
 * newSnake.cpp
 * @author John Salazar
 * @version 0.7.0
 * @date March 3, 2023
*/

/**
 * Known Bugs:
 * par   1: Draws an extra snake part                                                                                                        |   High
 * fixed 2: The right and bottem of the screen don't kill the snake                                                                          |   High
 * par   3: Food sometimes does not respawn sometimes                                                                                        |   Low
 *       4: Multiple inputs can be entered before the logic updates. This causes the snake to try to move back onto itself, killing itself   |   Medium
 *       5: game lags at random times (not sure this is my falt, and not sure how to fix it yet)                                             |   Low
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <vector>

//#define SNAKE_DEBUG

#define SCREEN_WIDTH 640 /*1280*/ 
#define SCREEN_HEIGHT 640 /*720*/
#define SCREEN_SCALE 1

#define GAME_SPACE_SIZE 16
#define DELAYAMOUNT 300

struct snake
{
    int length = 2;                                 //how long is the snake?
    int dir = 4;                                    //what direction is the snake facing
    std::vector<SDL_Rect> snakeParts;               //where to draw the snake's body parts
};

void placeFood(int gs[GAME_SPACE_SIZE][GAME_SPACE_SIZE], int snakeSize, int spaceSize = GAME_SPACE_SIZE)
{
    srand(time(nullptr));
    int foodPos;

    //calculate number of free spaces left
    int spacesLeft = (GAME_SPACE_SIZE * GAME_SPACE_SIZE) - (snakeSize+1);

    
    do
    {
        //generate the random number
        foodPos = rand() % spacesLeft;

        //place the food in the grid
        if(gs[foodPos/spaceSize][foodPos%spaceSize] == 0)
        {
            gs[foodPos/spaceSize][foodPos%spaceSize] = -1;
        }

    } while (gs[foodPos/spaceSize][foodPos%spaceSize] != -1);
    
    
    //gs[foodPos/spaceSize][foodPos%spaceSize] = -1;

}

/**
 * tries to change the snake to the new dir
 * 
 * the snake can only move left or right, from it's perspective
 * 
 * current dir      |       valid new dir
 * 1                |   2,4
 * 2                |   1,3
 * 3                |   2, 4
 * 4                |   1,3
*/
void handleChangeDir(snake &s, int newDir, bool* lock)          //this doesn't work -_-
{
    //if the snake's dir and the new dir have the same evenness (both are even or odd) return else snake.dir = new dir

    if(!(*lock) || s.dir % 2 == newDir % 2)
    {
        return;
    }
    s.dir = newDir;
    *lock = false;
}

void printGameSpace(int gs[GAME_SPACE_SIZE][GAME_SPACE_SIZE])
{
    for(int i = 0; i < GAME_SPACE_SIZE; i++)
        {
            for(int j = 0; j < GAME_SPACE_SIZE; j++)
            {
                std::cout << (gs[i][j] == -1? "*" : (gs[i][j] == 1? "A" : (gs[i][j] == 0? "-" : "S")));
            }
            std::cout << "\t";
            for(int j = 0; j < GAME_SPACE_SIZE; j++)
            {
                std::cout << gs[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
}

int main(int argc, char** argv)
{
    //SDL2 Initialization
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "\t~!ERROR: SDL failed to initialize!~\n" << SDL_GetError() << std::endl;
        return 1;
    }
    if(!(window = SDL_CreateWindow("Snake++", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0)))
    {
        std::cerr << "\t~!ERROR: SDL failed to open window!~\n" << SDL_GetError() << std::endl;
        return 1;
    }
    if(!(renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)))
    {
        std::cerr << "\t~!ERROR: SDL failed to create renderer!~\n" << SDL_GetError() << std::endl;
        return 1;
    }

    //Logic Initialization
    bool running = true;        //is the application still running?
    bool headFlag = false;      //Has the head been updated yet?
    bool tailFlag = false;      //Has the tail been updated yet?
    bool killFlag = false;      //Has the snake been killed
    bool inputFlag = true;      //Has the direction been updated for this logic cycle?
    bool findFood = true;      //for debug - has the food been found
    int delay = 0;              //My bootleg wait
    int gameSpace[GAME_SPACE_SIZE][GAME_SPACE_SIZE];    //holds all the game play stuff; -1 = food; 0 = open space; int>0 = part of snake (1 is the head, highest is the tail)
    snake sk;                   //The snake - logic
    SDL_Rect food;              //The food
    SDL_Event keyEvent;         //The snake - render
    //std::vector<SDL_Rect> toDraw();     //maybe not?

    /*  Initialize gameSpace to all 0s  */
    for(int i = 0; i < GAME_SPACE_SIZE; i++)
    {
        for(int j = 0; j < GAME_SPACE_SIZE; j++)
        {
            gameSpace[i][j] = 0;
        }
    }

    /*  Place the snake in gameSpace   */
    gameSpace[GAME_SPACE_SIZE/2][GAME_SPACE_SIZE/2] = 1;

    /*  Place the initial food  */
    placeFood(gameSpace, sk.length);

    //Main Loop
    
    while(running)
    { 
         /*  Input   */
        SDL_PollEvent(&keyEvent);
        switch(keyEvent.type)
        {
            case SDL_KEYDOWN:

                switch(keyEvent.key.keysym.sym)
                {
                    case SDLK_w:
                    case SDLK_UP:
                        handleChangeDir(sk, 1, &inputFlag);
                        #ifdef SNAKE_DEBUG
                        std::cout << "UP" << std::endl;
                        #endif
                    break;
                        
                    case SDLK_a:
                    case SDLK_LEFT:
                        handleChangeDir(sk, 4, &inputFlag);
                        #ifdef SNAKE_DEBUG
                        std::cout << "RIGHT" << std::endl;
                        #endif
                    break;

                    case SDLK_s:
                    case SDLK_DOWN:
                        handleChangeDir(sk, 3, &inputFlag);
                        #ifdef SNAKE_DEBUG
                        std::cout << "DOWN" << std::endl;
                        #endif
                    break;

                    case SDLK_d:
                    case SDLK_RIGHT:
                        handleChangeDir(sk, 2, &inputFlag);
                        #ifdef SNAKE_DEBUG
                        std::cout << "LEFT" << std::endl;
                        #endif
                    break;

                    default: 
                    std::cout << "Unknown" << std::endl;
                    break;
                }

            break;

            case SDL_QUIT:
                    running = false;
            break;
            default: break;
        }
        
        if(delay++ > DELAYAMOUNT)
        {
              
            /*  Logic   */
            #ifdef SNAKE_DEBUG
            printGameSpace(gameSpace);
            #endif

            sk.snakeParts.clear();
            delay = 0;

            for(int i = 0; i < GAME_SPACE_SIZE; i++)        //loop over gameSpace to update logic
            {
                for(int j = 0; j < GAME_SPACE_SIZE; j++)
                {
                    switch(gameSpace[i][j])     //determin what I am looking at
                    {
                        case -1:        // food - update SDL_Rect food with the position
                        
                            findFood = true;
                            food.x = (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                            food.y = (i*SCREEN_WIDTH) / GAME_SPACE_SIZE;
                            food.w = SCREEN_WIDTH / GAME_SPACE_SIZE;
                            food.h = SCREEN_HEIGHT / GAME_SPACE_SIZE;
                        
                        break;

                        case 0:         // empty space
                        //do nothing
                        break;

                        case 1:         //head - change the next space (acording to direction) to 1 then increment me      ~!This should only be done once!~
                            if(!headFlag)
                            {
                                headFlag = true;
                                SDL_Rect temp;
                                
                                switch(sk.dir)  //What way is the snake facing
                                {
                                    case 1: 
                                        if(i-1 < 0 || gameSpace[i-1][j] > 0){killFlag = true;} //kill
                                        else    //I am moving to a valid space
                                        {
                                            if(gameSpace[i-1][j] == -1) //am I about to move on to the food?
                                            {
                                                sk.length += 1;
                                                placeFood(gameSpace, sk.length);
                                                findFood = true;
                                            }

                                            //move the snake
                                            gameSpace[i-1][j] = 1;
                                            temp.x = (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                                            temp.y = ((i-1)*SCREEN_WIDTH) / GAME_SPACE_SIZE;    
                                        }
                                        break;
                        
                                    case 2: //fixed
                                        if(j+1 == GAME_SPACE_SIZE || gameSpace[i][j+1] > 0) {killFlag = true;} //kill
                                        else
                                        {
                                            if(gameSpace[i][j+1] == -1)
                                            {
                                                sk.length += 1;
                                                placeFood(gameSpace, sk.length);
                                                findFood = true;
                                            }

                                            gameSpace[i][j+1] = 1; 
                                            temp.x = ((j+1)*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                                            temp.y = (i*SCREEN_WIDTH) / GAME_SPACE_SIZE;    
                                        }
                                        break;
                                    case 3: //fixed
                                        if(i+1 > GAME_SPACE_SIZE || gameSpace[i+1][j] > 0) {killFlag = true;} //kill
                                        else
                                        {
                                            if(gameSpace[i+1][j] == -1)
                                            {
                                                sk.length += 1;
                                                placeFood(gameSpace, sk.length);
                                                findFood = true;
                                            }

                                            gameSpace[i+1][j] = 1;
                                            temp.x = (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                                            temp.y = ((i+1)*SCREEN_WIDTH) / GAME_SPACE_SIZE;
                                        }
                                        break;
                                    case 4: //fixed 
                                        if(j-1 < 0 || gameSpace[i][j-1] > 0) {killFlag = true;} //kill
                                        else
                                        {
                                            if(gameSpace[i][j-1] == -1)
                                            {
                                                sk.length += 1;
                                                placeFood(gameSpace, sk.length);
                                                findFood = true;
                                            }

                                            gameSpace[i][j-1] = 1;
                                            temp.x = ((j-1)*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                                            temp.y = (i*SCREEN_WIDTH) / GAME_SPACE_SIZE;    
                                        }
                                        break;
                                }
                                temp.w = SCREEN_WIDTH / GAME_SPACE_SIZE;
                                temp.h = SCREEN_HEIGHT / GAME_SPACE_SIZE;
                                sk.snakeParts.push_back(temp);  //add new head to snakeParts                               
                                
                                temp.x = (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                                temp.y = (i*SCREEN_WIDTH) / GAME_SPACE_SIZE;
                                sk.snakeParts.push_back(temp);  //add old head to snakeParts

                                gameSpace[i][j] = gameSpace[i][j]+1; //age the current part

                            }
                        
                        break;

                        default:        //rest of the snake
                        //gameSpace[i][j] == sk.length? set to 0 : increment by one
                        /*  ~! This part of the code can be refactored !~   */ 
                            SDL_Rect temp;
                            if(!tailFlag)
                            {
                                //tailFlag = true;
                                if(gameSpace[i][j] >= sk.length)
                                {
                                    gameSpace[i][j] = 0;
                                    tailFlag = true;
                                }
                                else 
                                {
                                    gameSpace[i][j] = gameSpace[i][j] +1;

                                    
                                    temp.x = (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                                    temp.y = (i*SCREEN_WIDTH) / GAME_SPACE_SIZE;
                                    temp.w = SCREEN_WIDTH / GAME_SPACE_SIZE;
                                    temp.h = SCREEN_HEIGHT / GAME_SPACE_SIZE;
                                } 
                            }
                            else
                            {
                                gameSpace[i][j] = gameSpace[i][j] +1;

                                
                                temp.x = (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                                temp.y = (i*SCREEN_WIDTH) / GAME_SPACE_SIZE;
                                temp.w = SCREEN_WIDTH / GAME_SPACE_SIZE;
                                temp.h = SCREEN_HEIGHT / GAME_SPACE_SIZE;
                            }

                            /*
                            temp.x = (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE;
                            temp.y = (i*SCREEN_WIDTH) / GAME_SPACE_SIZE;
                            temp.w = SCREEN_WIDTH / GAME_SPACE_SIZE;
                            temp.h = SCREEN_HEIGHT / GAME_SPACE_SIZE;
                            */

                            sk.snakeParts.push_back(temp);
                        break;
                    }
                }
            }
            ///*
            headFlag = false;
            tailFlag = false;
            if(!findFood)
            {
                std::cerr << "ERROR: FOOD NOT FOUND" << std::endl;
                printGameSpace(gameSpace);
                std::cerr << std::endl;
                placeFood(gameSpace, sk.length);
                //return 1;
                findFood = true;
            } /*
            else
            {
                findFood = false;    
            }
            */
           findFood = false;
        }
        inputFlag = true;

        /*  Draw    */
        /*  Blank Screen*/
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        /*  Draw Food   */
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);   
        SDL_RenderFillRect(renderer, &food);

        /*  Draw Snake*/
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for(SDL_Rect rect : sk.snakeParts)
        {
            SDL_RenderFillRect(renderer, &rect);
        }   

        /*  Draw Lines  */
        //SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for(int i = 1; i < GAME_SPACE_SIZE; i++)
        {
            SDL_RenderDrawLine(renderer, (i*SCREEN_WIDTH) / GAME_SPACE_SIZE, 0, (i*SCREEN_WIDTH) / GAME_SPACE_SIZE, SCREEN_HEIGHT);
        }
        for(int j = 1; j < GAME_SPACE_SIZE; j++)
        {
            SDL_RenderDrawLine(renderer, 0, (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE, SCREEN_HEIGHT, (j*SCREEN_HEIGHT) / GAME_SPACE_SIZE);
        }

        SDL_RenderPresent(renderer);

        if(killFlag)
        {
            std::cout << "Game Over! \nScore: " << (sk.length*10) << std::endl;
            running = false;
        }
    }
    return 0;
}