# Wii U Programming
A quick thrash for existing programmers

**Chapter 3 - Hello, world!**

Time to get down and dirty with some actual code! I've thrown together a Hello World example for HBL that we're going to take a look at. In this chapter, you'll see how **OSDynLoad** works in practice, the way to use **OSScreen**, reading from the DRC with **VPAD** functions, and how it all comes together in a HBL-compatible way.

Open up [resource 3.1](/resources/3-1-Hullo-World) in a new tab - it's the code we'll be referencing to throughout this chapter.

## Overview - the Hello World project

Resource 3.1 is a simple Hello World project I threw together to demonstrate how code on the Wii U works. I made it to be as simple as possible; it's the minimum you need to have this functionality while still having neat source. This certainly isn't the only program of this nature available on the system; but in my opinion it's the only one that does what it needs to do while remaining easy to get your head around.

The project consists of three files - entry.c, wiiu.h and hbl.h. They're all located in /src - feel free to take a look around! This chapter will go over each line-by-line.

***src/entry.c*** contains, among other things, the entrypoint (I suppose main.c is probably a better name). It's also where the bulk of the program logic is, with the headers resigned to static information and variable declarations. HBL will hand control to your program by calling a C function named `__entry_menu`. While it doesn't matter where the function is, I decided to keep it in entry.c to be neat.

***src/wiiu.h*** contains a large number of function pointer declarations, structs and bitmasks for interfacing with the Wii U's dynamic libraries. I've sorted this by library.

***src/hbl.h*** contains some #defines and other things to access the helpful information HBL leaves in memory for us. This particular setup only accesses the `OSSpecifics` struct, which contains the addresses of `OSDynLoad_Acquire` and `OSDynLoad_FindExport`; required to start loading libraries.

With that said, let's start looking into the code!

## entry.c - line by line

```c
#include <stdlib.h>

#include "hbl.h"
#include "wiiu.h"
```
Pretty much what you'd expect for the first few lines. It's worth noting that `<stdlib.h>` is provided by devkitPPC and was written for the Wii. While this won't be an issue for the majority of functions, some (most notably `malloc`) do not work correctly on the Wii U.

```c
int __entry_menu(int argc, char **argv) {
```
As I briefly mentioned above, `__entry_menu` is the function called by HBL to start the program. It functions identically to your usual `int main(int argc, char **argv)` function. In most cases, you won't get any arguments.

