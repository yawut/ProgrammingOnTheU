# Wii U Programming
A quick thrash for existing programmers

**Chapter 3 - Hello, world!**

Alright, let's get into it! We're going to look at a very simple wut application that uses two major libraries - **libwhb** and **OSScreen**. We'll take a quick look at (TODO finish writing intro)

Open up [resource 3.1](/resources/3-1-HelloWorld) in another tab - that's the code we'll be referencing throughout this chapter. We've only got two files there - `CMakeLists.txt` and `main.c`. `CMakeLists.txt` is part of the wut build system - as you've probably guessed, wut uses cmake to build homebrew, using macros to add a few extensions we'll look at later. `main.c` is the only source file for this app, and it's what we'll look at first in this chapter.

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

libwhb also ships with a "Console" logging backend, which prints your log messages to the screen. For the purposes of this tutorial, we're going to handle printing to the screen ourselves! We'll have a quick look at the Console backend later so you can save time for simple projects. In any case, let's continue with the code.

```c
WHBProcInit();
```
While it might not be obvious at first glance, Cafe is actually a fully multitasking OS, with a concept of the "foreground" and the "background". Sometimes, our app will get moved into the background - this can happen to open the HOME menu overlay, for example; though apps like the Internet Browser or Nintendo eShop do the same thing. Nintendo provides a library called **ProcUI** to handle these transitions along with a few other functions - the auto power-down features, for example. While ProcUI is relatively simple, libwhb provides an even simpler wrapper so we don't really have to think about it at all. Here we initialise that.

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
             tvBufferSize, drcBufferSize);
```
OSScreen needs two memory areas to put framebuffers in - here we use [`OSScreenGetBufferSizeEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#ga14a0a2e005fc00ddd23ac58aff566ee5) to ask how much it needs, both for the TV and DRC (Gamepad). The size needed depends on the resolution the Wii U is running at, so it's important we ask. We'll need access to these sizes even after allocation for reasons we'll see later, so I stick them in variables.

```c
void* tvBuffer = memalign(0x100, tvBufferSize);
void* drcBuffer = memalign(0x100, drcBufferSize);
```
Now we can actually allocate! There's nothing too noteworthy here, except my choice of `memalign` over a normal `malloc` - we'll get to my reason for this in a moment.

*Note: allocations and local variables do not necessarily start zeroed on Cafe like they do on other platforms - make sure you account for this or use calloc!*

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

With all our initiaisation code out of the way, it's time to get on with our planetary greetings!

```c
while(WHBProcIsRunning()) {
```
I talked before about what libwhb's ProcUI wrapper does - managing foreground/background states, some system poweroff features, whether we need to quit, things like that. All that processing actually happens within this call - `WHBProcIsRunning` calls out to Nintendo's ProcUI library, handles any requested foreground/background transitions, then finally returns true or false depending on whether the app should keep running. Due to the nature of the work it does, it makes sense to call this semi-regularly - so I elected to use it for a while loop in this example.

