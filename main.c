/* Randomly created maze using recursive backtracking.
 * Play as Tadhg, trying to get to mama
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

//constants used to represent the 4 directions from each node
const uint8_t Directions[] = {0x01, 0x02, 0x04, 0x08};        //N, E, S, W
//array of x, y pairs representing physical movement in the N, E, S, W directions
const int moveDir[] = {0,-1,1,0,0,1,-1,0};

//function prototypes (the maze creator function)
void boreFrom(uint8_t* maze, int mazeW, int mazeH, int x, int y);


//Main
int main(int args, char** argv)
{
    //maze dimensions in nodes
    const int mazeW = 25;
    const int mazeH = 25;

    //array to hold maze data
    uint8_t maze[mazeW*mazeH];

    //zero fill maze array
    int n,m;
    for(n=0; n<mazeW; ++n)
    {
        for(m=0; m<mazeH; ++m)
        {
            maze[mazeH*n+m] = 0x00;
        }
    }

    //starting position
    //srand(10); // comment line out for random behaviour

    //starting positions for maze creation
    int x, y;
    x = rand()%mazeW;
    y = rand()%mazeH;

    //recursive function that carves maze starting at the specified point
    boreFrom(&maze, mazeW, mazeH, x, y);

    //function to create a texture representation of the maze
    //initialise SDL, create window, get renderer and set to render to texture
    int textureW = 500;
    int textureH = 500;

    //SDL init
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );

    //Create window renderer and blank texture
    SDL_Window* window = SDL_CreateWindow("Tadhg Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, textureW, textureH, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
    SDL_Texture* mazeTexture = SDL_CreateTexture(renderer,SDL_GetWindowPixelFormat(window),SDL_TEXTUREACCESS_TARGET,textureW,textureH);

    //Set rendering target to blank texture and clear
    SDL_SetRenderTarget(renderer, mazeTexture);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);

    //draw outline of maze
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_Rect outerRect = {0,0,textureW,textureH};
    SDL_RenderDrawRect(renderer, &outerRect);

    //width and height in px of a node
    int nodeW = textureW/mazeW;
    int nodeH = textureH/mazeH;

    //drawing maze walls
    for(n=0; n<mazeW; ++n)
    {
        for(m=0; m<mazeH; ++m)
        {
            if(((maze[n*mazeH+m]&Directions[1])!=Directions[1])&&n!=mazeW-1) SDL_RenderDrawLine(renderer, (n+1)*nodeW, m*nodeH, (n+1)*nodeW, (m+1)*nodeH);
            if(((maze[n*mazeH+m]&Directions[2])!=Directions[2])&&m!=mazeH-1) SDL_RenderDrawLine(renderer, n*nodeW, (m+1)*nodeH, (n+1)*nodeW, (m+1)*nodeH);
        }
    }

    //reset render target to screen (default)
    SDL_SetRenderTarget(renderer, NULL);
    //render maze texture to screen
    SDL_RenderCopy(renderer, mazeTexture, NULL, NULL);
    //display renderer content
    SDL_RenderPresent(renderer);

    //SDL event loop
    bool quit = false;
    SDL_Event e;
    while( !quit )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
        }
    }

    //free memory allocated for maze
    SDL_DestroyTexture(mazeTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    mazeTexture=NULL;
    renderer=NULL;
    window=NULL;
    SDL_Quit();

    return 0;
}

//Maze carving function, uses recursive backtracking
void boreFrom(uint8_t* maze, int mazeW, int mazeH, int x, int y)
{
    static int c = 0;
    while((maze[mazeH*x+y]&0xF0)!=0xF0)
    {
        int dir = rand()%4;
        if((maze[mazeH*x+y]&(Directions[dir]<<4))!=(Directions[dir]<<4))
        {
            maze[mazeH*x+y] |= (Directions[dir]<<4);
            int nx = x + moveDir[dir*2];
            int ny = y + moveDir[dir*2+1];
            if(nx >= 0 && ny >= 0 && nx < mazeW && ny < mazeH && (maze[mazeH*nx+ny]&0x0F)==0x00)
            {
                ++c;
                printf("%.0f percent complete\n", (float)c/mazeH/mazeW*100);
                maze[mazeH*x+y] |= Directions[dir];
                int shift = dir < 2 ? 2 : -2; //necessary shift to get opposite direction of dir
                maze[mazeH*nx+ny] |= Directions[dir+shift];
                boreFrom(maze, mazeW, mazeH, nx, ny);
            }
        }
    }
}
