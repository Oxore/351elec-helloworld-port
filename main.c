#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// this is the color in rgb format,
// maxing out all would give you the color white,
// and it will be your text's color
const SDL_Color black = {0, 0, 0, 0};

Uint32 exit_timer_cb(Uint32 interval, void *param)
{
    (void) param;
    SDL_Event event;
    SDL_UserEvent userevent;

    /* In this example, our callback pushes an SDL_USEREVENT event
    into the queue, and causes our callback to be called again at the
    same interval: */

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = (void *)10;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
}

void bake_message(
        SDL_Renderer* renderer,
        TTF_Font* font,
        SDL_Surface** surface,
        SDL_Texture** texture,
        const char* text)
{
    SDL_FreeSurface(*surface);
    SDL_DestroyTexture(*texture);
    *surface = TTF_RenderText_Solid(font, text, black);
    *texture = SDL_CreateTextureFromSurface(renderer, *surface);
}

// https://gist.github.com/fschr/92958222e35a823e738bb181fe045274
int main(int argc, char * const argv[])
{
    (void) argc;
    (void) argv;
    printf("Hello, world!\n");
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        return 0;
    }
    SDL_CreateWindowAndRenderer(
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            0,
            &window,
            &renderer);
    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return 0;
    }

    // The object is not used but must be opened to have controller input events
    SDL_Joystick* joystick = SDL_JoystickOpen(0);

    Uint32 delay_ms = 5 * 1000; // 5 Seconds
    SDL_TimerID my_timer_id = SDL_AddTimer(delay_ms, exit_timer_cb, NULL);
    (void) my_timer_id;

    TTF_Init();

    //this opens a font style and sets a size
    TTF_Font* font = TTF_OpenFont("PixelEmulator-xq08.ttf", 24);
    if (font == NULL)
    {
        fprintf(stderr, "Failed to open font: %s\n", SDL_GetError());
        return 0;
    }

    // as TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first
    SDL_Surface* surfaceMessage =
        TTF_RenderText_Solid(font, "Hello, world!", black);

    // now you can convert it into a texture
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect Message_rect; //create a rect
    Message_rect.x = 100;  //controls the rect's x coordinate
    Message_rect.y = 100; // controls the rect's y coordinte
    Message_rect.w = surfaceMessage->w; // controls the width of the rect
    Message_rect.h = surfaceMessage->h; // controls the height of the rect

    // (0,0) is on the top left of the window/screen,
    // think a rect as the text's box,
    // that way it would be very simple to understand

    // Now since it's a texture, you have to put RenderCopy
    // in your game loop area, the area where the whole code executes

    // you put the renderer's name first, the Message,
    // the crop size (you can ignore this if you don't want
    // to dabble with cropping), and the rect which is the size
    // and coordinate of your texture
    SDL_RenderCopy(renderer, message, NULL, &Message_rect);

    SDL_Event event;
    while (1) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
            break;
        if (event.type == SDL_USEREVENT && (intptr_t)event.user.data1 == 10)
            break;
        if (event.type == SDL_JOYHATMOTION && event.jhat.value == SDL_HAT_UP) {
            bake_message(renderer, font, &surfaceMessage, &message, "Up");
            Message_rect.w = surfaceMessage->w;
            Message_rect.h = surfaceMessage->h;
        }
        SDL_Delay(10);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderFillRect(renderer, &Message_rect);
        SDL_RenderCopy(renderer, message, NULL, &Message_rect);
        SDL_RenderPresent(renderer);
    }

    // Don't forget to free your surface and texture
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);

    SDL_JoystickClose(joystick);

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
