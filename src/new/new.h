#pragma pack(push, 1)

//New functions
void* movedata(void* src0, void* src1, void* dest0, void* dest1, size_t bytes);
void delay(int16_t ms);
void read_joystick(void);
int32_t lzss_decompress(char *input, char *output);

#pragma pack(pop)