# Wii U Programming
A quick thrash for existing programmers

###### todo: make a nice introduction

## Prerequisites
 - A Wii U and a computer
 - Fairly decent C or C++ experience (Personally, I used knowledge of Java to pick up C as I went. I don't recommend it.)
 - Facilities to compile homebrew (if you can compile Loadiine you can compile anything)
 - The right mindset (exactly what this means is up to you)

## A Quick Recap of Computer Science
Just in case you're a bit behind on your computer science, we're going to quickly tear through some basics so we're all up to speed. Ready? Let's go!

Any computer has a processor, some memory and various I/O things. The processor reads instructions from memory and executes them. Each instruction might be telling the processor to read something from memory, add some numbers, whatever. Memory (specifically RAM) is a spot where the processor can store data, and lots of it. Each byte of memory has an "address"; counted in hex (0 up to F). Due to the way computers are wired up inside, different memory addresses can often refer to different chips on the motherboard; the Wii U has several memory chips that all have their own address "ranges" (for example, 0xF4000000 to 0xF6000000 is the "MEM1" chip). These ranges are often not the same as the way the machine is wired up; processors usually have some method of "mapping" addresses.

That's a lot to take in, but if you can get it (especially memory addressing) you'll find that low-level poking on the Wii U comes much easier.