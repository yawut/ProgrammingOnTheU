# Wii U Programming
A quick thrash for existing programmers

**Chapter 2 - A (technical) Overview of the Wii U**

Now we've looked at the Wii U from a distance, it's time to get a bit closer to what you'll be seeing on the metal. In this chapter, we'll be taking a quick overview of the important system libraries and how they work, what sort of memory setup we get, and what this is all used for in real life.

## The Wii U's Programming Model
(for browserhax/HBL)

Te Wii U is different to the Wii in that it almost completely revolves around dynamic libraries. The graphics API is a dynamic library, the wcontrollers are accessed with a dynamic library, pretty much everything you can do must be done through a dynamic library. This is pretty clever on Nintendo's part since they can enforce their permissions system in these libraries (although that's not too much of an issue under HBL). Libraries are in ``.rpl`` format (Nintendo's fancy pretty-much-ELF files). There's a whole bunch of them just sitting around in memory ready for you to use.

Since they're in dynamic libraries, function pointers are used to access most of the in-built functions on the Wii U. The function ``OSDynLoad_Acquire`` will give you a *handle* to a given library, while ``OSDynLoad_FindExport`` will export a function from a library into a function pointer. Here's an example of this in action:
```c
//Define OSDynLoad functions. Only needed for userspace, HBL has a far better method of delivering these functions.
#define OSDynLoad_Acquire ((void (*)(char* rpl, unsigned int *handle))0x0102A3B4)
#define OSDynLoad_FindExport ((void (*)(unsigned int handle, int isdata, char *symbol, void *address))0x0102B828)
//Set up the desired function pointer
void (*OSSleepTicks)(int ticks);

//get a handle to coreinit.rpl, the library that contains OSSleepTicks
unsigned int coreinit_handle;
OSDynLoad_Acquire("coreinit.rpl", &coreinit_handle);

//we now have a handle to coreinit in coreinit_handle.
//This is actually a void* pointing to the .rpl in memory.
//Let's export that function!
OSDynLoad_FindExport(coreinit_handle, 0, "OSSleepTicks", &OSSleepTicks);

//We can now call OSSleepTicks wherever the function pointer is accessible
OSSleepTicks(5000);
```
As you can see, all you need to get any of the Wii U's dynamic functions is one or two calls to OSDynLoad and the prototype for that function. While this is a pretty neat system, it does require you to know all the functions you'll ever want *and* which library to find them in.

So, let's go through the important libraries. You'll pick up functions as you go along, but it's essential to have an idea of where you might find them.

### coreinit.rpl
Coreinit is the mother of all the Wii U's dynamic libraries. It's basically a "misc" library that has everything from memory manipulation to SD card access to system information to obscure PowerPC instructions, and even a simple graphics system. Coreinit is often the first library accessed by any homebrew due to the sheer range of essential things that it can do.

Here's a few examples of useful coreinit functions:
 - The **OSScreen** group of functions, which provide a simple graphics system that just works. You get a framebuffer and a font renderer - we'll look at OSScreen in Chapter 3.
 - The **MEM** group of functions provide an easy way to manage memory and use heaps (lower-level equivalents of `malloc()` etc.). You don't *have* to use these functions (DMA works just fine), but it's easier if you do.
 - The **FS** and **FSA** set of functions talk to the SD card and other internal storage locations on the Wii U.
 - The **DC** and **IC** functions, which deal with the PowerPC's caching system and help enforce your favorite cache coherency model (because everyone has one of those).
 - All the other functions, such as the **IM** set which deal with power management, the **IOS** group which does exactly what you think they do, the **MCP** set which handle title installing and management, all the miscellaneous functions under **OS**... the list goes on.

### gx2.rpl
GX2 is the Wii U's fancy graphics API. I'm not particularly fluent in it at the time of writing but I hear it's similar to OpenGL. OSScreen will do for this tutorial, however, so we won't really be looking at it.

### vpad.rpl
The VPAD library has all the functions you need to communicate with the Gamepad (aka DRC).

### sndcore2.rpl / snd_core.rpl
These two libraries are responsible for sound (along with their friend libraries, `snd_user.rpl` and `snduser2.rpl`) and provide the functions you'll need to make all the strangled noises you want!

This is just a quick course on the most essential libraries, you can find a more complete list (along with specific function documentation) [here](https://wiiubrew.org/wiki/Cafe_OS#Libraries).

***Chapter 2: Over and out!***
In Chapter 3, we'll have a look at how this all comes together in a HBL-compatible Hello World! Take a look [here](/tutorial/Chapter%203.md).
