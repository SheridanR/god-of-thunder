#pragma pack(push, 1)

//New functions
void* movedata(void* src0, void* src1, void* dest0, void* dest1, size_t bytes);
void delay(int16_t ms);
void read_joystick(void);
int32_t lzss_decompress(char *input, char *output);
void sdl_exit(void);
void sdl_update(void);
void tick_update(void);
uint32_t interrupt t0Service(uint32_t interval, void* param);
uint32_t convert_color(uint8_t);

#pragma pack(pop)

//New definitions
extern SDL_Window* sdl_window;
extern SDL_Surface* sdl_window_surface;
extern SDL_TimerID sdl_timer;
extern uint16_t current_page;

typedef struct palette {
    SDL_Color colors[256];
} palette_t;
extern palette_t sdl_palette;

#define NATIVE_SCREEN_WIDTH 320
#define NATIVE_SCREEN_HEIGHT 240