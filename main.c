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

//constants used to represent the 4 directions from each node
const unsigned char Directions[] = {0x01, 0x02, 0x04, 0x08};        //N, E, S, W
//array of x, y pairs representing physical movement in the N, E, S, W directions
const int moveDir[] = {0,-1,1,0,0,1,-1,0};

//function prototypes (the maze creator function)
void boreFrom(unsigned char* maze, int x, int y);
int BSFsolve(unsigned char *maze, int startX, int startY, int endX, int endY);

void testQ();

//maze dimensions in nodes
const int mazeW = 20;
const int mazeH = 20;

//Main
int main(int args, char** argv)
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

    //function to create a texture representation of the maze
    //initialise SDL, create window, get renderer and set to render to texture
    int textureW = 800;
    int textureH = 800;

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

    //Get Tadhg into the game
    IMG_Init(IMG_INIT_JPG);
    SDL_Surface* tadhgSurf = IMG_Load("./tadhg.jpg");
    SDL_Texture* tadhg = SDL_CreateTextureFromSurface(renderer, tadhgSurf);
    SDL_FreeSurface(tadhgSurf);
    tadhgSurf = NULL;

    //Get mama into the game
    SDL_Surface* mamaSurf = IMG_Load("./mama.jpg");
    SDL_Texture* mama = SDL_CreateTextureFromSurface(renderer, mamaSurf);
    SDL_FreeSurface(mamaSurf);
    mamaSurf = NULL;

    //Get complete into the game
    SDL_Surface* completeSurf = IMG_Load("./complete.jpg");
    SDL_Texture* complete = SDL_CreateTextureFromSurface(renderer, completeSurf);
    SDL_FreeSurface(completeSurf);
    completeSurf = NULL;

    //random start point
    int tadhgX = rand()%(mazeW/3);
    int tadhgY = rand()%(mazeH/3);
    int mamaX = (mazeW/2)+rand()%(mazeW/3);
    int mamaY = (mazeH/2)+rand()%(mazeH/3);

    //Computer solve, consider running this in parallel
    int noMoves = BSFsolve(&maze, tadhgX, tadhgY, mamaX, mamaY);
    printf("at least %i moves to finish\n", noMoves);

    SDL_Rect destTadhg = {tadhgX*nodeW+1, tadhgY*nodeH+1, nodeW-1, nodeH-1};
    SDL_RenderCopy(renderer, tadhg, NULL, &destTadhg);
    SDL_Rect destMama = {mamaX*nodeW+1, mamaY*nodeH+1, nodeW-1, nodeH-1};
    SDL_RenderCopy(renderer, mama, NULL, &destMama);

    //display renderer content
    SDL_RenderPresent(renderer);

    //SDL event loop
    bool quit = false;
    bool noUpdate = false;
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
            else if( e.type == SDL_KEYDOWN )
            {
                //Select surfaces based on key press
                switch( e.key.keysym.sym )
                {
                    case SDLK_UP:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[0]) tadhgY-=1;
                    break;

                    case SDLK_DOWN:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[2]) tadhgY+=1;
                    break;

                    case SDLK_LEFT:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[3]) tadhgX-=1;
                    break;

                    case SDLK_RIGHT:
                        if(maze[tadhgX*mazeH+tadhgY]&Directions[1]) tadhgX+=1;
                    break;

                    default:
                    break;
                }
            }
        }
        if(tadhgX==mamaX && tadhgY==mamaY)
        {
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, complete, NULL, NULL);
            noUpdate = true;
        }
        else if(!noUpdate)
        {
            SDL_RenderCopy(renderer, mazeTexture, NULL, NULL);
            SDL_Rect destTadhg = {tadhgX*nodeW+1, tadhgY*nodeH+1, nodeW-1, nodeH-1};
            SDL_RenderCopy(renderer, tadhg, NULL, &destTadhg);
            SDL_Rect destMama = {mamaX*nodeW+1, mamaY*nodeH+1, nodeW-1, nodeH-1};
            SDL_RenderCopy(renderer, mama, NULL, &destMama);
        }

        //display renderer content
        SDL_RenderPresent(renderer);
    }

    printf("This maze can be replayed using the seed: %i\n", seed);

    //free memory allocated for maze
    SDL_DestroyTexture(mazeTexture);
    SDL_DestroyTexture(tadhg);
    SDL_DestroyTexture(mama);
    SDL_DestroyTexture(complete);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    mazeTexture=NULL;
    tadhg=NULL;
    mama=NULL;
    complete=NULL;
    renderer=NULL;
    window=NULL;
    SDL_Quit();

    return 0;
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



//***functions for computer to solve maze***
//Queue to use for BFS search
#define ROOT_NODE_PARENT_DIRECTION -3
#define MAX_QUEUE_LENGTH 250

typedef struct Node
{
    int level;
    int parentDirection;
    int x, y;
} Node;

Node NULL_NODE = {-1, -1, -1, -1};

typedef struct Queue
{
    Node nodes[MAX_QUEUE_LENGTH];
    Node* head;
    Node* tail;
} Queue;

Queue Q_init()
{
    Queue q;
    q.head = NULL;
    q.tail = NULL;
    return q;
}

void Q_enqueue(Queue* q, Node n)
{
    if (q->head == NULL)
    {
        q->nodes[0] = n;
        q->head = &q->nodes[0];
        q->tail = &q->nodes[0];
    }
    else
    {
        q->tail++;
        if(q->tail > &q->nodes[MAX_QUEUE_LENGTH - 1]) q->tail = &q->nodes[0];
        if(q->tail != q->head)
        {
            *(q->tail) = n;
        }
        else
        {
            printf("Queue is full\n");
            q->tail--;
            if(q->tail < &q->nodes[0]) q->tail = &q->nodes[MAX_QUEUE_LENGTH - 1];
        }
    }
}

Node Q_dequeue(Queue* q)
{
    Node res = NULL_NODE;
    if(q->head != NULL) res = *(q->head);
    q->head++;
    if(q->head == q->tail+1 )
    {
        printf("Queue is emptied\n");
        q->head = NULL;
        q->tail = NULL;
    }
    return res;
}

bool Q_notEmpty(Queue* q)
{
    bool res = q->head == NULL ? false : true;
    return res;
}

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
