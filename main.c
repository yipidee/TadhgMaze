/* Randomly created maze using recursive backtracking.
 * Play as Tadhg, trying to get to mama
**/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#ifdef __linux
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
#elif _WIN32
    #include <SDL.h>
    #include <SDL_image.h>
#endif
#include "TextLabel.h"
#include "Queue.h"

//constants used to represent the 4 directions from each node
const unsigned char Directions[] = {0x01, 0x02, 0x04, 0x08};        //N, E, S, W
//array of x, y pairs representing physical movement in the N, E, S, W directions
const int moveDir[] = {0,-1,1,0,0,1,-1,0};

//function prototypes (the maze creator function)
void boreFrom(unsigned char* maze, int x, int y);
//maze solver used in calculating score
int BSFsolve(unsigned char *maze, int startX, int startY, int endX, int endY);

typedef enum
{
    STATE_INTRO,
    STATE_PLAY,
    STATE_COMPLETE,
    STATE_QUIT
}GAME_STATE;

//maze dimensions in nodes
const int mazeW = 40;
const int mazeH = 30;
const int textureW = 800;
const int textureH = 600;

//functions equating to games states
GAME_STATE intro(void);
GAME_STATE play(void);
GAME_STATE complete(void);

//global variables
SDL_Window* window;
SDL_Renderer* renderer;
int noMoves;
int totalMoves;

int main(int args, char** argv)
{
    //SDL init
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );
    IMG_Init(IMG_INIT_JPG);

    //init SDL window and renderer
    window = SDL_CreateWindow("Tadhg Maze Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, textureW, textureH, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

    //finite state machine linking game states
    GAME_STATE (*FST[])(void) = {&intro, &play, &complete};

    //set current state to INTRO screen
    GAME_STATE currentState = STATE_INTRO;

    //run through states until quit state initiated
    while(currentState != STATE_QUIT)
    {
        currentState = FST[currentState]();
    }

    //games has ended, release resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    renderer = NULL;
    window = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}

GAME_STATE intro()
{
    //TODO: make intro screen
    return STATE_PLAY;
}

