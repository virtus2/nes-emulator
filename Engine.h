#pragma once
#include <SDL.h>
#include <cstdio>
#include <sstream>
#include <chrono>
#include <string>
class Engine
{
    public:
    Engine() {}
    ~Engine() {}
    std::string sAppName = "NES Emulator";

    public:
    bool Init(int screen_w, int screen_h);
    void Start();

    private:
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event; // event handler

    private:
    bool isRunning = false;
    std::chrono::time_point<std::chrono::system_clock> m_tp1, m_tp2;
    float fLastElapsed = 0.0f;
    float fFrameTimer = 1.0f;
    int nFrameCount = 0;
    uint32_t nLastFPS = 0;

    private:
    void PrepareEngine();
    void EngineThread();
    void Update();
};
bool Engine::Init(int screen_w, int screen_h)
{
    bool success = true;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("SDL could not initialize!: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        window = SDL_CreateWindow(sAppName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_w, screen_h, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created!: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Crate renderer for window
            renderer = SDL_CreateRenderer(window, -1, NULL);
            if (renderer == NULL)
            {
                printf("Renderer could not be created!: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                // Initialize renderer color
                SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
            }

        }
    }
    return success;
}
void Engine::Start()
{
    isRunning = true;
    EngineThread();
    //std::thread t = std::thread(&Engine::EngineThread, this);
    //t.join();
}
void Engine::PrepareEngine()
{
    m_tp1 = std::chrono::system_clock::now();
    m_tp2 = std::chrono::system_clock::now();
}

void Engine::EngineThread()
{
    PrepareEngine();
    while (isRunning)
    {
        while (isRunning) { Update(); }

        //if(OnUserDestroy)
    }
}
    
void Engine::Update()
{
    m_tp2 = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedTime = m_tp2 - m_tp1;
    m_tp1 = m_tp2;

    float fElapsedTime = elapsedTime.count();
    fLastElapsed = fElapsedTime;
    // scan input

    // frame update

    // display frame

    // present graphics to screen

    // update title bar
    fFrameTimer += fElapsedTime;
    nFrameCount++;
    if (fFrameTimer >= 1.0f)
    {
        nLastFPS = nFrameCount;
        fFrameTimer -= 1.0f;
        std::string title = sAppName + " FPS: " + std::to_string(nFrameCount);
        SDL_SetWindowTitle(window, title.c_str());
        nFrameCount = 0;
    }
}
