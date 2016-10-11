/**
	ProgrammingOnTheU - Hello World Example application (resource 3.1)
	See Chapter 3 for details.

	Execution starts at __entry_menu
**/

#include <stdlib.h>

#include "hbl.h"
#include "wiiu.h"

int __entry_menu(int argc, char **argv) {
	//casting tomfoolery to let us use Acquire and FindExport
	OSDynLoad_Acquire = (int(*)(const char*, unsigned int*))(OS_SPECIFICS->addr_OSDynLoad_Acquire);
	OSDynLoad_FindExport = (int(*)(unsigned int, int, const char*, void*))(OS_SPECIFICS->addr_OSDynLoad_FindExport);

	//Export the functions we need. See wiiu.h for function pointers.
	unsigned int coreinit_handle = 0;
	OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);

	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenGetBufferSizeEx", &OSScreenGetBufferSizeEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenSetBufferEx", &OSScreenSetBufferEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenEnableEx", &OSScreenEnableEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenClearBufferEx", &OSScreenClearBufferEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenPutFontEx", &OSScreenPutFontEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenFlipBuffersEx", &OSScreenFlipBuffersEx);
	OSDynLoad_FindExport(coreinit_handle, 0, "OSScreenInit", &OSScreenInit);

	OSDynLoad_FindExport(coreinit_handle, 0, "DCFlushRange", &DCFlushRange);

	unsigned int vpad_handle = 0;
	OSDynLoad_Acquire("vpad.rpl", &vpad_handle);

	OSDynLoad_FindExport(vpad_handle, 0, "VPADInit", &VPADInit);
	OSDynLoad_FindExport(vpad_handle, 0, "VPADRead", &VPADRead);

	//Time to start setting up OSScreen!
	//Self-explanatory. Don't forget this!
	OSScreenInit();

	//Get required buffer sizes
	unsigned int buffer0Size = OSScreenGetBufferSizeEx(0);
	unsigned int totalBufferSize = buffer0Size + OSScreenGetBufferSizeEx(1);

	//Set TV buffer (0) and DRC buffer (1) to reside in MEM1
	OSScreenSetBufferEx(0, (void*)0xF4000000);
	OSScreenSetBufferEx(1, (void*)(0xF4000000 + buffer0Size));

	//Turn on OSScreen for TV (0) and DRC (1)
	OSScreenEnableEx(0, 1);
	OSScreenEnableEx(1, 1);

	//Clear each buffer (RGBA colour)
	OSScreenClearBufferEx(0, 0x00000000);
	OSScreenClearBufferEx(1, 0x00000000);

	//Print text to TV (buffer 0)
	//Remember: PutFontEx is (bufferNum, x, y, text).
	//X is in characters (monospace font)
	//Y is in lines (small margin between characters)
	//Nintendo just had to be confusing, but this system is actually REALLY helpful once you're used to it.
	OSScreenPutFontEx(0, 0, 0, "Hello World! This is the TV.");
	OSScreenPutFontEx(0, 0, 1, "Press HOME on the DRC to quit back to HBL.");

	//Print text to DRC (buffer 1)
	OSScreenPutFontEx(1, 0, 0, "Hello World! This is the DRC.");
	OSScreenPutFontEx(1, 0, 1, "Press HOME on the DRC to quit back to HBL.");

	//Flush data to memory (read the tutorial please)
	DCFlushRange((void*)0xF4000000, totalBufferSize);

	//Flip the buffers, our text is now onscreen. Kinda like "committing" your changes.
	OSScreenFlipBuffersEx(0);
	OSScreenFlipBuffersEx(1);

	//Set up the DRC
	VPADInit();

	//Variables to hold button data
	VPADData vpad;
	int error;

	for (;;) { //If you didn't know this trick, it's basically while(true), but faster
		//Read the DRC's buttons
		VPADRead(0, &vpad, 1, &error);

		if (!error) {
			if (vpad.btns_h & VPAD_BUTTON_HOME) {
				break;
			}
		}
	}

	//Quickly clear out the OSScreen framebuffer.
	//We don't need to do this, but it's good practice.

	OSScreenClearBufferEx(0, 0x00000000);
	OSScreenClearBufferEx(1, 0x00000000);

	OSScreenFlipBuffersEx(0);
	OSScreenFlipBuffersEx(1);

	OSScreenClearBufferEx(0, 0x00000000);
	OSScreenClearBufferEx(1, 0x00000000);

	DCFlushRange((void*)0xF4000000, totalBufferSize);

    return EXIT_SUCCESS;
}
