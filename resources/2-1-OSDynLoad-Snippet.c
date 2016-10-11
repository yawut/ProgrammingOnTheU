/**
	ProgrammingOnTheU - OSDynLoad example snippet
	See Chapter 2 for details.
**/

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
