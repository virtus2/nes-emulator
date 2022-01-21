#ifndef VTS_DEF
#define VTS_DEF

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <cstdio>
#include <sstream>
#include <chrono>
#include <string>
#include <vector>
/*
*  Constants
*/
namespace vts
{
    class Engine;
    class Sprite;
    const SDL_Color
        GREY{ 192,192,192 }, DARK_GREY{ 128,128,128 }, VERY_DARK_GREY{ 64,64,64 },
        RED{ 255, 0, 0 }, DARK_RED{ 128, 0, 0 }, VERY_DARK_RED{ 64, 0, 0 },
        YELLOW{ 255, 255, 0 }, DARK_YELLOW{ 128, 128, 0 }, VERY_DARK_YELLOW{ 64, 64, 0 },
        GREEN{ 0, 255, 0 }, DARK_GREEN{ 0, 128, 0 }, VERY_DARK_GREEN{ 0, 64, 0 },
        CYAN{ 0, 255, 255 }, DARK_CYAN{ 0, 128, 128 }, VERY_DARK_CYAN{ 0, 64, 64 },
        BLUE{ 0, 0, 255 }, DARK_BLUE{ 0, 0, 128 }, VERY_DARK_BLUE{ 0, 0, 64 },
        MAGENTA{ 255, 0, 255 }, DARK_MAGENTA{ 128, 0, 128 }, VERY_DARK_MAGENTA{ 64, 0, 64 },
        WHITE{ 255, 255, 255 }, BLACK{ 0, 0, 0 }, BLANK{ 0, 0, 0, 0 };

    constexpr uint8_t nMouseButtons = 5;
    constexpr uint8_t nDefaultAlpha = 0xFF;
    constexpr uint32_t nDefaultPixel = (nDefaultAlpha << 24);

    struct HWButton
    {
        bool bPressed = false;
        bool bReleased = false;
        bool bHeld = false;
    };

    class Sprite
    {
        public:
        Sprite();
        ~Sprite();
        Sprite(int32_t w, int32_t h);

        public:
        int32_t width = 0;
        int32_t height = 0;
        SDL_Surface* surface = NULL;


        public:
        bool SetPixel(int32_t x, int32_t y, SDL_Color p);
    };

    class Engine
    {
        public:
        Engine() {}
        ~Engine() {}
        std::string sAppName = "NES Emulator";


        public:
        bool Init(int screen_w, int screen_h);
        void Start();

        public:
        virtual bool OnUserCreate();
        virtual bool OnUserUpdate(float fElapsedTime);

        public:
        // Drawing
        void DrawString(int32_t x, int32_t y, const std::string& sText, SDL_Color color = WHITE, uint32_t scale=1);
        void DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale = 1);
        void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, SDL_Color p);
        void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, SDL_Color p = WHITE);
        void Clear(SDL_Color color);

        public:
        // Utility

        public: // Hardware Interfaces
        HWButton GetKey(SDL_Scancode key) const;

        private:
        SDL_Window* window = NULL;
        SDL_Surface* screenSurface = NULL;
        SDL_Renderer* renderer = NULL;
        TTF_Font* font = NULL;

        private:
        std::vector<int> vScreenSize = { 256,240 };

        // Keyboard
        bool pKeyNewState[256] = { 0 };
        bool pKeyOldState[256] = { 0 };
        HWButton pKeyboardState[256] = { 0 };
        // Mouse
        bool pMouseNewState[nMouseButtons] = { 0 };
        bool pMouseOldState[nMouseButtons] = { 0 };
        HWButton pMouseState[nMouseButtons] = { 0 };
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
        void HandleSystemEvent();
    };
}
#endif // VTS_DEF

//#define VTS_ENGINE

#ifdef VTS_ENGINE
#undef VTS_ENGINE
namespace vts{
	// O------------------------------------------------------------------------------O
	// | vts::Sprite IMPLEMENTATION                                                   |
	// O------------------------------------------------------------------------------O
    Sprite::Sprite()
    {
        width = height = 0;
        surface = NULL;
    }
    Sprite::~Sprite()
    {
        SDL_FreeSurface(surface);
    }
    Sprite::Sprite(int32_t w, int32_t h)
    {
        Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
#else 
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
#endif
        width = w, height = h;
        // the depth of the surface in bits = 32
        // (Bytes per Pixel = 4)
        surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, rmask, gmask, bmask, amask);
        if (surface == NULL)    
        {
            printf("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
            return;
        }

    }