GAME_STATE play()
{
    //array to hold maze data
    unsigned char maze[mazeW*mazeH];

    //zero fill maze array
    int n,m;
    for(n=0; n<mazeW*mazeH; ++n) maze[n] = 0x00;

    //starting position
    int seed = time(NULL);
    srand(seed);

    //starting positions for maze creation
    int x, y;
    x = rand()%mazeW;
    y = rand()%mazeH;

    //recursive function that carves maze starting at the specified point
    boreFrom(&maze, x, y);

    //Create window renderer and blank texture
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

    //Get Tadhg into the game
    SDL_Surface* tadhgSurf = IMG_Load("./tadhg.jpg");
    SDL_Texture* tadhg = SDL_CreateTextureFromSurface(renderer, tadhgSurf);
    SDL_FreeSurface(tadhgSurf);
    tadhgSurf = NULL;

    //Get mama into the game
    SDL_Surface* mamaSurf = IMG_Load("./mama.jpg");
    SDL_Texture* mama = SDL_CreateTextureFromSurface(renderer, mamaSurf);
    SDL_FreeSurface(mamaSurf);
    mamaSurf = NULL;

    //random start point
    int tadhgX = rand()%(mazeW/3);
    int tadhgY = rand()%(mazeH/3);
    int mamaX = (mazeW/2)+rand()%(mazeW/3);
    int mamaY = (mazeH/2)+rand()%(mazeH/3);

    //Computer solve, consider running this in parallel
    noMoves = BSFsolve(&maze, tadhgX, tadhgY, mamaX, mamaY);
    printf("at least %i moves to finish\n", noMoves);

    SDL_Rect destTadhg = {tadhgX*nodeW+1, tadhgY*nodeH+1, nodeW-1, nodeH-1};
    SDL_RenderCopy(renderer, tadhg, NULL, &destTadhg);
    SDL_Rect destMama = {mamaX*nodeW+1, mamaY*nodeH+1, nodeW-1, nodeH-1};
    SDL_RenderCopy(renderer, mama, NULL, &destMama);

    //label to show total moves made
    TextLabel movesMadeLabel = TL_createTextLabel("Moves made: 0", 0, 0);
    TL_setFontSize(movesMadeLabel, 24);
    TL_setX(movesMadeLabel, textureW - TL_getWidth(movesMadeLabel)-25);
    TL_setY(movesMadeLabel, 10);
    TL_renderTextLabel(movesMadeLabel, renderer);
    char movesMadeText[18]; //used later to vary content of the above label

    //display renderer content
    SDL_RenderPresent(renderer);

    //SDL event loop
    bool quit = false;
    SDL_Event e;
    totalMoves = 0;
    while( true )
    {
        //Handle events on queue
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            else if( e.type == SDL_KEYDOWN )
            {
                //Select surfaces based on key press
                switch( e.key.keysym.sym )
                {
                    case SDLK_UP:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[0]) {tadhgY-=1; totalMoves++;}
                    break;

                    case SDLK_DOWN:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[2]) {tadhgY+=1; totalMoves++;}
                    break;

                    case SDLK_LEFT:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[3]) {tadhgX-=1; totalMoves++;}
                    break;

                    case SDLK_RIGHT:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[1]) {tadhgX+=1; totalMoves++;}
                    break;

                    case SDLK_ESCAPE:
                        quit = true;

                    default:
                    break;
                }
            }
        }

        if(quit || (tadhgX==mamaX && tadhgY==mamaY))
        {
            //free memory allocated for maze
            SDL_DestroyTexture(mazeTexture);
            SDL_DestroyTexture(tadhg);
            SDL_DestroyTexture(mama);
            mazeTexture=NULL;
            tadhg=NULL;
            mama=NULL;
            TL_destroyTextLabel(movesMadeLabel);
            GAME_STATE retVal = quit ? STATE_QUIT : STATE_COMPLETE;
            return retVal;
        }
        SDL_RenderCopy(renderer, mazeTexture, NULL, NULL);
        SDL_Rect destTadhg = {tadhgX*nodeW+1, tadhgY*nodeH+1, nodeW-1, nodeH-1};
        SDL_RenderCopy(renderer, tadhg, NULL, &destTadhg);
        SDL_Rect destMama = {mamaX*nodeW+1, mamaY*nodeH+1, nodeW-1, nodeH-1};
        SDL_RenderCopy(renderer, mama, NULL, &destMama);

        snprintf(movesMadeText, 18, "Moves made: %i", totalMoves);
        TL_setText(movesMadeLabel, movesMadeText);
        TL_renderTextLabel(movesMadeLabel, renderer);

        //display renderer content
        SDL_RenderPresent(renderer);
    }
}

