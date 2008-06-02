#include "tetris.h"
#include "sdlaux.h"
#include <windows.h>
#include <assert.h>

const char bricklayouts[][16] = {
    {1, 1, 1, 1, /* line hori */
     0, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {1, 0, 0, 0, /* line vert */
     1, 0, 0, 0,
     1, 0, 0, 0,
     1, 0, 0, 0},
    {1, 1, 0, 0, /* 7 #1 */
     0, 1, 0, 0,
     0, 1, 0, 0,
     0, 0, 0, 0},
    {0, 0, 1, 0, /* 7 #2 */
     1, 1, 1, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {1, 0, 0, 0, /* 7 #3 */
     1, 0, 0, 0,
     1, 1, 0, 0,
     0, 0, 0, 0},
    {1, 1, 1, 0, /* 7 #4 */
     1, 0, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {1, 1, 0, 0, /* 7_2 #1 */
     1, 0, 0, 0,
     1, 0, 0, 0,
     0, 0, 0, 0},
    {1, 1, 1, 0, /* 7_2 #2 */
     0, 0, 1, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {0, 1, 0, 0, /* 7_2 #3 */
     0, 1, 0, 0,
     1, 1, 0, 0,
     0, 0, 0, 0},
    {1, 0, 0, 0, /* 7_2 #4 */
     1, 1, 1, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {1, 1, 0, 0, /* z #1 */
     0, 1, 1, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {0, 1, 0, 0, /* z #2 */
     1, 1, 0, 0,
     1, 0, 0, 0,
     0, 0, 0, 0},
    {0, 1, 1, 0, /* z_2 #1 */
     1, 1, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {1, 0, 0, 0, /* z_2 #2 */
     1, 1, 0, 0,
     0, 1, 0, 0,
     0, 0, 0, 0},    
    {1, 1, 1, 0, /* T #1 */
     0, 1, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {0, 1, 0, 0, /* T #2 */
     1, 1, 0, 0,
     0, 1, 0, 0,
     0, 0, 0, 0},
    {0, 1, 0, 0, /* T #3 */
     1, 1, 1, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
    {1, 0, 0, 0, /* T #4 */
     1, 1, 0, 0,
     1, 0, 0, 0,
     0, 0, 0, 0},
    {1, 1, 0, 0, /* [+] */
     1, 1, 0, 0,
     0, 0, 0, 0,
     0, 0, 0, 0},
};

const char brickcolors[][3] = {
    {255, 255, 255}, /* line hori */     
    {255, 255, 255}, /* line vert */     
    {255, 0, 0}, /* 7 #1 */    
    {255, 0, 0}, /* 7 #2 */     
    {255, 0, 0}, /* 7 #3 */
    {255, 0, 0}, /* 7 #4 */
    {255, 0, 0}, /* 7_2 #1 */
    {255, 0, 0}, /* 7_2 #2 */
    {255, 0, 0}, /* 7_2 #3 */     
    {255, 0, 0}, /* 7_2 #4 */
    {0, 255, 0}, /* z #1 */
    {0, 255, 0}, /* z #2 */
    {0, 255, 0}, /* z_2 #1 */
    {0, 255, 0}, /* z_2 #2 */  
    {0, 0, 255}, /* T #1 */
    {0, 0, 255}, /* T #2 */
    {0, 0, 255}, /* T #3 */
    {0, 0, 255}, /* T #4 */
    {0, 255, 255}, /* [+] */
};

#define NUMBRICKTYPES DIM(bricklayouts)
#define ASSERTTYPE(type) assert(0 <= (type) && (type) < NUMBRICKTYPES)

typedef struct {
    int type;
    int x;
    int y;
    int row;
    int col;
} brick;
brick g_brick;

int cells[SCREENROW][SCREENCOL];

void initgame();
void uninitgame();
void drawbrick(SDL_Surface* surface, int type, int x, int y);
int updateinput(SDLKey key);
void drawframe(SDL_Surface* surface);
int chkground();

void spawnbrick();

#define drawbrick_rowcol(surface, type, row, col) drawbrick(surface, type, row*BRICKH, col*BRICKW)

int main(int argc, char* argv[])
{   
    SDL_Surface* screen;
    SDL_Event event;    
    int done;
    DWORD begin_time; /* use to control fps */
    DWORD left_time;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return -1;    

    atexit(SDL_Quit);

    if ((screen = SDL_SetVideoMode(SCREENW, SCREENH, 32, 0)) == NULL) 
        return -1;    

    initgame();

    /* main loop */
    done = 0;
    while (!done) {
        begin_time = GetTickCount();
        if(SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                done = 1;
                break;
            case SDL_KEYDOWN:
                done = updateinput(event.key.keysym.sym);
                break;            
            default:                
                ; /* FALLTHROUGH */
            }
        }        
        drawframe(screen);
        SDL_Flip(screen);        
        left_time = MSPF  + begin_time - GetTickCount();
        if (left_time > 0)
            Sleep(left_time);
    }
    return 0;
}

void initgame()
{
    ZeroMemory(&g_brick, sizeof(g_brick));
}

void uninitgame()
{
}

void drawbrick(SDL_Surface* surface, int type, int x, int y)
{
    int row, col;

    ASSERTTYPE(type);

    /* draw base */
    for (row=0; row<4; row++)
        for (col=0; col<4; col++)
            if (bricklayouts[type][row*4+col])
                SDL_FillRect_Fast(surface, x+col*BLOCKW, y+row*BLOCKH, BLOCKW, BLOCKH, brickcolors[type][0],brickcolors[type][1],brickcolors[type][2]);

    /* draw shadow */
    for (row=0; row<4; row++)
        for (col=0; col<4; col++)
            if (bricklayouts[type][row*4+col]) {
                SDL_LineV(surface, y+row*BLOCKH, y+(row+1)*BLOCKH, x+col*BLOCKW, 255, 255, 255);
                break;
            }    
    for (col=0; col<4; col++)  
        for (row=0; row<4; row++)
            if (bricklayouts[type][row*4+col]) {                 
                SDL_LineH(surface, x+col*BLOCKW, x+(col+1)*BLOCKW, y+row*BLOCKH, 255, 255, 255);                
                break;
            }    
}

int updateinput(SDLKey key)
{
    if (key == SDLK_ESCAPE)
        return 1;

    if (key == SDLK_UP)
        g_brick.type++;
    else if (key == SDLK_DOWN) {
        if (chkground()) 
            spawnbrick();
        else 
            g_brick.row++;        
    }
    else if (key == SDLK_LEFT) {
        /* chkleft(); */
    }
    else if (key == SDLK_RIGHT) {
        /* chkright(); */
    }

    return 0;
}

void drawframe(SDL_Surface* surface)
{      
    /* draw background */
    SDL_FillRect_Fast(surface, 0, 0, SCREENW, SCREENH, 0, 0, 0);

    drawbrick(surface, g_brick.type, g_brick.col*BLOCKW, g_brick.row*BLOCKH);
}

int chkground()
{
    int row, col;

    /* get the block under */
    for (col=3; col>=0; col--)
        for (row=3; row>=0; row--)
            if (bricklayouts[g_brick.type][row*4+col]) {                
                if (g_brick.row+1 > SCREENROW-1)
                    return 1;
                if (cells[g_brick.row+1][g_brick.col] >= 0)
                    return 1;                    
                break;
            }

    return 0;
}

void spawnbrick()
{
    g_brick.row = 0;
    g_brick.col = SCREENCOL/2-2;
    g_brick.type = rand() % NUMBRICKTYPES;
}