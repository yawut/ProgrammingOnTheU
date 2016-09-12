# Wii U Programming
A quick thrash for existing programmers

**Chapter 2 - A (technical) Overview of the Wii U**

Now we've looked at the Wii U from a distance, it's time to get a bit closer to what you'll be seeing on the metal. In this chapter, we'll be taking a quick overview of the important system libraries and how they work, what sort of memory setup we get, and what this is all used for in real life.

## The Wii U's Programming Model
(for browserhax/HBL)

The Wii U is different to the Wii in that it almost completely revolves around dynamic libraries. The graphics API is a dynamic library, the controllers are accessed with a dynamic library, pretty much everything you can do must be done through a dynamic library. This is pretty clever on Nintendo's part since they can enforce their permissions system in these libraries (although that's not too much of an issue under HBL). Libraries are in ``.rpl`` format (Nintendo's fancy pretty-much-ELF files). There's a whole bunch of them just sitting around in memory ready for you to use.

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
As you can see, all you need to get any of the Wii U's dynamic functions is one or two calls to OSDynLoad and the prototype for that function. While this is a pretty neat system, it does require you to know all the functions you'll ever want *and* which libary to find them in.
