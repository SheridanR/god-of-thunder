// THOR - The God of Thunder
// See LICENSE for details
#pragma pack(push, 1)


//G_LIB.LIB

void xsetmode(void);
void xshowpage(unsigned page);
void xline(int16_t x0,int16_t y0,int16_t x1,int16_t y1,int16_t page,int16_t color);
void xfillrectangle(int16_t StartX, int16_t StartY, int16_t EndX, int16_t EndY,
    uint16_t PageBase, int16_t Color);
void xpset(int16_t X, int16_t Y, uint16_t PageBase, int16_t Color);
int16_t  xpoint(int16_t X, int16_t Y, uint16_t PageBase);
void xget(int16_t x1,int16_t y1,int16_t x2,int16_t y2,uint16_t pagebase,
    char *buff,int16_t invis);
void xput(int16_t x,int16_t y,uint16_t pagebase,char *buff);
void xput2(int16_t x,int16_t y,uint16_t pagebase,char *buff);
void xtext(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color);
void xtext1(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color);
void xtextx(int16_t x,int16_t y,uint16_t pagebase,char *buff,int16_t color);

void xfput(int16_t x,int16_t y,uint16_t pagebase,char *buff);
void xfarput(int16_t x,int16_t y,uint16_t pagebase,char *buff);
void xcopyd2dmasked(int16_t SourceStartX,
    int16_t SourceStartY, int16_t SourceEndX, int16_t SourceEndY,
    int16_t DestStartX, int16_t DestStartY, MASK_IMAGE * Source,
    uint16_t DestPageBase, int16_t DestBitmapWidth);
void xcopyd2dmasked2(
    int16_t SourceEndX, int16_t SourceEndY,
    int16_t DestStartX, int16_t DestStartY, MASK_IMAGE *Source,
    uint16_t DestPageBase);
void xcopys2d(int16_t SourceStartX, int16_t SourceStartY,
    int16_t SourceEndX, int16_t SourceEndY, int16_t DestStartX,
    int16_t DestStartY, char* SourcePtr, uint16_t DestPageBase,
    int16_t SourceBitmapWidth, int16_t DestBitmapWidth);
void xcopyd2d(int16_t SourceStartX, int16_t SourceStartY,
    int16_t SourceEndX, int16_t SourceEndY, int16_t DestStartX,
    int16_t DestStartY, uint16_t SourcePageBase,
    uint16_t DestPageBase, int16_t SourceBitmapWidth,
    int16_t DestBitmapWidth);
uint16_t xcreatmaskimage(MASK_IMAGE * ImageToSet,
    uint16_t DispMemStart, char * Image, int16_t ImageWidth,
    int16_t ImageHeight, char * Mask);
uint16_t xcreatmaskimage2(MASK_IMAGE * ImageToSet,
    uint16_t DispMemStart, char * Image, int16_t ImageWidth,
    int16_t ImageHeight, char * Mask);

void xddfast(int16_t source_x,int16_t source_y, int16_t width, int16_t height,
    int16_t dest_x, int16_t dest_y,
    uint16_t source_page,uint16_t dest_page);
xsetpal(unsigned char color, unsigned char R,unsigned char G,unsigned char B);
xgetpal(char * pal, int16_t num_colrs, int16_t start_index);

//G_MAIN.C
void run_gotm(void);
void printt(int16_t val);
void thor_dies(void);
void thor_spins(int16_t flag);
void thor_spins(int16_t flag);
void pause(int16_t delay);
void rotate_pal(void);
int16_t  rnd(int16_t max);

//G_GRP.C

void xprint(int16_t x,int16_t y,char *string,uint16_t page,int16_t color);
void xprintx(int16_t x,int16_t y,char *string,uint16_t page,int16_t color);
void split_screen(void);
int16_t  load_palette(void);
void xbox(int16_t x1,int16_t y1,int16_t x2,int16_t y2,unsigned page,int16_t color);
void fade_in(void);
void fade_out(void);
void unsplit_screen(void);
void screen_dump(void);
void show_all_actors(void);