```c
//casting tomfoolery to let us use Acquire and FindExport
OSDynLoad_Acquire = (int(*)(const char*, unsigned int*))(OS_SPECIFICS->addr_OSDynLoad_Acquire);
OSDynLoad_FindExport = (int(*)(unsigned int, int, const char*, void*))(OS_SPECIFICS->addr_OSDynLoad_FindExport);
```
In this section, we take the addresses from the `OSSpecifics` struct I mentioned above (#defined to 0x00801500) and cast them into the function pointers set up in wiiu.h. This lets us use them to start exporting other functions, which is what we do next.

```c
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
```
If you remember how OSDynLoad works (see [Chapter 2](/tutorial/Chapter%202.md) if you don't) this should be fairly self-explanatory, just on a larger scale. Here, we export all the functions we need into their respective pointers, ready for us to use as we need.

```c
//Time to start setting up OSScreen!
//Self-explanatory. Don't forget this!
OSScreenInit();
```
Here we call our first OSScreen function - `OSScreenInit`. This function flicks registers and beats the graphics card into submission so we can use the other functions. It should always be called before any other OSScreen function.

```c
//Get required buffer sizes
unsigned int buffer0Size = OSScreenGetBufferSizeEx(0);
unsigned int totalBufferSize = buffer0Size + OSScreenGetBufferSizeEx(1);
```
OSScreen has two framebuffers - the first one for the TV and the second one for the DRC (0 and 1, respectivley). To help out with memory managment, we need the size of each. You'll see different approaches to this in different people's code, but here I call `OSScreenGetBufferSizeEx` on each buffer to get the size of the first buffer and the total size of both the buffers. These will be very useful later.

```c
//Set TV buffer (0) and DRC buffer (1) to reside in MEM1
OSScreenSetBufferEx(0, (void*)0xF4000000);
OSScreenSetBufferEx(1, (void*)(0xF4000000 + buffer0Size));
```
These two function calls tell OSScreen where we want the pixel data to be kept. The function itself is fairly simple - the buffer number and where we want it it memory. Here we set them to be right after each other (thus the need for the buffer sizes).

You can tell that I'm avoiding the real question here: *Where the heck did 0xF4000000 come from?*

I hinted in the introduction to this tutorial that the Wii U has a lot of different memory chips and areas. The actual system is quite complex, requiring *two* large tables dedicated to this on the wiki. However, the system can be simplified down to three major areas: MEM1, MEM2 and sometimes the bucket. MEM1 is a fairly small chunk of really fast on-die memory mapped from address 0xF4000000 to 0xF6000000. We usually use this for framebuffers due to its speed, although GCC also has a knack for putting arrays in this area - consider this if you get screen corruption! MEM2 is the bulk of the Wii U's memory. While slower than MEM1, it's far larger. It's mapped at 0x10000000. The size, and therefore end address, is defined on a per-app basis, anywhere up to 0x50000000. You're better off interacting with MEM2 via coreinit's MEM functions to deal with this uncertainty. The third area, the bucket, is another smallish area like MEM1. It seems to be used as a hardware communication area in some software, but it still functions like normal memory. In any case, it's worth knowing about and is a useful general-purpose mapping. It's mapped from 0xE0000000 to 0xE4000000.

Thus, to answer the question I posed earlier - the framebuffer is set to 0xF4000000 since it's in MEM1, a really fast and efficient part of the Wii U's memory.

It's worth noting that setting the buffer directly to an arbitrary address like this is sometimes considered bad form and more complex apps will usually have some kind of memory heap that they use to allocate a dedicated space just for the buffer. As long as it's aligned to the nearest 0x100 bytes, OSScreen will happily put the buffer anywhere.

```c
//Turn on OSScreen for TV (0) and DRC (1)
OSScreenEnableEx(0, 1);
OSScreenEnableEx(1, 1);
```
These two calls to OSScreenEnableEx are pretty self-explanatory - the first argument is the buffer number while the second argument is a boolean - enable OSScreen or disable OSScreen. These calls might not be neccesary (OSScreenInit flicks all the same switches) but it's nice to have there anyway, just to be safe.

```c
//Clear each buffer (RGBA colour)
OSScreenClearBufferEx(0, 0x00000000);
OSScreenClearBufferEx(1, 0x00000000);
```
Here we fill both buffers with a colour of our choosing (black). As with most of the OSScreen methods, the first argument is the buffer number, while in this case the second argument is an RGBA colour (like those used in HTML but with alpha/transparency on the end). In this case, we fill each buffer with black.

```c
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
```
Time for the good stuff! As you can tell, here's where we write some text to each buffer. Each buffer can be written to seperately and independently as shown here (with different text on each display). The comments in the code explain the arguments well enough so I won't go over them again here - it's really a "try it and see" situation anyway.

```c
//Flush data to memory (read the tutorial please)
DCFlushRange((void*)0xF4000000, totalBufferSize);
```
To cut an insanely long story short(ish); many years ago, someone at IBM decided that their shiny new PowerPC needed to cache memory (okay, memory caching is far older than that, but stay with me here). Thus, every PowerPC was given an internal chunk of memory. Due to it being inside the processor, it's stupidly fast - but you can't use it. The purpose of this memory is to be a mediator between the actual memory chips and the number-crunching bit of the processor - a cache. For those unfamiliar with caching, the idea is that you have a resource that's slow to access (main memory) and a temporary location that's very fast (the cache). When you try to read some memory, the processor first checks if there's a copy in the cache. If there is, it'll read that instead (because it's faster). A similar rule applies to when you're writing memory. Periodically, stuff in the cache will filter back down to main memory and everyone's happy.

This works great when there's a single PowerPC and nothing else. However, we have a graphics card to contend with - a graphics card that knows nothing of the cache and just wants a complete location in memory to read its pixel data from. Without programmer intervention, we'd end up with a situation where not all our newly-written pixel data ends up in memory (because it's still in the cache) and the graphics card ends up reading who-knows-what instead.

Thus, we, the programmers, intervene. When we "flush" a cache, we write whatever is in the cache into main memory. When we "invalidate" a cache, we tell the processor that the cache is not to be trusted and that it should read from main memory. Invalidaing is useful when some other bit of hardware starts messing with memory (looking at you, sound card) and we need to read that instead of what's in the cache. The PowerPC has seperate caches for instructions and data (although you can't flush the instruction cache, which is fair enough when you think about it); so to achieve what we need, we have to **flush** the **data cache** (we've been writing *data*) into memory. We only need to flush a section (or **range**) - the framebuffer.

Thus, DCFlushRange (**d**ata **c**ache **flush range**) is the function we use for the job. We start flushing at the address of our framebuffer (0xF4000000) and we keep on flushing for the size of the buffer (totalBufferSize). It's worth noting that due to the way the PowerPC arranges memory (in "blocks") you shouldn't trust the size to be interpreted as an exact number of bytes - the cache flush is likely to flush a bit more memory than what you asked.

```c
//Flip the buffers, our text is now onscreen. Kinda like "committing" your changes.
OSScreenFlipBuffersEx(0);
OSScreenFlipBuffersEx(1);
```
Finally, once everything is in main memory, we tell the graphics card it's okay to **flip the buffers**. You see, up until this point, we've been working on a buffer that's not actually on screen. Two buffers have been allocated - one for the graphics card to show onscreen and one for the OSScreen functions to operate on. In order to show all the graphics we've just rendered on-screen, we simply swap these buffers around - the one you've been writing to becomes the on on-screen and vice-versa. This system (called *double-buffering*) makes sure that there's never any flicker or weird artefacts.

Ta-da! At this point, our text is shown onscreen. If you want, you can just drop into a `while (1) {}` or `for (;;) {}` (common practices in embedded programming) and marvel at the wonderous font.

One problem - that's *boring.* I said we'd mess with the DRC, and mess we shall.

```c
//Set up the DRC
VPADInit();

//Variables to hold button data
VPADData vpad;
int error;
```
I reckon these are fairly self-explanatory - we call `VPADInit` to initialise the library and Gamepad, then set up a `VPADData` struct to hold the information we retrieve. We also make a variable to let us know of any errors the library faces.

```c
for (;;) {
	VPADRead(0, &vpad, 1, &error);
```
Here we have the VPADRead function, which (as implied) reads the DRC's status. The first argument is supposedly a channel, but since the console only supports one Gamepad at a time this argument is always 0. (Side note: the "channel" variable gets waaaay down into the base WiFi driver, fully implemented.) Next is a pointer to our VPADData struct to write into. The third argument is a strange one - It claims to be the number of arrays to write, but nobody has really found a use for writing more than one array. Thus, we just hardcode it to 1. The final argument is a pointer to write any errors to.

*(P.S: `for (;;)` is a C trick that basically means `while (1)` but can be more efficient on some older compilers.)*

```c
if (!error) {
	if (vpad.btns_h & VPAD_BUTTON_HOME) {
		break;
	}
}
```

VPADRead fills out a struct (usually named VPADData) that contain a bunch of bitmasked integers with button data. Here we take a look at `btns_h`, which contains buttons currently being *held.* Similar variables exist for buttons that were triggered and released on that poll (although these can be a little finicky); as well as lots of other information about what the DRC is up to like the touch-screen data and battery level. Check out the VPADData struct declaration in wiiu.h for more details. Anyway, this condition comes out as "if the home button is pressed", whereupon we break from the loop.

```c
//Quickly clear out the OSScreen framebuffer.
//We don't need to do this, but it's good practice.

OSScreenClearBufferEx(0, 0x00000000);
OSScreenClearBufferEx(1, 0x00000000);

OSScreenFlipBuffersEx(0);
OSScreenFlipBuffersEx(1);

OSScreenClearBufferEx(0, 0x00000000);
OSScreenClearBufferEx(1, 0x00000000);

DCFlushRange((void*)0xF4000000, totalBufferSize);
```

Before quitting, it's a good idea to quickly clean up your framebuffers. Since OSScreen simply works with raw memory and we didn't use any kind of memory managment there's a risk we may leave some garbage data in memory before quitting. While this usually isn't an issue, HBL seems to be a bit picky about this sort of thing. Your mileage may vary.

As for the code, we simply fill both buffers with black and flush the relevant caches. Since black is represented with zeros, we end up with nice, clean memory and a blank screen to boot.

```c
    return EXIT_SUCCESS;
}
```

We're done! Return your favorite status code that HBL will almost definitely ignore (except for `EXIT_RELAUNCH_ON_LOAD`, which boots Mii Maker and is not particularly useful) and revel in the way it cleanly exits back to HBL.

# Suggestions and Further Learning

Once you've gotten over your excitment about it not crashing, I stronly implore you to take the opportunity to read the code an mess with it. It's all available as [resource 3.1](/resources/3-1-Hullo-World) - compile it, play with it, get a feel for it. Here's some suggestions:

 - Change which button the code looks out for. Why not A? ZR? One of the stick buttons?
 - Add a few more lines of text of your own choosing. Remember double-buffering!
 - If you're feeling confident, why not FindExport `OSSleepTicks` (from earlier in the tutorial) and add a sleep to the for loop? It's working the CPU pretty hard right now, after all. Be careful though - a sleep for too long will make the DRC feel unresponsive!

This is how I learnt - playing with existing code (I drew circles on a button tester!) and experimenting around. If you get stuck or need help, feel free to ask! Every other developer you'll find has been here before and they'll gladly help you out.
