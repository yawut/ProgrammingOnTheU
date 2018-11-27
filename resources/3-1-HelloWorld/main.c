#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include <coreinit/screen.h>
#include <coreinit/cache.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log.h>
#include <whb/proc.h>

int main(int argc, char** argv) {
/*  Init logging. We log both to Cafe's internal logger (shows up in Decaf, some
    crash logs) and over UDP to be received with udplogserver. */
    WHBLogCafeInit();
    WHBLogUdpInit();
/*  \n characters are option with WHBLog */
    WHBLogPrint("Hello World! Logging initialised.");

/*  Init WHB's ProcUI wrapper. This will manage all the little Cafe OS bits and
    pieces for us - home menu overlay, power saving features, etc. */
    WHBProcInit();

/*  Init OSScreen. This is the really simple graphics API we'll be using to
    draw some text! */
    OSScreenInit();

/*  OSScreen needs buffers for each display - get the size of those now.
    "DRC" is Nintendo's acronym for the Gamepad. */
    size_t tvBufferSize = OSScreenGetBufferSizeEx(SCREEN_TV);
    size_t drcBufferSize = OSScreenGetBufferSizeEx(SCREEN_DRC);
    WHBLogPrintf("Will allocate 0x%X bytes for the TV, " \
                 "and 0x%X bytes for the DRC.",
                 tvBufferSize, drcBufferSize);

/*  Try to allocate an area for the buffers. According to OSScreenSetBufferEx's
    documentation, these need to be 0x100 aligned. */
    void* tvBuffer = memalign(0x100, tvBufferSize);
    void* drcBuffer = memalign(0x100, drcBufferSize);

/*  Make sure the allocation actually succeeded! */
    if (!tvBuffer || !drcBuffer) {
        WHBLogPrint("Out of memory!");

    /*  It's vital to free everything - under certain circumstances, your memory
        allocations can stay allocated even after you quit. */
        if (tvBuffer) free(tvBuffer);
        if (drcBuffer) free(drcBuffer);

    /*  Deinit everything */
        OSScreenShutdown();
        WHBProcShutdown();

        WHBLogPrint("Quitting.");
        WHBLogCafeDeinit();
        WHBLogUdpDeinit();

    /*  Your exit code doesn't really matter, though that may be changed in
        future. Don't use -3, that's reserved for HBL. */
        return 1;
    }

/*  Buffers are all good, set them */
    OSScreenSetBufferEx(SCREEN_TV, tvBuffer);
    OSScreenSetBufferEx(SCREEN_DRC, drcBuffer);

/*  Finally, enable OSScreen for each display! */
    OSScreenEnableEx(SCREEN_TV, true);
    OSScreenEnableEx(SCREEN_DRC, true);

/*  WHBProcIsRunning will return false if the OS asks us to quit, so it's a
    good candidate for a loop */
    while(WHBProcIsRunning()) {
    /*  Clear each buffer - the 0x... is an RGBX colour */
        OSScreenClearBufferEx(SCREEN_TV, 0x00000000);
        OSScreenClearBufferEx(SCREEN_DRC, 0x00000000);

    /*  Print some text. Coordinates are (columns, rows). */
        OSScreenPutFontEx(SCREEN_TV, 0, 0, "Hello world! This is the TV.");
        OSScreenPutFontEx(SCREEN_TV, 0, 1, "Neat, right?");

        OSScreenPutFontEx(SCREEN_DRC, 0, 0, "Hello world! This is the DRC.");
        OSScreenPutFontEx(SCREEN_DRC, 0, 1, "Neat, right?");

    /*  Flush all caches - read the tutorial, please! */
        DCFlushRange(tvBuffer, tvBufferSize);
        DCFlushRange(drcBuffer, drcBufferSize);

    /*  Flip buffers - the next is now on screen! Flipping is kinda like
        committing your graphics changes. */
        OSScreenFlipBuffersEx(SCREEN_TV);
        OSScreenFlipBuffersEx(SCREEN_DRC);
    }

    WHBLogPrint("Got shutdown request!");

/*  It's vital to free everything - under certain circumstances, your memory
    allocations can stay allocated even after you quit. */
    if (tvBuffer) free(tvBuffer);
    if (drcBuffer) free(drcBuffer);

/*  Deinit everything */
    OSScreenShutdown();
    WHBProcShutdown();

    WHBLogPrint("Quitting.");
    WHBLogCafeDeinit();
    WHBLogUdpDeinit();

/*  Your exit code doesn't really matter, though that may be changed in
    future. Don't use -3, that's reserved for HBL. */
    return 1;
}
