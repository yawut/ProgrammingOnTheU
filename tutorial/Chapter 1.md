# Wii U Programming
A quick thrash for existing programmers

**Chapter 1 - An Overview of the Wii U**

Heya! (announcer voice) Welcome... to the world of Wii U programming. In all seriousness; hi! This is a quick guide for pre-existing programmers to get their head around the Wii U and what it has to offer. It's a great console that's been completely cracked open. Programming is a challenge, but it's fun; and the end results are always satisfying. I hope to see your apps running on my console soon!

## Prerequisites
 - A Wii U and a computer
 - Fairly decent C or C++ experience (Personally, I used knowledge of Java to pick up C as I went. I don't recommend it.)
 - Facilities to compile homebrew (this tutorial will use wut)
 - Basic knowledge of the Wii U scene
 - The right mindset (exactly what this means is up to you)

## A Quick Recap of Computer Science
Just in case you're a bit behind on your computer science, we're going to quickly tear through some basics so we're all up to speed. Ready? Let's go!

Any computer has a processor, some memory and various I/O things. The processor reads instructions from memory and executes them. Each instruction might be telling the processor to read something from memory, add some numbers, whatever. Memory (specifically RAM) is a spot where the processor can store data, and lots of it. Each byte of memory has an "address"; counted in hex (0 up to F). Due to the way computers are wired up inside, different memory addresses can often refer to different chips on the motherboard; the Wii U has several memory chips that all have their own address "ranges" (for example, 0xF4000000 to 0xF6000000 is the "MEM1" chip). These ranges are often not the same as the way the machine is wired up; processors usually have some method of "mapping" addresses.

That's a lot to take in, but if you can get it (especially memory addressing) you'll find that low-level poking on the Wii U becomes much easier.

## The Wii U's Security Model
The Wii U has an interesting security system. Two processors work together to make the system we have to deal with; the PowerPC and the ARM (aka IOSU).

The PowerPC processor is where your code will run - all official games, apps and most homebrew software run on the PowerPC. The PowerPC has a kernel as well, which handles the process of interfacing with IOSU and other system functions. The PowerPC kernel can access all memory, although programs running within its userspace (games/apps) are subject to a permissions system. The PowerPC kernel exploit (which is run automatically by the browser exploit and Haxchi before the Homebrew Launcher is opened) allows the introduction of a "backdoor" that allows the running game/app to access all memory on the console.

The IOSU is a seperate ARM processor that acts as a gatekeeper between the PowerPC and the Wii U's hardware. It enforces a permissions system where it applies restrictions based on what program is running on the PowerPC. It's no powerhouse but it does a rather good job; for example, it prevents access to the SD card unless the running program has a reason to use it. It also checks the signatures of programs running on the PowerPC, which is why we can't just install homebrew without running a custom firmware (which disables the checks and restrictions). It has a userspace and a kernel; just like the PowerPC.

## The Wii U Environment
Running code on the Wii U can be achieved through several methods.
 - **Browserhax** exploits the Internet Browser to run code. Binaries are loaded into Javascript arrays or MP4 files (depending on the target firmware version) which are then run by the exploit. Code running in the browser is subject to its rather limiting permissions. The method of launching the code also crashes the browser, leaving memory and system libraries in "limbo"; rendering many features unusable. An ELF loader written for the exploit works around the inherent filesize limitations, but other oddities (such as not loading all the ELF, breaking C global variables) make browserhax a far from ideal environment. Luckily, on firmwares with a kernel exploit, better homebrew launching methods are available.
 - **The Homebrew Launcher** is a program (requiring a kernel exploit) that injects itself into Mii Maker. Mii Maker has far more permissions than the Internet Browser, allowing things like SD card access. Since homebrew code is executed before Mii Maker even starts (as if the `main()` function was replaced) the environment is much cleaner since all memory and system libraries are in their initial states; however some Wii U features - such as the home menu overlay and certain library functions - remain unsupported. Homebrew ELF files are loaded from the SD card or over the network (using tools like `wiiload`) into a special memory range (0x00800000) and executed. This memory range is only 8 MiB (including HBL's `sd_loader`), so the homebrew is limited in size. These so-called HBL ELF files were the gold standard in homebrew for years, though limitations in the format and a messy toolchain have allowed newer methods to become a better choice for most homebrew.
 - **The Homebrew Launcher (RPX)** is a slightly different method of loading homebrew with HBL - instead of working with HBL ELF files, the Wii U's native RPX format is used. When loading an RPX file, HBL uses the system's native loader; integrating homebrew apps into the system far more smoothly. This removes most of the restrictions of HBL ELF - executables can be as large as a retail game, and all OS libraries are available. The "wut" toolchain can build RPX files compatible with HBL, providing a much better development experience than traditional HBL ELF toolchains.
 - **Native Channels** are the most integrated homebrew can get into the Wii U's OS. Making use of custom firmwares and wut's ability to build RPX files, homebrew can now be installed as a channel, using the exact same method that the system uses to install eShop games. While this completely integrates homebrew into the system (making it largely idential to retail games) there are some limitations that come with it - kernel access not being guaranteed and binaries being stored in a read-only location, for example. Most RPX homebrew developed for the Homebrew Launcher should also work as a channel without serious modification, though certain tasks (such as self-updating) are different.

The current recommended development target is RPX homebrew, using wut. You can develop against HBL, making use of its convenient network loading; and eventually package your homebrew as a channel, if desired.

###### todo: compilers/languages + setting up wut?

***Chapter 1: That's all folks!***
Head over to Chapter 2 [here](/tutorial/Chapter%202.md).
