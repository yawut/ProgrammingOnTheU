# Wii U Programming
A quick thrash for existing programmers

**Chapter 2 - A (technical) Overview of the Wii U**

Now we've looked at the Wii U from a distance, it's time to get a bit closer to what you'll be seeing on the metal. In this chapter, we'll be taking a quick overview of the important system libraries and how they work, what sort of memory setup we get, and what this is all used for in real life.

## The Wii U's Programming Model
(for RPX/wut)

The Wii U is different to the Wii in that it almost completely revolves around dynamic libraries. The graphics API is a dynamic library, the controllers are accessed with a dynamic library, pretty much everything you can do must be done through a dynamic library. This is pretty clever on Nintendo's part since they can enforce their permissions system in these libraries (although that's not too much of an issue for homebrew). Libraries are in ``.rpl`` format - they're just RPX with the extension changed. There's a whole bunch of them just sitting around in memory ready for you to use.

While HBL ELF homebrew and other formats have to play around with function pointers and dynamic linking by hand, wut leverages RPX's built-in dynamic library support to automatically pull in any functions you use from Nintendo's libraries - just include the appropriate header.

(people who read the old HBL ELF version of this tutorial will appreciate this)

*Resource 2.1 - WUT Functions Snippet*
```c
//include the wut header containing the function we want.
//Documentation is available for these headers - we'll get to that later.
#include <coreinit/thread.h>

//All the coreinit thread-related functions are now available.
//Let's use OSSleepTicks to sleep our thread for some time.
OSSleepTicks(5000);

//that's it! Let's add another header with macros to convert time units.
#include <coreinit/time.h>

//We now have a bunch of useful time-related macros.
//Let's use one to sleep for 5 seconds.
OSSleepTicks(OSSecondsToTicks(5));
```
WUT provides headers for most of Nintendo's libraries - you can use any function by including the appropriate header and possibly tweaking the link flags (more on this later).

So, let's go through the important libraries. You'll pick up functions as you go along, but it's essential to have an idea of where you might find them.

### coreinit.rpl
Coreinit is the mother of all the Wii U's dynamic libraries. It's basically a "misc" library that has everything from memory manipulation to SD card access to system information to obscure PowerPC instructions, and even a simple graphics system. Coreinit is often the first library accessed by any homebrew due to the sheer range of essential things that it can do.

Here's a few examples of useful coreinit functions:
 - The **OSScreen** group of functions, which provide a simple graphics system that just works. You get a framebuffer and a font renderer - we'll look at OSScreen in Chapter 3.
 - The **MEM** group of functions provide an easy way to manage memory and use heaps (lower-level equivalents of `malloc()` etc.). You don't *have* to use these functions - wut has several memory managment systems available for you to use. More on this later.
 - The **FS** and **FSA** set of functions talk to the SD card and other internal storage locations on the Wii U.
 - The **DC** and **IC** functions, which deal with the PowerPC's caching system and help enforce your favorite cache coherency model (because everyone has one of those).
 - All the other functions, such as the **IM** set which deal with power management, the **IOS** group which does exactly what you think they do, the **MCP** set which handle title installing and management, all the miscellaneous functions under **OS**... the list goes on.

### gx2.rpl
GX2 is the Wii U's fancy graphics API. I'm not particularly fluent in it at the time of writing but I hear it's similar to Vulkan. OSScreen will do for this tutorial, however, so we won't really be looking at it.

### vpad.rpl
The VPAD library has all the functions you need to communicate with the Gamepad (aka DRC).

### sndcore2.rpl / snd_core.rpl
These two libraries are responsible for sound (along with their friend libraries, `snd_user.rpl` and `snduser2.rpl`) and provide the functions you'll need to make all the strangled noises you want!

This is just a quick course on the most essential libraries, you can find a more complete list (along with specific function documentation) [here](https://wiiubrew.org/wiki/Cafe_OS#Libraries).

######todo: link to wut docs, not wiiubrew

***Chapter 2: Over and out!***
In Chapter 3, we'll have a look at how this all comes together in a Hello World! Take a look [here](/tutorial/Chapter%203.md).
