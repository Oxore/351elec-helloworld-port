#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
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
    bool dpad_right;
    bool dpad_up;
    bool dpad_down;
    bool dpad_left;
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

    if (event->type == SDL_JOYHATMOTION) {
        in->dpad_right = event->jhat.value & SDL_HAT_RIGHT;
        in->dpad_up = event->jhat.value & SDL_HAT_UP;
        in->dpad_down = event->jhat.value & SDL_HAT_DOWN;
        in->dpad_left = event->jhat.value & SDL_HAT_LEFT;
    }
}

static inline void handle_btn(
        SDL_Rect* sprite_src, const SDL_Event* event, uint8_t id)
{
    if (event->type == SDL_JOYBUTTONDOWN && event->jbutton.button == id)
        sprite_src->x += sprite_src->w;
    else if (event->type == SDL_JOYBUTTONUP && event->jbutton.button == id)
        sprite_src->x -= sprite_src->w;
}

static inline void handle_btn_a(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 0);
}

static inline void handle_btn_b(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 1);
}

static inline void handle_btn_x(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 2);
}

static inline void handle_btn_y(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 3);
}

static inline void handle_btn_start(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 6);
}

static inline void handle_btn_select(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 7);
}

static inline void handle_btn_l1(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 4);
}

static inline void handle_btn_l2(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 10);
}

static inline void handle_btn_r1(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 5);
}

static inline void handle_btn_r2(
        SDL_Rect* sprite_src, const SDL_Event* event)
{
    handle_btn(sprite_src, event, 11);
}

static inline void handle_stick(
        SDL_Rect* plane_src,
        SDL_Rect* stick_dst,
        const SDL_Rect* stick_dst_init,
        uint8_t axis_id,
        uint8_t btn_id,
        bool inverted,
        const SDL_Event* event)
{
    if (event->type == SDL_JOYBUTTONDOWN && event->jbutton.button == btn_id)
        plane_src->x += plane_src->w;
    else if (event->type == SDL_JOYBUTTONUP && event->jbutton.button == btn_id)
        plane_src->x -= plane_src->w;

    if (event->type == SDL_JOYAXISMOTION) {
        uint32_t value = (event->jaxis.value / 1024);
        value = inverted ? -value : value;
        if (event->jaxis.axis == axis_id) {
            // Horizontal
            stick_dst->x = value + stick_dst_init->x;
        } else if (event->jaxis.axis == axis_id + 1) {
            // Vertical
            stick_dst->y = value + stick_dst_init->y;
        }
    }
}

static inline void handle_left_stick(
        SDL_Rect* plane_src,
        SDL_Rect* stick_dst,
        const SDL_Rect* stick_dst_init,
        const SDL_Event* event)
{
    handle_stick(plane_src, stick_dst, stick_dst_init, 0, 0x08, true, event);
}

static inline void handle_right_stick(
        SDL_Rect* plane_src,
        SDL_Rect* stick_dst,
        const SDL_Rect* stick_dst_init,
        const SDL_Event* event)
{
    handle_stick(plane_src, stick_dst, stick_dst_init, 2, 0x09, false, event);
}

static inline bool is_exit_state(const struct input_handl_ctx* in)
{
    return in->l3 == in->r3 && in->l3 == true;
}

static inline bool is_dpad_right(const struct input_handl_ctx* in)
{
    return in->dpad_right;
}

static inline bool is_dpad_up(const struct input_handl_ctx* in)
{
    return in->dpad_up;
}

static inline bool is_dpad_down(const struct input_handl_ctx* in)
{
    return in->dpad_down;
}