GAME_STATE complete()
{
    //Get complete into the game
    SDL_Surface* completeSurf = IMG_Load("./complete.jpg");
    SDL_Texture* complete = SDL_CreateTextureFromSurface(renderer, completeSurf);
    SDL_FreeSurface(completeSurf);
    completeSurf = NULL;

    //Get congrats text texture
    TextLabel completion = TL_createTextLabel("Tadhg's safe! Yeah!", 0, 0);
    TL_setFontSize(completion, 64);
    TL_setX(completion, (textureW-TL_getWidth(completion))/2);
    TL_setY(completion, (textureH-TL_getHeight(completion))/6);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, complete, NULL, NULL);
    TL_renderTextLabel(completion, renderer);

    //label to write score to screen
    char score[50];
    snprintf(score, 50, "Your score this time: %.0f/100", 100*(float)noMoves/(float)totalMoves);
    TextLabel scoreLabel = TL_createTextLabel(score, 0, 0);
    TL_setFontSize(scoreLabel, 42);
    TL_setX(scoreLabel, (textureW-TL_getWidth(scoreLabel))/2);
    TL_setY(scoreLabel, (textureH-TL_getHeight(scoreLabel))/2);
    TL_renderTextLabel(scoreLabel, renderer);

    TextLabel quitMessage = TL_createTextLabel("Press 'Q' to quit", 0, 0);
    TextLabel contMessage = TL_createTextLabel("Press Enter to play new Maze", 0, 0);
    TL_setFontSize(quitMessage, 38);
    TL_setFontSize(contMessage, 38);
    TL_setX(quitMessage, (textureW-TL_getWidth(quitMessage))/2);
    TL_setY(quitMessage, 7*(textureH-TL_getHeight(quitMessage))/9);
    TL_setX(contMessage, (textureW-TL_getWidth(contMessage))/2);
    TL_setY(contMessage, TL_getY(quitMessage)+TL_getHeight(quitMessage));

    TL_renderTextLabel(quitMessage, renderer);
    TL_renderTextLabel(contMessage, renderer);

    //render the complete maze screen
    SDL_RenderPresent(renderer);

    //all resources can be destroyed since already rendered
    SDL_DestroyTexture(complete);
    complete = NULL;
    TL_destroyTextLabel(completion);
    TL_destroyTextLabel(scoreLabel);
    TL_destroyTextLabel(quitMessage);
    TL_destroyTextLabel(contMessage);
    completion = NULL;
    scoreLabel = NULL;
    quitMessage = NULL;
    contMessage = NULL;

    GAME_STATE choice;
    bool choiceMade = false;
    while( !choiceMade )
    {
        //Handle events on queue
        SDL_Event e;
        while( SDL_PollEvent( &e ) != 0 )
        {
            //User requests quit
            if( e.type == SDL_QUIT )
            {
                choiceMade = true;
                choice = STATE_QUIT;
            }
            else if( e.type == SDL_KEYDOWN )
            {
                //Select surfaces based on key press
                switch( e.key.keysym.sym )
                {
                    case SDLK_q:
                        choiceMade = true;
                        choice = STATE_QUIT;
                        break;

                    case SDLK_RETURN:
                        choiceMade = true;
                        choice = STATE_PLAY;
                        break;

                    default:
                    break;
                }
            }
        }
    }
    return choice;
}

//Maze carving function, uses recursive backtracking
void boreFrom(unsigned char* maze, int x, int y)
{
    //static int c = 0;
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
                //++c;
                //printf("%.0f percent complete\n", (float)c/mazeH/mazeW*100);
                maze[mazeH*x+y] |= Directions[dir];
                int shift = dir < 2 ? 2 : -2; //necessary shift to get opposite direction of dir
                maze[mazeH*nx+ny] |= Directions[dir+shift];
                boreFrom(maze, nx, ny);
            }
        }
    }
}

/*********************************************
***functions used by computer to solve maze***
*********************************************/

bool isNodeAtXY(Node n, int x, int y)
{
    bool res = ((n.x == x) && (n.y == y)) ? true : false;
    return res;
}

void enqueueChildren(Queue* q, unsigned char* maze, Node n)
{
    unsigned char pos = maze[n.x*mazeH + n.y];
    int i;
    for(i = 0; i<4 ; ++i)
    {
        if(((pos&Directions[i])==Directions[i]) && (i!=n.parentDirection))
        {
            Node child;
            child.level = n.level + 1;
            child.parentDirection = i < 2 ? i + 2 : i - 2;
            child.x = n.x;
            child.y = n.y;
            if(i==0) child.y--;
            if(i==1) child.x++;
            if(i==2) child.y++;
            if(i==3) child.x--;
            Q_enqueue(q, child);
        }
    }
}

int BSFsolve(unsigned char *maze, int startX, int startY, int endX, int endY)
{
    Node root;
    root.level = 0;
    root.parentDirection = ROOT_NODE_PARENT_DIRECTION;
    root.x=startX;
    root.y=startY;

    Queue q = Q_init();
    Q_enqueue(&q, root);

    while(Q_notEmpty(&q))
    {
        Node n = Q_dequeue(&q);
        if(!isNodeAtXY(n, NULL_NODE.x, NULL_NODE.y))
        {
            if(isNodeAtXY(n, endX, endY))
            {
                return n.level;
            }else
            {
                enqueueChildren(&q, maze, n);
            }
        }
    }
    return -1;
}