//G_INIT.C
int16_t initialize(void);
void exit_code(int16_t ex_flag);
#define interrupt
void interrupt keyboard_int();              // interrupt prototype
void demo_key_set(void);
void wait_not_response(void);
int16_t  wait_response(void);
int16_t  get_response(void);
void wait_key(int16_t index);
void wait_not_key(int16_t index);
int16_t  wait_ekey(int16_t index);
int16_t  wait_not_ekey(int16_t index);
void joy_key(void);
void set_joy(void);
void merge_keys(void);
int16_t  setup_boss(int16_t num);
void story(void);

//G_FILE.C
int32_t file_size(char *path);
uint16_t read_file(char *filename,char *buff,
    int32_t offset, uint16_t amount,int16_t key);
int16_t  load_bg_data(void);
int16_t  load_sd_data(void);
int16_t  load_objects(void);
int16_t  load_actor(int16_t file,int16_t num);
int16_t  load_picture(int16_t index,char *buff);
void setup_filenames(int16_t level);
int16_t  load_speech(int16_t index);
int32_t file_size(char *path);
void *get_file(char *filename,int16_t key);
void save_game(void);
int16_t  load_game(int16_t flag);
void help(void);
int32_t res_read(char *name,char *buff);
int16_t  load_music(int16_t num);

//G_PANEL.C
void status_panel(void);
void display_health(void);
void display_magic(void);
void display_jewels(void);
void display_score(void);
void display_keys(void);
void display_item(void);
int16_t  init_status_panel(void);
void add_jewels(int16_t num);
void add_score(int16_t num);
void add_magic(int16_t num);
void add_health(int16_t num);
void add_keys(int16_t num);
void fill_health(void);
void fill_magic(void);
void fill_score(int16_t num);
void score_for_inv(void);
void boss_status(int16_t health);
int16_t  select_option(char *option[],char *title,int16_t ipos);
int16_t  option_menu(void);
int16_t  ask_exit(void);
int16_t  select_sound(void);
int16_t  select_music(void);
int16_t  select_slow(void);
int16_t  select_scroll(void);
void select_fastmode(void);
void select_skill(void);
void hammer_smack(int16_t x,int16_t y);
void show_scr(void);


//G_BACK.C
void build_screen(uint16_t pg);
void show_level(int16_t new_level);
void scroll_level_left(void);
void scroll_level_up(void);
void scroll_level_right(void);
void scroll_level_down(void);
void phase_level(void);
void copy_bg_icon(int16_t num,uint16_t src_page,uint16_t dst_page);
int16_t  odin_speaks(int16_t index,int16_t item);
void d_restore(void);
int16_t  actor_speaks(ACTOR *actr,int16_t index,int16_t item);
int16_t  display_speech(int16_t item, char *pic,int16_t tf);
void select_item(void);
void show_item(int16_t item);
int16_t  use_thunder(int16_t flag);
int16_t  use_hourglass(int16_t flag);
int16_t  use_boots(int16_t flag);
void use_item(void);
int16_t  switch_icons(void);
int16_t  rotate_arrows(void);
void kill_enemies(int16_t iy,int16_t ix);
void remove_objects(int16_t y,int16_t x);
void place_tile(int16_t x,int16_t y,int16_t tile);
int16_t  bgtile(int16_t x,int16_t y);

//G_IMAGE.C
uint16_t make_mask(MASK_IMAGE * image,
    uint16_t page_start, char * Image, int16_t image_width,
    int16_t image_height);
int16_t  load_standard_actors(void);
void setup_actor(ACTOR *actr,char num,char dir,int16_t x, int16_t y);
void show_enemies(void);
int16_t  load_enemy(int16_t type);
int16_t actor_visible(int16_t invis_num);
void setup_magic_item(int16_t item);
void load_new_thor(void);

