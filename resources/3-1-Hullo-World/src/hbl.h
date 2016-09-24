/**
	ProgrammingOnTheU - Hello World Example application (resource 3.1)
	See Chapter 3 for details.

	Execution starts at __entry_menu
**/

/**! HBL compatibility stuff, allows us to avoid hardcoding things **/
/**! See common/common.h and common/os_defs.h in Dimok-style projects **/

typedef struct _OsSpecifics
{
    unsigned int addr_OSDynLoad_Acquire;
    unsigned int addr_OSDynLoad_FindExport;
    unsigned int addr_OSTitle_main_entry;

    unsigned int addr_KernSyscallTbl1;
    unsigned int addr_KernSyscallTbl2;
    unsigned int addr_KernSyscallTbl3;
    unsigned int addr_KernSyscallTbl4;
    unsigned int addr_KernSyscallTbl5;
} OsSpecifics;

#ifndef MEM_BASE
#define MEM_BASE                    (0x00800000)
#endif

#define OS_SPECIFICS                ((OsSpecifics*)(MEM_BASE + 0x1500))
