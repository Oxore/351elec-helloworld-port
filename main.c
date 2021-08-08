#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <inttypes.h>
#include <stdbool.h>

// SDL_JOYBUTTONDOWN(0x603) and SDL_JOYBUTTONUP(0x604).
// - Created when
//   - A, B, X, Y
//   - Start, Select,
//   - L1, R1, L2, R2, L3, R3 (yep, stick presses are here)
// - Button ID may be acquired from jbutton.button.
//
// SDL_JOYHATMOTION(0x602) created when D-pad pressed
// - Button ID may be acquired from jhat->value.
// - Button ID is basically just a bit and if every button is pressed then all
//   bits are set. Bits are SDL_HAT_(UP|DOWN|RIGHT|LEFT) from SDL_joystick.h.
//
// SDL_JOYAXISMOTION(0x600) created when L3 or R3 stick tilted.
// - Tilt value is int16 and may be acquired from jaxis.value.
// - Axis ID may be acquired from jaxis.axis.
//
// SDL_KEYDOWN(0x300) and SDL_KEYUP(0x301).
// - Created when computer keyboard key is pressed/released
// - Key ID may be acquired from key.keysym.sym (SDL_Keycode).
// - Key scancode may be acquired from key.keysym.scancode (SDL_Scancode).

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// this is the color in rgb format,
// maxing out all would give you the color white,
// and it will be your text's color
const SDL_Color black = {0, 0, 0, 0};

struct input_handl_ctx {
    bool l3;
    bool r3;
};

static inline void handle_input(
        struct input_handl_ctx* in, const SDL_Event* event)
{
    if (event->type == SDL_JOYBUTTONDOWN || event->type == SDL_JOYBUTTONUP) {
        bool pressed = event->type == SDL_JOYBUTTONDOWN;
        if (event->jbutton.button == 0x08)
            in->l3 = pressed;
        else if (event->jbutton.button == 0x09)
            in->r3 = pressed;
    }
}

static inline bool is_exit_state(const struct input_handl_ctx* in)
{
    return in->l3 == in->r3 && in->l3 == true;
}

Uint32 exit_timer_cb(Uint32 interval, void *param)
{
    (void) param;
    SDL_PushEvent(&(SDL_Event){ .type = SDL_QUIT });
    return interval;
}

void bake_event(
        SDL_Renderer* renderer,
        TTF_Font* font,
        SDL_Surface** surface,
        SDL_Texture** texture,
        const SDL_Event* event)
{
    char text[100] = {0};
    int size = snprintf(text, sizeof(text), "type=0x%" PRIX32, event->type);
            //event->jhat.value);
            //"jhat->value=0x%" PRIX32 ",\n",
            //"jaxis->value=0x%" PRIX32 ",\n",

    if (size >= 0 && (size_t)size <= sizeof(text))
    {
        if (event->type == SDL_JOYHATMOTION)
        {
            snprintf(
                    text + size,
                    sizeof(text) - size,
                    " d-pad button=0x%" PRIX8, event->jhat.value);
        }
        else if (event->type == SDL_JOYAXISMOTION)
        {
            snprintf(
                    text + size,
                    sizeof(text) - size,
                    " axis=%" PRIi32 " value=%" PRIi16,
                    event->jaxis.axis,
                    event->jaxis.value);
        }
        else if (event->type == SDL_JOYBUTTONDOWN || event->type == SDL_JOYBUTTONUP)
        {
            snprintf(
                    text + size,
                    sizeof(text) - size,
                    " button=0x%" PRIX8, event->jbutton.button);
        }
    }
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

    Uint32 delay_ms = 10 * 1000;
    SDL_TimerID my_timer_id = SDL_AddTimer(delay_ms, exit_timer_cb, NULL);

    TTF_Init();

    // Open a font style with specific font size
    TTF_Font* font = TTF_OpenFont("font-manaspc/manaspc.ttf", 12);
    if (font == NULL) {
        fprintf(stderr, "Failed to open font: %s\n", SDL_GetError());
        return 0;
    }

    // As TTF_RenderText_Solid could only be used on
    // SDL_Surface then you have to create the surface first
    SDL_Surface* message_surface =
        TTF_RenderText_Solid(font, "Hello, world!", black);

    // Convert the surface into a texture
    SDL_Texture* message =
        SDL_CreateTextureFromSurface(renderer, message_surface);

    // SDL_Rect is just a dimension parameter aggregation for render operation
    SDL_Rect message_rect = {
        .x = 100,
        .y = 100,
        .w = message_surface->w, // width of the rect
        .h = message_surface->h, // height of the rect
    };

    struct input_handl_ctx input = {0};
    bool should_exit = false;
    while (!should_exit) {
        SDL_Event event = (SDL_Event){0};
        while (SDL_PollEvent(&event)) {
            handle_input(&input, &event);
            if (event.type == SDL_QUIT || is_exit_state(&input))
                should_exit = true;

            bake_event(renderer, font, &message_surface, &message, &event);
            message_rect.w = message_surface->w;
            message_rect.h = message_surface->h;

            // Restart timeout exit timer
            SDL_RemoveTimer(my_timer_id);
            my_timer_id = SDL_AddTimer(delay_ms, exit_timer_cb, NULL);
        }

        SDL_Delay(10);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderFillRect(renderer, &message_rect);
        SDL_RenderCopy(renderer, message, NULL, &message_rect);
        SDL_RenderPresent(renderer);
    }

    SDL_FreeSurface(message_surface);
    SDL_DestroyTexture(message);
    TTF_CloseFont(font);
    SDL_JoystickClose(joystick);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
