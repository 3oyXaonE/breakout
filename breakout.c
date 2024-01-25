#include <stdio.h>
#include <SDL2/SDL.h>

#define FALSE 0
#define TRUE 1
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 720
#define PLATFORM_WIDTH 80
#define PLATFORM_HEIGHT 10
#define FPS 120


SDL_Window* mWindow = NULL;
SDL_Renderer* mRenderer = NULL;
int isRunning = 0;
int xdir = 0;
int ydir = -1;
int speed = 500 / FPS;
int prevRenderTime;

struct platform {
    int x;
    int y;
    int width;
    int height;
} platform;

struct ball {
    int x;
    int y;
    int width;
    int height;
} ball;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} brick;

brick *arr;
int nob;
int horizontal_distance = 10;
int vertical_distance = 8;
int width = 26;
int height = 12;
int score = 0;
int col;
int row;
int str;

void initializeSDL() {
    SDL_Init(SDL_INIT_EVERYTHING);
    mWindow = SDL_CreateWindow(
    "Breakout",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH,
    WINDOW_HEIGHT,
    0);
    mRenderer = SDL_CreateRenderer(mWindow, -1, 0);
}

void setupBricks() {
    col = (WINDOW_WIDTH - horizontal_distance) / (width + horizontal_distance);
    row = (WINDOW_HEIGHT / 5) / (height + vertical_distance);
    printf("col: %d\nrow: %d\n", col, row);
    nob = col * row;
    arr = malloc(nob * sizeof(brick));;
    str = ((WINDOW_WIDTH % ((col * (width + horizontal_distance))- horizontal_distance)) / 2);
    for (int i = 0; i < col; i++) {
        for (int k = 0; k < row; k++) {
            brick b = {str + i * (width + horizontal_distance), vertical_distance + k * (height + vertical_distance), width, height };
            arr[k*col + i] = b;
        }
    }
}

int hitsBrick() {
    int hitc = (ball.x - str) % (width + horizontal_distance);
    int hitr = ball.y % (height + vertical_distance);
    int c = (ball.x - str) / (width + horizontal_distance);
    int r = ball.y / (height + vertical_distance);
    if (hitc < width) {
        if (hitr > vertical_distance) {
            return r*col + c;
        }
    }
    if (hitc < width + horizontal_distance - ball.width) {
        if (hitr > vertical_distance) {
            return r*col + ++c;
        }
    }
    return -1;
}

void setup() {
    platform.x = (WINDOW_WIDTH - PLATFORM_WIDTH) / 2;
    platform.y = WINDOW_HEIGHT - 40;
    platform.width = PLATFORM_WIDTH;
    platform.height = PLATFORM_HEIGHT;

    ball.width = 10;
    ball.height = ball.width;
    ball.x = (WINDOW_WIDTH - ball.width) / 2;
    ball.y = WINDOW_HEIGHT - 40;

    isRunning = TRUE;
    prevRenderTime = SDL_GetTicks();
}

void getInput() {
    SDL_Event event;
    SDL_PollEvent(&event);
    const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
     
    switch(event.type) {
        case SDL_QUIT:
            isRunning = FALSE;
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym) {
                int s;
                case SDLK_LEFT:
                    s = keyboardState[SDL_SCANCODE_RIGHT] || !keyboardState[SDL_SCANCODE_LEFT] ? 30:15;
                    if (platform.x >= s) {
                        platform.x -= s;
                    } else {
                        platform.x = 0;
                    }
                    break;
                    
                case SDLK_RIGHT:
                    s = keyboardState[SDL_SCANCODE_LEFT] || !keyboardState[SDL_SCANCODE_RIGHT] ? 30:15;
                    if (platform.x < WINDOW_WIDTH - PLATFORM_WIDTH - s) {
                        platform.x += s;
                    } else {
                        platform.x = WINDOW_WIDTH - PLATFORM_WIDTH - 1;
                    }
                    break;
                case SDLK_ESCAPE:
                    isRunning = FALSE;
                    break;
            }
    }
}

int hitsPlatform() {
    return ((ball.y == platform.y) && (ball.x < (platform.x + platform.width)) && (ball.x >= platform.x));
}

int hitsWall() {
    return (ball.x < 0) || (ball.x >= WINDOW_WIDTH);
}

int hitsCeiling() {
    return ball.y < 0;
}

int hitsPit() {
    return ball.y >= WINDOW_HEIGHT;
}



int updateState() {
    ball.x += speed*xdir;
    ball.y += speed*ydir;

    if (ball.y < WINDOW_HEIGHT / 5) {
        int i = hitsBrick();
        if (i == -1) return 0;
        ydir *= -1;
        arr[i].x = -420;
        if (++score == nob) isRunning = FALSE;
        return 0;
    }

    if (hitsPlatform() == TRUE) {
        ydir *= -1;
        xdir = (ball.x < ((platform.x + platform.width) / 2)) ? -1:1;
        return 0;
    }

    if (hitsWall() == TRUE) {
        xdir *= -1;
        return 0;
    }

    if (hitsCeiling() == TRUE) {
        ydir = 1;
        return 0;
    }

    if (hitsPit() == TRUE) isRunning = FALSE;

    return 0;
}


void renderState() {
    SDL_SetRenderDrawColor(mRenderer, 30, 0, 50, 255);
    SDL_RenderClear(mRenderer);

    for (int i = 0; i < nob; i++) {
        brick tmp = arr[i];
        if (tmp.x == -420) continue;
        SDL_FRect rectOfBrick = {
        tmp.x,
        tmp.y,
        tmp.width,
        tmp.height
        };
        SDL_SetRenderDrawColor(mRenderer, 200, 200, 200, 255);
        SDL_RenderFillRectF(mRenderer, &rectOfBrick);
    }

    SDL_FRect rectOfPlatform = {
        platform.x,
        platform.y,
        platform.width,
        platform.height
    };
    SDL_SetRenderDrawColor(mRenderer, 0, 200, 80, 255);
    SDL_RenderFillRectF(mRenderer, &rectOfPlatform);

    SDL_FRect rectOfBall = {
        ball.x,
        ball.y,
        ball.width,
        ball.height
    };
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
    SDL_RenderFillRectF(mRenderer, &rectOfBall);

    SDL_RenderPresent(mRenderer);
}

void renderVictory() {
    
}

void shutdownSDL() {
    SDL_DestroyRenderer(mRenderer);
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void cleanup() {
    free(arr);
}

int main() {
    initializeSDL();
    setup();
    setupBricks();
    while(isRunning) {
        if (SDL_GetTicks() - prevRenderTime < 10) SDL_Delay(10 - (SDL_GetTicks() - prevRenderTime));
        getInput();
        prevRenderTime = SDL_GetTicks();
        updateState();
        renderState();
    }
    if (score == nob) renderVictory();
    shutdownSDL();
    cleanup();
    return 0;
}