static inline bool is_dpad_left(const struct input_handl_ctx* in)
{
    return in->dpad_left;
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
            SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
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
        fprintf(stderr, "Failed to open the font: %s\n", SDL_GetError());
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
        .y = 50,
        .w = message_surface->w, // width of the rect
        .h = message_surface->h, // height of the rect
    };

    // Open the sprite atlas
    SDL_Surface* atlas_surface = IMG_Load("controls.png");
    if (atlas_surface == NULL) {
        fprintf(stderr, "Failed to open the image: %s\n", SDL_GetError());
        return 0;
    }

    // Convert the surface into a texture
    SDL_Texture* atlas =
        SDL_CreateTextureFromSurface(renderer, atlas_surface);

    const SDL_Rect buttons_dst = { .x = 300, .y = 150, .w = 0, .h = 0, };

    SDL_Rect btn_a_src = { .x = 0, .y = 0, .w = 15, .h = 15, };
    SDL_Rect btn_a_dst = {
        .x = buttons_dst.x + 44,
        .y = buttons_dst.y + 22,
        .w = 30,
        .h = 30,
    };

    SDL_Rect btn_b_src = { .x = 30, .y = 0, .w = 15, .h = 15, };
    SDL_Rect btn_b_dst = {
        .x = buttons_dst.x + 22,
        .y = buttons_dst.y + 44,
        .w = 30,
        .h = 30,
    };

    SDL_Rect btn_x_src = { .x = 0, .y = 15, .w = 15, .h = 15, };
    SDL_Rect btn_x_dst = {
        .x = buttons_dst.x + 22,
        .y = buttons_dst.y,
        .w = 30,
        .h = 30,
    };

    SDL_Rect btn_y_src = { .x = 30, .y = 15, .w = 15, .h = 15, };
    SDL_Rect btn_y_dst = {
        .x = buttons_dst.x,
        .y = buttons_dst.y + 22,
        .w = 30,
        .h = 30,
    };

    SDL_Rect btn_start_src = { .x = 60, .y = 0, .w = 10, .h = 10, };
    SDL_Rect btn_start_dst = { .x = 300, .y = 120, .w = 20, .h = 20, };

    SDL_Rect btn_select_src = btn_start_src;
    SDL_Rect btn_select_dst = { .x = 150, .y = 120, .w = 20, .h = 20, };

    SDL_Rect btn_dpad_src = { .x = 0, .y = 30, .w = 37, .h = 37, };
    SDL_Rect btn_dpad_right_src = { .x = 37, .y = 30, .w = 37, .h = 37, };
    SDL_Rect btn_dpad_up_src = { .x = 74, .y = 30, .w = 37, .h = 37, };
    SDL_Rect btn_dpad_down_src = { .x = 111, .y = 30, .w = 37, .h = 37, };
    SDL_Rect btn_dpad_left_src = { .x = 148, .y = 30, .w = 37, .h = 37, };
    SDL_Rect btn_dpad_dst = { .x = 96, .y = 150, .w = 74, .h = 74, };

    SDL_Rect stick_plane_left_src = { .x = 0, .y = 67, .w = 37, .h = 37, };
    SDL_Rect stick_plane_left_dst = { .x = 96, .y = 230, .w = 74, .h = 74, };

    SDL_Rect stick_plane_right_src = { .x = 0, .y = 67, .w = 37, .h = 37, };
    SDL_Rect stick_plane_right_dst = { .x = 300, .y = 230, .w = 74, .h = 74, };

    SDL_Rect stick_left_src = { .x = 74, .y = 67, .w = 3, .h = 3, };
    const SDL_Rect stick_left_dst_init = {
        .x = stick_plane_left_dst.x + (stick_plane_left_dst.w/2 & (~0 - 1)) - 2,
        .y = stick_plane_left_dst.y + (stick_plane_left_dst.h/2 & (~0 - 1)) - 2,
        .w = 6,
        .h = 6,
    };
    SDL_Rect stick_left_dst = stick_left_dst_init;

    SDL_Rect stick_right_src = stick_left_src;
    const SDL_Rect stick_right_dst_init = {
        .x = stick_plane_right_dst.x + (stick_plane_right_dst.w/2 & (~0 - 1)) - 2,
        .y = stick_plane_right_dst.y + (stick_plane_right_dst.h/2 & (~0 - 1)) - 2,
        .w = 6,
        .h = 6,
    };
    SDL_Rect stick_right_dst = stick_right_dst_init;

    SDL_Rect btn_l1_src = { .x = 0, .y = 108, .w = 20, .h = 15, };
    SDL_Rect btn_l1_dst = { .x = 88, .y = 80, .w = 40, .h = 30, };

    SDL_Rect btn_l2_src = { .x = 80, .y = 108, .w = 18, .h = 15, };
    SDL_Rect btn_l2_dst = { .x = 134, .y = 80, .w = 36, .h = 30, };

    SDL_Rect btn_r1_src = { .x = 40, .y = 108, .w = 20, .h = 15, };
    SDL_Rect btn_r1_dst = { .x = 340, .y = 80, .w = 40, .h = 30, };

    SDL_Rect btn_r2_src = { .x = 116, .y = 108, .w = 18, .h = 15, };
    SDL_Rect btn_r2_dst = { .x = 300, .y = 80, .w = 36, .h = 30, };

    struct input_handl_ctx input = {0};
    bool should_exit = false;
    while (!should_exit) {
        SDL_Event event = (SDL_Event){0};
        while (SDL_PollEvent(&event)) {
            handle_btn_a(&btn_a_src, &event);
            handle_btn_b(&btn_b_src, &event);
            handle_btn_x(&btn_x_src, &event);
            handle_btn_y(&btn_y_src, &event);
            handle_btn_start(&btn_start_src, &event);
            handle_btn_select(&btn_select_src, &event);
            handle_left_stick(
                    &stick_plane_left_src,
                    &stick_left_dst,
                    &stick_left_dst_init,
                    &event);
            handle_right_stick(
                    &stick_plane_right_src,
                    &stick_right_dst,
                    &stick_right_dst_init,
                    &event);
            handle_btn_l1(&btn_l1_src, &event);
            handle_btn_r1(&btn_r1_src, &event);
            handle_btn_l2(&btn_l2_src, &event);
            handle_btn_r2(&btn_r2_src, &event);
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
        SDL_SetRenderDrawColor(renderer, 0x45, 0x45, 0x45, 0x00);
        SDL_RenderClear(renderer);
        SDL_RenderFillRect(renderer, &message_rect);
        SDL_RenderFillRect(renderer, &btn_a_dst);
        SDL_RenderFillRect(renderer, &btn_b_dst);
        SDL_RenderFillRect(renderer, &btn_x_dst);
        SDL_RenderFillRect(renderer, &btn_y_dst);
        SDL_RenderFillRect(renderer, &btn_start_dst);
        SDL_RenderFillRect(renderer, &btn_select_dst);
        SDL_RenderFillRect(renderer, &btn_dpad_dst);
        SDL_RenderCopy(renderer, message, NULL, &message_rect);
        SDL_RenderCopy(renderer, atlas, &btn_a_src, &btn_a_dst);
        SDL_RenderCopy(renderer, atlas, &btn_b_src, &btn_b_dst);
        SDL_RenderCopy(renderer, atlas, &btn_x_src, &btn_x_dst);
        SDL_RenderCopy(renderer, atlas, &btn_y_src, &btn_y_dst);
        SDL_RenderCopy(renderer, atlas, &btn_start_src, &btn_start_dst);
        SDL_RenderCopy(renderer, atlas, &btn_select_src, &btn_select_dst);
        SDL_RenderCopy(renderer, atlas, &btn_l1_src, &btn_l1_dst);
        SDL_RenderCopy(renderer, atlas, &btn_l2_src, &btn_l2_dst);
        SDL_RenderCopy(renderer, atlas, &btn_r1_src, &btn_r1_dst);
        SDL_RenderCopy(renderer, atlas, &btn_r2_src, &btn_r2_dst);
        SDL_RenderCopy(renderer, atlas, &btn_dpad_src, &btn_dpad_dst);
        if (is_dpad_right(&input))
            SDL_RenderCopy(renderer, atlas, &btn_dpad_right_src, &btn_dpad_dst);
        if (is_dpad_up(&input))
            SDL_RenderCopy(renderer, atlas, &btn_dpad_up_src, &btn_dpad_dst);
        if (is_dpad_down(&input))
            SDL_RenderCopy(renderer, atlas, &btn_dpad_down_src, &btn_dpad_dst);
        if (is_dpad_left(&input))
            SDL_RenderCopy(renderer, atlas, &btn_dpad_left_src, &btn_dpad_dst);
        if (is_dpad_left(&input))
            SDL_RenderCopy(renderer, atlas, &btn_dpad_left_src, &btn_dpad_dst);
        SDL_RenderCopy(renderer, atlas, &stick_plane_left_src, &stick_plane_left_dst);
        SDL_RenderCopy(renderer, atlas, &stick_plane_right_src, &stick_plane_right_dst);
        SDL_RenderCopy(renderer, atlas, &stick_left_src, &stick_left_dst);
        SDL_RenderCopy(renderer, atlas, &stick_right_src, &stick_right_dst);
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
