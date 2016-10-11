# Wii U Programming
A quick thrash for existing programmers

**Chapter 3 - Hello, world!**

Time to get down and dirty with some actual code! I've thrown together a Hello World example for HBL that we're going to take a look at. In this chapter, you'll see how **OSDynLoad** works in practice, the way to use **OSScreen**, reading from the DRC with **VPAD** functions, and it all comes together in a HBL-compatible way.

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