```c
OSScreenClearBufferEx(SCREEN_TV, 0x00000000);
OSScreenClearBufferEx(SCREEN_DRC, 0x00000000);
```
So, we're inside our loop - it's time to start rendering! We start with calls to [`OSScreenClearBufferEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#gaa265bdc1d4e801a8e9495ab4f4cabafe) for both the TV and Gamepad - this will wipe both framebuffers clean, filling them with the colour we request. Here, we pass in `0x00000000` - this is RGBX format colour. That means the first two digits are the red value, the next two are green, the third two are blue and the last two are ignored. All the values range from 00 to FF. So, this colour has 00 red, 00 green, and 00 blue... sounds like black! Making both screens black gives us a convenient starting point to render on top of.

```c
OSScreenPutFontEx(SCREEN_TV, 0, 0, "Hello world! This is the TV.");
OSScreenPutFontEx(SCREEN_TV, 0, 1, "Neat, right?");

OSScreenPutFontEx(SCREEN_DRC, 0, 0, "Hello world! This is the DRC.");
OSScreenPutFontEx(SCREEN_DRC, 0, 1, "Neat, right?");
```
Now we've cleared out our framebuffer, we can render some text into it! This is done with [`OSScreenPutFontEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#gacf5e67a9873092ab755c3af2db421a01) - it takes a screen, a row and column to start the text at; measured in *characters*, not pixels; and finally the string to actually draw. Here we put the text `"Neat, right?"` on the line below the `"Hello World!"` text - that third parameter is the row the text should start on.

One thing to keep in mind when using OSScreenPutFontEx is that the Wii U does no scaling - you can actually fit more characters on the TV when it's running at a resolution like 1080p or 720p. The Gamepad is always 480p, so you need to be careful - even if your text looks fine on the TV, it might be outside the borders of the Gamepad! There are ways to stretch the TV image so everything matches, but that's outside the scope of this tutorial.

At this point, you might think we're done - We've rendered our text, right? Sadly, we're not quite there yet - there's two things that remain to be done: flushing and flipping the buffers.

```c
DCFlushRange(tvBuffer, tvBufferSize);
DCFlushRange(drcBuffer, drcBufferSize);
```
It'd be crazy to try and fully explain everything that's going on here in a few short paragraphs. For those of you who are good with your CPU internals, this function flushes the buffers since the Wii U hardware isn't cache-coherent. You can probably skip to the next snippet now! For the rest of us, let's dive in.

If you remember the recap of computer science from Chapter 1, you'll know that all our data and code is stored in memory. The thing is, memory is *slow*. Compared to the CPU, it's outright unbearable. If the CPU interfaced with memory directly, it could easily spend most of its time waiting on the memory to cough up the required data. To solve this problem, modern computers have a **cache** - A small area of memory, usually inside the CPU chip itself. Despite its size - 512KiB on the Wii U (one of the three cores has 2MiB) - this memory is blazingly fast. You can't access it directly though; instead the CPU manages it, keeping a copy of any recently-accessed memory there. If the CPU needs to use that same memory again, it can get the data from the cache instead of the much slower main memory! It can also save some time by writing memory changes to the cache and letting the changes "filter down" to main memory in their own time - this is useful for things like counters that change frequently but don't really need to be in main memory.

This is great and results in a much faster system. There is one problem though - since the CPU can write to the cache without actually updating main memory, all our OSScreen rendering efforts may be caught up in the cache while memory remains outdated. This isn't an issue for our code on the CPU - we'll always read the most recent data - but the GPU is a different story. It knows nothing of the CPU's caches and just wants to read our pixel data from main memory. We can ensure that our writes from the CPU are actually in memory by *flushing* the cache - any changes stuck in the cache are written out and we can rest assured that main memory actually contains the data we wrote. That's why we need to call these functions - we flush any pending writes out of the data cache (**d**ata **c**ache **flush** **range**, or [`DCFlushRange`](https://decaf-emu.github.io/wut/group__coreinit__cache.html#ga3189eaf014ed0ec62c6ecfc5f25d658a)), so we can know that the GPU can read our image correctly and display what we want it to.

With that out of the way...

```c
OSScreenFlipBuffersEx(SCREEN_TV);
OSScreenFlipBuffersEx(SCREEN_DRC);
```
We've been drawing plenty of text into the OSScreen framebuffers, but if we stopped right here none of it would actually show up on screen. That's because all our text and image data has been drawn into the *work buffer* - a secondary framebuffer that everything gets drawn to. The screen doesn't show the work buffer, instead displaying the *visible buffer*. When we're ready, we can swap these two buffers by calling [`OSScreenFlipBuffersEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#ga09b9072ab8dd2095f97ba39e24e3b76b) - the work buffer becomes the visible buffer and is shown on-screen, while the old visible buffer becomes our new work buffer. This is known as "flipping" the buffers. This system is used to avoid screen tearing and visual glitches while we draw - things are only shown on-screen once we're ready to show them. It also gives us a chance to sort out the caches! This system of flipping two buffers is widely known as "double buffering".

```c
}
WHBLogPrint("Got shutdown request!");
```
At this point, our text is on-screen! Marvel at it in all its glory. The while loop we set up before will continue looping, constantly drawing new frames and displaying them. Pressing the HOME button will make `WHBProcIsRunning` return false, ending the loop and telling us to clean up and quit.

```c
if (tvBuffer) free(tvBuffer);
if (drcBuffer) free(drcBuffer);
```
We allocated these buffers earlier, so it's essential that we free them as part of our cleanup process. While PC applications can get away with being a tad lax here, the Wii U is a different story - depending on how your application ends up getting launched, it's possible for memory leaks to stick around after your application exits, sapping memory space from other programs. So, we play good citizen and free our buffers here.

*Note: Depending on which allocator your app uses (more on this later), calling `free(NULL)` can result in an application crash. To be safe, always do a null-check before freeing.*

```c
OSScreenShutdown();
WHBProcShutdown();

WHBLogPrint("Quitting.");
WHBLogCafeDeinit();
WHBLogUdpDeinit();
```
Not much to say here - we simply de-init the libraries we were using. Again, it's essential that we do this thoroughly to prevent knock-on effects to other applications.

```c
    return 1;
}
```
Well, that's it! We return a sane value here (some significance may be assigned to them in future) and our application quits!

*Note: Returning `-3` is reserved for HBL - in most cases, returning this will cause Mii Maker to open.*

## Suggestions and Further Learning
Once you've gotten over your obvious excitement (it didn't crash!) I recommend you go download a copy of the code ([resource 3.1](/resources/3-1-HelloWorld)) and mess with it. Tweak things, move chunks of code around, add new calls and logic, whatever. While you might break the app, I assure you it's impossible to break your console with OSScreen - if the worst happens and your code becomes unresponsive, just hold the power button down until the console switches off (the power LED will turn red). Get used to that - You'll probably be doing it a lot!

Here's some ideas for things you might change:

 - Change the text that gets drawn onscreen - change some words, maybe write a whole new message? Add a few extra lines of text. How much can you write before going off the side of the screen?
 - Change the background colour. Red or blue is easy - how about yellow? Purple? Try a different colour on the TV and the Gamepad.
 - Remember [`OSSleepTicks`](https://decaf-emu.github.io/wut/group__coreinit__thread.html#gaec240f68873bb19c753cfdd346264c17), from Chapter 2? Try adding a small delay at the end of the while loop - how long can you wait before controls start becoming unresponsive? Don't forget your `#include`s.
 - Play around with some of the other [OSScreen functions](https://decaf-emu.github.io/wut/group__coreinit__screen.html) - how about [`OSScreenPutPixelEx`](https://decaf-emu.github.io/wut/group__coreinit__screen.html#ga3f4b6594fdc62b57e5ceb6cdc0e57d5a)? Try drawing some shapes with that.

It may seem somewhat silly, but this is how I learnt and how most of the other Wii U developers I talked to learnt - taking existing code and experimenting on it; changing it to do whatever they want it to do. If you get stuck, feel free to ask for help! We've all been there and will gladly help. Wherever you found out about this tutorial is probably a good place to try.

***Chapter 3: So long!*** Chapter 4 isn't quite ready yet - please check back later! I'll be sure to mention it on my social accounts; [pick your platform](https://heyquark.com/aboutme/) if you're into that.