//G_MOVE.C
void next_frame(ACTOR *actr);
int16_t  point_within(int16_t x,int16_t y,int16_t x1,int16_t y1,int16_t x2,int16_t y2);
int16_t  overlap(int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t x3,int16_t y3,int16_t x4,int16_t y4);
int16_t  reverse_direction(ACTOR *actr);
void thor_shoots(void);
void actor_damaged(ACTOR *actr,int16_t damage);
void thor_damaged(ACTOR *actr);
void actor_destroyed(ACTOR *actr);
int16_t  actor_shoots(ACTOR *actr,int16_t dir);
void actor_always_shoots(ACTOR *actr,int16_t dir);
void move_actor(ACTOR *actr);

//G_MOVPAT.C
int16_t  check_move0(int16_t x,int16_t y, ACTOR *actr);
int16_t  check_move1(int16_t x,int16_t y, ACTOR *actr);
int16_t  check_move2(int16_t x,int16_t y, ACTOR *actr);
int16_t  check_move3(int16_t x,int16_t y, ACTOR *actr);
int16_t  check_move4(int16_t x,int16_t y, ACTOR *actr);
int16_t  check_thor_move(int16_t x,int16_t y, ACTOR *actr);
void set_thor_vars(void);

//G_OBJECT.C
void show_objects(int16_t level,uint16_t pg);
void pick_up_object(int16_t p);
int16_t  drop_object(ACTOR *actr);
int16_t  _drop_obj(ACTOR *actr,int16_t o);
void delete_object(void);

//G_SPTILE.C
int16_t special_tile_thor(int16_t x,int16_t y,int16_t icon);
int16_t special_tile(ACTOR *actr,int16_t x,int16_t y,int16_t icon);

//G_SBFX.C
int16_t  sbfx_init(void);
void sbfx_exit(void);

//G_SOUND.C
int16_t  sound_init(void);
void sound_exit(void);
void play_sound(int16_t index, int16_t priority_override);
int16_t  sound_playing(void);

//G_MUSIC.C
int16_t  music_init(void);
void music_play(int16_t num,int16_t override);
void music_pause(void);
void music_resume(void);
int16_t  music_is_on(void);

//G_SCRIPT.C
void execute_script(int32_t index,char *pic);

//G_BOSS1.C   //serpent

int16_t  boss1_movement(ACTOR *actr);
void check_boss1_hit(ACTOR *actr,int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t act_num);
void boss_level1(void);
int16_t  boss_dead1(void);
void closing_sequence1(void);

//G_BOSS21.C  //wraith

int16_t  boss21_movement(ACTOR *actr);
void check_boss21_hit(void);
void boss_level21(void);
int16_t  boss_dead21(void);
void closing_sequence21(void);

//G_BOSS22.C  //skull

int16_t  boss22_movement(ACTOR *actr);
void check_boss22_hit(ACTOR *actr,int16_t x1,int16_t y1,int16_t x2,int16_t y2,int16_t act_num);
void boss_level22(void);
int16_t  boss_dead22(void);
void check_boss21_hit(void);
void closing_sequence22(void);
void boss22_set(int16_t d,int16_t x,int16_t y);
int16_t  boss22a_movement(ACTOR *actr);
int16_t  boss22b_movement(ACTOR *actr);

//G_ASM.ASM
void xdisplay_actors(ACTOR *act,uint16_t page);
void xerase_actors(ACTOR *act,uint16_t page);
void pal_fade_in(char *buff);
void pal_fade_out(char *buff);
void read_joystick(void);
void UnLZSS(char *src,char *dest,int16_t len);
#define REPEAT(a) for(rep=0;rep<a;rep++)
#define IN_RANGE(v,l,h) (v>=l && v<=h)

//Replace old functions
#define farfree free
#define farmalloc malloc
#define _fstrncpy strncpy
#define _fmemcpy memcpy
#define _fmemset memset
#define _fstrlen strlen
#define _fstrncmp strncmp
#define _fstrcpy strcpy
#define _fstrcat strcat
#define _fstrcmp strcmp
#define FP_SEG
#define FP_OFF

#pragma pack(pop)