    bool Sprite::SetPixel(int32_t x, int32_t y, SDL_Color p)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
        {
            Uint32* pixels = (Uint32*)surface->pixels;
            // SDL stores it's pixel color in a Uint32
            // R 0000 0000
            // G 0000 0000
            // B 0000 0000
            // A 0000 0000
            Uint32 color = SDL_MapRGBA(surface->format, p.r, p.g, p.b, p.a);
            pixels[y * width + x] = color;
            return true;
        }
        else
            return false;
    }

    // O------------------------------------------------------------------------------O
    // | vts::Engine IMPLEMENTATION                                                   |
    // O------------------------------------------------------------------------------O
    bool Engine::Init(int screen_w, int screen_h)
    {
        bool success = true;
        vScreenSize = { screen_w, screen_h };
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
                    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                }
                if (TTF_Init() == -1)
                {
                    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    success = false;
                }
                font = TTF_OpenFont("Pixel_NES.ttf", 10);
                if (font == NULL)
                {
                    printf("Failed to load font! SDL_ttf Error:%s\n", TTF_GetError());
                    success = false;
                }
            }
        }
        return success;
    }
    void Engine::Start()
    {
        isRunning = true;
        EngineThread();
    }

    bool Engine::OnUserCreate()
    {
        return false;
    }
    bool Engine::OnUserUpdate(float fElapsedTime)
    {
        return false;
    }
    void Engine::PrepareEngine()
    {
        m_tp1 = std::chrono::system_clock::now();
        m_tp2 = std::chrono::system_clock::now();
    }

    void Engine::EngineThread()
    {
        PrepareEngine();
        if (!OnUserCreate()) isRunning = false;
        while (isRunning)
        {
            while (isRunning) { Update(); }

            //if(OnUserDestroy)
        }
    }

    void Engine::HandleSystemEvent()
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            pKeyNewState[event.key.keysym.scancode] = true;
            break;
        case SDL_KEYUP:
            pKeyNewState[event.key.keysym.scancode] = false;
            break;
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
        HandleSystemEvent();
        auto ScanHardware = [&](HWButton* pKeys, bool* pStateOld, bool* pStateNew, uint32_t nKeyCount) {
            for (uint32_t i = 0; i < nKeyCount; i++)
            {
                pKeys[i].bPressed = false;
                pKeys[i].bReleased = false;
                if (pStateNew[i] != pStateOld[i])
                {
                    if (pStateNew[i])
                    {
                        pKeys[i].bPressed = !pKeys[i].bHeld;
                        pKeys[i].bHeld = true;
                    }
                    else
                    {
                        pKeys[i].bReleased = true;
                        pKeys[i].bHeld = false;
                    }
                }
                pStateOld[i] = pStateNew[i];
            }
        };
        ScanHardware(pKeyboardState, pKeyOldState, pKeyNewState, 256);
        // handle frame update
        if (!OnUserUpdate(fElapsedTime)) isRunning = false;
        // display frame
        // present graphics to screen

        SDL_RenderPresent(renderer);
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

    void Engine::DrawString(int32_t x, int32_t y, const std::string& sText, SDL_Color color, uint32_t scale)
    {
        SDL_Surface* surface = TTF_RenderText_Solid(font, sText.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst{ x, y, surface->w * scale, surface->h *scale};
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void Engine::DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale)
    {
        if (sprite == nullptr) return;
        SDL_Surface* surface = SDL_ConvertSurfaceFormat(sprite->surface, SDL_GetWindowPixelFormat(window), 0);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dst{ x, y, sprite->width * scale, sprite->height * scale };
        SDL_RenderCopy(renderer, texture, NULL, &dst);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void Engine::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, SDL_Color p)
    {
        SDL_Rect rect{ x,y,w,h };
        SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.a);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }

    void Engine::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, SDL_Color p)
    {
        SDL_Rect rect{ x,y,w,h };
        SDL_SetRenderDrawColor(renderer, p.r, p.g, p.b, p.a);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }

    void Engine::Clear(SDL_Color color)
    {
        // Clear screen
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }

    HWButton Engine::GetKey(SDL_Scancode k) const
    {
        return pKeyboardState[k];
    }
}
#endif // VTS_ENGINE