#define far

#include <stdint.h>

extern	void LZSS_decompress(char far *src,char far *dest,int16_t len);
uint16_t LZSS_compress(int32_t origsize,char far *input,char far *output);

typedef struct{
    char name[9];
    int32_t offset;
    int32_t length;   //original len
    int32_t original_size;
    int16_t  key;
} RES_HEADER;

#define RES_MAX_ENTRIES 256  //max # of elements

#define RES_NO_ERROR		0

#define RES_NOT_ACTIVE      -1
#define RES_CANT_OPEN       -2
#define RES_CANT_CLOSE      -3
#define RES_CANT_WRITE      -4
#define RES_CANT_READ       -5
#define RES_ENTRY_NOT_FOUND -6
#define RES_ALREADY_OPEN    -7
#define RES_FULL            -8
#define RES_CANT_SEEK       -9
#define RES_NOT_OPEN        -10
#define RES_CANT_CREATE     -11
#define RES_CANT_FIND       -12
#define RES_ALREADY_EXISTS  -13
#define RES_CANT_OPEN_SRC   -14
#define RES_CANT_OPEN_DST   -15
#define RES_SRC_TOO_LARGE   -16
#define RES_OUT_OF_MEMORY   -17

extern FILE *res_fp;
extern RES_HEADER res_header[RES_MAX_ENTRIES];
extern int16_t res_active;
extern int16_t res_changed;
extern char far *res_lzss_buff;

void res_init(char far *);
int16_t  res_open(char *filename);
int16_t  res_close(void);
void res_encrypt(char far *buff,int32_t len,char key);
void res_decrypt(char far *buff,int32_t len,char key);
int16_t  res_abort(void);
int16_t  res_create(char *filename);
int32_t res_encode(char far *buff,int32_t length);
int32_t res_decode(char far *buff);
int16_t  res_find_name(char *name);
int16_t  res_find_empty(void);
int16_t  res_write(char *name,char far *buff,int32_t length,int16_t encode_flag);
int32_t res_read(char *name,char far *buff);
int32_t res_read_element(char *name,char far *buff,int32_t offset,int32_t size);
void far *res_falloc_read(char *name);
int16_t  res_add_file(char *fname,char *name,int16_t encode_flag);
int16_t  res_extract_file(char *fname,char *name);
int16_t  res_delete_file(char *name);
int16_t  res_pack(char *filename);
int16_t  res_replace_file(char *fname,char *name,int16_t encode_flag);
int32_t res_length(char *name);
int32_t res_original_size(char *name);
int16_t  res_rename_file(char *oldname,char *newname);
char *res_error(int16_t num);
