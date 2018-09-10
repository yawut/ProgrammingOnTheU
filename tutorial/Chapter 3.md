# Wii U Programming
A quick thrash for existing programmers

**Chapter 3 - Hello, world!**

Alright, let's get into it! We're going to look at a very simple wut application that uses two major libraries - **libwhb** and **OSScreen**. We'll take a quick look at (TODO finish writing intro)

Open up [resource 3.1](/resources/3-1-HulloWorld) in another tab - that's the code we'll be referencing throughout this chapter. We've only got two files there - `CMakeLists.txt` and `main.c`. ``CMakeLists.txt` is part of the wut build system - as you've probably guessed, wut uses cmake to build homebrew, using macros to add a few extensions we'll look at later. `main.c` is the only source file for this app, and it's what we'll look at first in this chapter.

You might also want to reference against [wut's documentation](https://decaf-emu.github.io/wut/), which lists all the Nintendo-provided functions along with descriptions for most of them. I'll link to the specific functions where I can, though be aware the links may go bad as wut updates.

## `main.c` - line by line
So, let's open up this source and have a look! Feel free to take this at your own pace - read the source, read this chapter, whatever. I've added some comments to `main.c` if you want to blow through quickly, but for a deeper look I'm going to go over it line by line.

```c
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>
```
This is about what you'd expect from a C program. There are a few small considerations when using the standard C functions - we'll get to those later.

```c
#include <coreinit/screen.h>
#include <whb/log_cafe.h>
#include <whb/log_udp.h>
#include <whb/log.h>
#include <whb/proc.h>
```
Here's our first hint that this is no average C program! [`<coreinit/screen.h>`](https://decaf-emu.github.io/wut/screen_8h.html) is a wut-supplied header that contains all the OSScreen functions we'll be using later to make some graphics. The other three headers are from libwhb, pulling in logging (with both Cafe and UDP backends - more on this later) and `proc.h`, which we'll also come to soon enough.

```c
int main(int argc, char** argv) {
```
Homebrew using wut has a normal `main` function, as you'd expect. One thing to note is that there's not an obvious way for the user to influence the argc/argv values, so there's little point in parsing them in most cases.

```c
WHBLogCafeInit();
WHBLogUdpInit();
WHBLogPrint("Hello World! Logging initialised.");
```
Here we set up some logging! libwhb has this neat system where you can set up as many logging backends as you like, and your messages will get sent to all of them. First, we call `WHBLogCafeInit` to set up libwhb's Cafe logger - this uses the Wii U's internal logging systems. These aren't usually visible to you, but the messages do show up when using `decaf-emu` (a Wii U emulator) or when viewing certain crash logs on-console. Next, we call `WHBLogUdpInit` to init the UDP logger - it'll send all log messages out over the network. These can be recieved with the `udplogserver` tool, included with wut. We can now use `WHBLogPrint` to send out our first message!

```c
WHBProcInit();
```
While it might not be obvious at first glance, Cafe is actually a fully multitasking OS, with a concept of the "foreground" and the "background". Sometimes, our app will get moved into the background - this can happen to open the HOME menu overlay, for example; though apps like the Internet Browser or Nintendo eShop do the same thing. Nintendo provides a library called **ProcUI** to handle these transitions along with a few other functions - the auto power-down features, for example. While ProcUI is relativley simple, libwhb provides an even simpler wrapper so we don't really have to think about it at all. Here we initialise that.

*Note: at the time of writing, libwhb's procui wrapper does not allow us to cleanly move from the background into the foreground - so resuming the app from the menu overlay may not work. Also note that when running from HBL, libwhb will exit the app instead of moving to the background.*

```c
OSScreenInit();
```
Here we start to init OSScreen; a really simple graphics library that Nintendo made (though we've never seen them use it). Just calling [`OSScreenInit`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#gac678395798fae82a857a824eedebd7de) isn't quite enough, though - most of what we're doing from here on out will be related to OSScreen in some way.

```c
size_t tvBufferSize = OSScreenGetBufferSizeEx(SCREEN_TV);
size_t drcBufferSize = OSScreenGetBufferSizeEx(SCREEN_DRC);
WHBLogPrintf("Will allocate 0x%X bytes for the TV, " \
             "and 0x%X bytes for the DRC.",
             tvBufferSize, drcBufferSize)
```
OSScreen needs two memory areas to put framebuffers in - here we use [`OSScreenGetBufferSizeEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#ga14a0a2e005fc00ddd23ac58aff566ee5) to ask how much it needs, both for the TV and DRC (Gamepad). The size needed depends on the resolution the Wii U is running at, so it's important we ask. We'll need access to these sizes even after allocation for reasons we'll see later, so I stick them in variables.

```c
void* tvBuffer = memalign(0x100, tvBufferSize);
void* drcBuffer = memalign(0x100, drcBufferSize);
```
Now we can actually allocate! There's nothing too noteworthy here, except my choice of `memalign` over a normal `malloc`.

*Note: allocations and local variables do not neccesarily start zeroed on Cafe like they do on other platforms - make sure you account for this or use calloc!*

I'm going to skip over the nullcheck here - you can go look at it in the code if you want, but rest assured it's just an `if (!tvBuffer || !drcBuffer)` followed by the shutdown code we'll look at later.

```c
OSScreenSetBufferEx(SCREEN_TV, tvBuffer);
OSScreenSetBufferEx(SCREEN_DRC, drcBuffer);
```
So, we've allocated the memory OSScreen asked for, so now we just have to tell it! This code should be pretty self-explanatory - we use [`OSScreenSetBufferEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#gae7f7bf93df292c52972baf07b0ba8116) to set the pointers for both the TV and DRC screens. If you check out the linked documentation for that function, you'll note how it stresses the pointers should be 0x100 aligned - this is the reason I went with `memalign` before.

```c
OSScreenEnableEx(SCREEN_TV, true);
OSScreenEnableEx(SCREEN_DRC, true);
```
With that, we've got OSScreen set up and good to go! We call [`OSScreenEnableEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#ga0dd2476b23f7f4e52a5167f2335773e3) to have OSScreen actually take control of the displays and get our framebuffer up.