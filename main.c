/* Randomly spawning maze for Tadhg to run around in
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

const uint8_t Directions[] = {0x01, 0x02, 0x04, 0x08};        //N, E, S, W
const uint8_t OppDirections[] = {0x04, 0x08, 0x01, 0x02};     //S, W, N, E
const int moveDir[] = {0,-1,1,0,0,1,-1,0};

//function prototypes
void boreFrom(uint8_t* maze, int mazeW, int mazeH, int x, int y);
bool allChecked(uint8_t node);

int main(int args, char** argv)
{
    const int mazeW = 10;
    const int mazeH = 10;

    uint8_t* maze = malloc(sizeof(uint8_t)*mazeW*mazeH);

    int n,m;
    for(n=0; n<mazeW; ++n)
    {
        for(m=0; m<mazeH; ++m)
        {
            maze[mazeH*n+m] = 0x00;
        }
    }

    //starting position
    srand(10); // comment line out for random behaviour

    int x, y;
    x = rand()%mazeW;
    y = rand()%mazeH;

    //recursive function that carves maze starting at the specified point
    boreFrom(maze, mazeW, mazeH, x, y);

    //function to create a texture representation of the maze
    //initialise SDL, create window, get renderer and set to render to texture
    int textureW = 640;
    int textureH = 640;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    SDL_Window* window = SDL_CreateWindow("Tadhg Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, textureW, textureH, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_TEXTUREACCESS_TARGET);
    SDL_Texture* mazeTexture = SDL_CreateTexture(renderer,SDL_GetWindowPixelFormat(window),SDL_TEXTUREACCESS_TARGET,textureW,textureH);

    SDL_SetRenderTarget(renderer, mazeTexture);
    //draw outline of maze
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer);
    SDL_Rect outerRect = {0,0,textureW,textureH};
    SDL_RenderDrawRect(renderer, &outerRect);

    int nodeW = textureW/mazeW;
    int nodeH = textureH/mazeH;

    int i,j;
    for(i=0; i<mazeW; ++i)
    {
        for(j=0; j<mazeH; ++j)
        {
            if(((maze[i*mazeH+j]&Directions[1])==Directions[1])&&i!=mazeW-1) SDL_RenderDrawLine(renderer, (i+1)*nodeW, j*nodeH, (i+1)*nodeW, (j+1)*nodeH);
            if(((maze[i*mazeH+j]&Directions[2])==Directions[2])&&j!=mazeH-1) SDL_RenderDrawLine(renderer, i*nodeW, (j+1)*nodeH, (i+1)*nodeW, (j+1)*nodeH);
        }
    }

    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, mazeTexture, NULL, NULL);

    SDL_RenderPresent(renderer);

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
    free(maze);
    SDL_DestroyTexture(mazeTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    mazeTexture=NULL;
    renderer=NULL;
    window=NULL;
    SDL_Quit();

    return 0;
}

//Uses
void boreFrom(uint8_t* maze, int mazeW, int mazeH, int x, int y)
{
    uint8_t node = maze[mazeH*x+y];
    static int c = 0;
    while((node&0xF0)!=0xF0)
    {
        int dir = rand()%4;
        if((node&(Directions[dir]<<4))!=(Directions[dir]<<4))
        {
            node |= (Directions[dir]<<4);
            int nx = x + moveDir[dir*2];
            int ny = y + moveDir[dir*2+1];
            if((maze[mazeH*nx+ny]&0x0F)==0x00 && nx >= 0 && ny >= 0 && nx < mazeW && ny < mazeH)
            {
                ++c;
                printf("%.0f percent complete\n", (float)c/mazeH/mazeW*100);
                node |= Directions[dir];
                maze[mazeH*nx+ny] |= OppDirections[dir];
                boreFrom(maze, mazeW, mazeH, nx, ny);
            }
        }
    }
}
