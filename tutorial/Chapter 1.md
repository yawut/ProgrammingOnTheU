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

## Setting up the build environment
The environment required to build software for the Wii U (more specifically, using wut) consists of a compiler toolchain (in our case, devkitPPC is the recommended toolchain) as well as the wut programs and libraries. The process of installing this software is relatively well automated and can be performed in a few simple steps.

Please note that while devkitPPC is supported on Windows, wut is not. If you wish to use Windows to perform development activity, you may wish to install Ubuntu or another GNU/Linux distribution from the Microsoft Store app, and perform these instructions within the virtualised Linux subsystem. If you natively use a GNU/Linux distribution or macOS, you should be able to follow these instructions.

### Step 1: Install CMake
CMake is a tool that is used to generate build files for a project (e.g. a Makefile, Visual Studio project file, etc.) from a higher-level scripting language. CMake is a core part of the wut build system, and for most software that uses wut, CMake is required to perform the build.

CMake is well supported by package vendors and is easy to install. On a GNU/Linux distribution, you can usually install it using your platform's package manager, e.g. for Debian/Ubuntu/Raspbian/etc. -

```
$ sudo apt-get update
$ sudo apt-get install cmake
```

On macOS, if you have [Homebrew](https://brew.sh/) or [MacPorts](https://macports.org/) installed, you can install it using the respective command line tools. Otherwise, CMake is available as a graphical installer from https://cmake.org/download/ (scroll down to "Binary distributions").

### Step 2: Install the devkitPro package manager
The devkitPPC compiler toolchain is part of a larger set of software programs called devkitPro, which provides toolchains for development of homebrew for platforms such as the Nintendo 3DS, Switch, GameCube and Wii, as well as others. As such, devkitPro has its own package manager, `dkp-pacman`, which can be installed relatively easily, and can be used to configure the development environment to your liking.

For people using macOS or a Debian-based GNU/Linux distribution (e.g. Ubuntu, Raspbian) `dkp-pacman` can be installed by downloading and running the installer from its [download page](https://github.com/devkitPro/pacman/releases/latest). For other GNU/Linux distributions (especially Arch), you may already have the standard `pacman` installed, or you may be able to install `pacman`, which can be configured to read from the devkitPro repositories, as per the instructions on the [devkitPro wiki](https://devkitpro.org/wiki/devkitPro_pacman).

### Step 3: Install devkitPPC and other tools
Now that `dkp-pacman` is installed (or you've configured `pacman` to search the devkitPro repositories) make sure that the list of packages is up-to-date.

(Note: make sure that you use the correct program for your system - `dkp-pacman` or the standard `pacman`)

```
$ sudo dkp-pacman -Sy
```

At any point after the installation, you can upgrade all installed packages to the latest version with:

```
$ sudo dkp-pacman -Syu
```

Now, the packages that you want installed to perform Wii U homebrew development include `devkitPPC` (a compiler and binary utilities for the PowerPC processor variants used in the Nintendo GameCube, Wii and Wii U) and `wiiload` (used to remotely launch a program on a console running the Homebrew Launcher).

```
$ sudo dkp-pacman -S devkitPPC wiiload
```

Once these are installed, you need to export environment variables so that the build system knows where the compiler and support libraries are located. On GNU/Linux, this is provided automatically through the `devkit-env` package, so install it as well.

```
$ sudo dkp-pacman -S devkit-env
```

If you are on macOS, you will need to export these variables manually. Open `~/.bash_profile` in your favourite text editor (if it doesn't exist, create it) and add the following lines to it:

```
export DEVKITPRO=/opt/devkitpro
export DEVKITPPC=$DEVKITPRO/devkitPPC
export PATH=$DEVKITPPC/bin:$DEVKITPRO/tools/bin:$PATH
```

### Step 4: Set up Fling (Wii U-specific package repository)
In addition to the official devkitPro software packages, we have our own package repository (fling.heyquark.com) which contains many packages specific to Wii U development.

The process of setting up Fling is documented on its [GitLab repository page](https://gitlab.com/QuarkTheAwesome/wiiu-fling/blob/master/README.md#wiiu-fling).

### Step 5: Installing wut
With Fling installed, installing wut is very easy.

For GNU/Linux and Windows Subsystem for Linux users:

```
$ sudo dkp-pacman -S wut-linux
```

And for macOS users:

```
$ sudo dkp-pacman -S wut-osx
```

In both cases, this will install wut into `/opt/wut`. The wut build system requires that the environment variable `WUT_ROOT` be set to this directory - again, this is handled automatically on GNU/Linux systems, but if you're using macOS, add this to your `~/.bash_profile`:
```
export WUT_ROOT=/opt/wut
export PATH=$WUT_ROOT/bin:$PATH
```

And that's it. As of your next shell session, you should have access to all necessary components of the Wii U build system.

```
$ powerpc-eabi-gcc -v
...
gcc version 8.2.0 (devkitPPC release 33)

$ wiiload
wiiload v0.5.1
coded by dhewg
...

$ elf2rpl
elf2rpl <options> src dst
...
```

***Chapter 1: That's all folks!***
Head over to Chapter 2 [here](/tutorial/Chapter%202.md).
