//New functions
void* movedata (void* dest0, void* dest1, void* src0, void* src1, size_t bytes);
void delay(int ms);
void read_joystick(void);
char* res_falloc_read(char *name);
int res_find_name(char *name);
void res_init(char *buf);
int res_open(char *name);
void res_close(void);