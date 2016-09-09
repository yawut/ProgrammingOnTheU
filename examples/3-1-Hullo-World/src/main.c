#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include "dynamic_libs/os_functions.h"
#include "dynamic_libs/vpad_functions.h"
#include "utils/utils.h"
#include "common/common.h"

/* Entry point */
int Menu_Main(void)
{
    InitOSFunctionPointers();
    InitVPadFunctionPointers();
	
    VPADInit();

    // Prepare screen
    int screen_buf0_size = 0;
    int screen_buf1_size = 0;

    // Init screen and screen buffers
    OSScreenInit();
    screen_buf0_size = OSScreenGetBufferSizeEx(0);
    screen_buf1_size = OSScreenGetBufferSizeEx(1);

    unsigned char *screenBuffer = (unsigned char*)0xF4000000;

    OSScreenSetBufferEx(0, screenBuffer);
    OSScreenSetBufferEx(1, (screenBuffer + screen_buf0_size));

    OSScreenEnableEx(0, 1);
    OSScreenEnableEx(1, 1);

    // Clear screens
    OSScreenClearBufferEx(0, 0);
    OSScreenClearBufferEx(1, 0);

    // print to TV
    OSScreenPutFontEx(0, 0, 0, "Hullo World! Looks like we're programming with gas.");
    OSScreenPutFontEx(0, 0, 1, "Press HOME-Button to exit.");

    // print to DRC
    OSScreenPutFontEx(1, 0, 0, "Hullo World! This is the gamepad (aka DRC).");
	OSScreenPutFontEx(1, 0, 1, "Don't ask me why it's named that.");
    OSScreenPutFontEx(1, 0, 2, "Press HOME-Button to exit.");

    // Flush the cache
    DCFlushRange(screenBuffer, screen_buf0_size);
    DCFlushRange((screenBuffer + screen_buf0_size), screen_buf1_size);

    // Flip buffers
    OSScreenFlipBuffersEx(0);
    OSScreenFlipBuffersEx(1);

    int vpadError = -1;
    VPADData vpad;

    while(1)
    {
        VPADRead(0, &vpad, 1, &vpadError);

        if(vpadError == 0 && ((vpad.btns_d | vpad.btns_h) & VPAD_BUTTON_HOME))
            break;

		usleep(50000);
    }

	screenBuffer = NULL;

    return EXIT_SUCCESS;
}

