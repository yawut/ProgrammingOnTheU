/**
	ProgrammingOnTheU - Hello World Example application (resource 3.1)
	See Chapter 3 for details.

	Execution starts at __entry_menu
**/

/**! Wii U dynamic library function pointers and other stuff **/
/**! This is not an exhaustive list, it's only the functions we need! **/
/**! You will encounter far more functions in your travels, for sure. **/
/**! Try dynamic_libs/ in Dimok-style projects. **/

//*******************************************
//                 COREINIT
//*******************************************

//DynLoad functions
int (*OSDynLoad_Acquire)(const char* rpl, unsigned int* handle);
int (*OSDynLoad_FindExport)(unsigned int handle, int isdata, const char* symbol, void* address);

//OSScreen functions
unsigned int (*OSScreenGetBufferSizeEx)(int bufferNum);
void (*OSScreenSetBufferEx)(int bufferNum, void* buffer);
void (*OSScreenEnableEx)(int bufferNum, int status);
void (*OSScreenClearBufferEx)(int bufferNum, unsigned int colour);
void (*OSScreenPutFontEx)(int bufferNum, int x, int y, char* text);
void (*OSScreenFlipBuffersEx)(int bufferNum);
void (*OSScreenInit)();

//Memory functions
void (*DCFlushRange)(void* memory, unsigned int size);

//Other stuff
unsigned int (*OSGetTime)();

//*******************************************
//                   VPAD
//*******************************************

//Button defines
#define VPAD_BUTTON_A        0x8000
#define VPAD_BUTTON_B        0x4000
#define VPAD_BUTTON_X        0x2000
#define VPAD_BUTTON_Y        0x1000
#define VPAD_BUTTON_LEFT     0x0800
#define VPAD_BUTTON_RIGHT    0x0400
#define VPAD_BUTTON_UP       0x0200
#define VPAD_BUTTON_DOWN     0x0100
#define VPAD_BUTTON_ZL       0x0080
#define VPAD_BUTTON_ZR       0x0040
#define VPAD_BUTTON_L        0x0020
#define VPAD_BUTTON_R        0x0010
#define VPAD_BUTTON_PLUS     0x0008
#define VPAD_BUTTON_MINUS    0x0004
#define VPAD_BUTTON_HOME     0x0002
#define VPAD_BUTTON_SYNC     0x0001
#define VPAD_BUTTON_STICK_R  0x00020000
#define VPAD_BUTTON_STICK_L  0x00040000
#define VPAD_BUTTON_TV       0x00010000

#define VPAD_STICK_R_EMULATION_LEFT    0x04000000
#define VPAD_STICK_R_EMULATION_RIGHT   0x02000000
#define VPAD_STICK_R_EMULATION_UP      0x01000000
#define VPAD_STICK_R_EMULATION_DOWN    0x00800000

#define VPAD_STICK_L_EMULATION_LEFT    0x40000000
#define VPAD_STICK_L_EMULATION_RIGHT   0x20000000
#define VPAD_STICK_L_EMULATION_UP      0x10000000
#define VPAD_STICK_L_EMULATION_DOWN    0x08000000

//Structs
typedef struct _Vec2D {
    float x,y;
} Vec2D;

typedef struct _VPADTPData {
    unsigned short x, y;               /* Touch coordinates */
    unsigned short touched;            /* 1 = Touched, 0 = Not touched */
    unsigned short invalid;            /* 0 = All valid, 1 = X invalid, 2 = Y invalid, 3 = Both invalid? */
} VPADTPData;

typedef struct _VPADData {
    unsigned int btns_h;                  /* Held buttons */
    unsigned int btns_d;                  /* Buttons that are pressed at that instant */
    unsigned int btns_r;                  /* Released buttons */
    Vec2D lstick, rstick;        /* Each contains 4-byte X and Y components */
    char unknown1c[0x52 - 0x1c]; /* Contains accelerometer and gyroscope data somewhere */
    VPADTPData tpdata;           /* Normal touchscreen data */
    VPADTPData tpdata1;          /* Modified touchscreen data 1 */
    VPADTPData tpdata2;          /* Modified touchscreen data 2 */
    char unknown6a[0xa0 - 0x6a];
    unsigned char volume;
    unsigned char battery;             /* 0 to 6 */
    unsigned char unk_volume;          /* One less than volume */
    char unknowna4[0xac - 0xa4];
} VPADData;

//Function pointers
void (*VPADInit)();
void (*VPADRead)(int chan, VPADData* buffer, unsigned int numberOfBuffers, int* error);
