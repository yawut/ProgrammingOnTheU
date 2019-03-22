# Wii U Programming
A quick thrash for existing programmers

*note: this chapter is still in the draft stage. don't read it*

**Chapter 4 - Gamepad Input**

Now you've gotten a feel for how OSScreen can be used to generate graphics, it's time to let libwhb do all that work and focus on the next thing you'll need to make a cool app - Gamepad input! buttons! We're gonna dip our toes into the world of interactivity with a simple button tester application.

A quick semantic note: I use both the terms "Gamepad" and "DRC" to refer to the reasonably-sized tablet controller that the Wii U is famous for. I don't use "Gamepad" to refer to things like Pro Controllers - they're actually more similar to Wiimotes than the DRC; as we'll see in a future chapter.

As before, I've written some sample code for us to go through together - open up [resource 4.1](/resources/4-1-ButtonTester) and have a look around. I'm gonna move through this one a little faster, and there'll be some practical stuff at the end. Let's go!

## `main.c` - line by line
```c
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include <vpad/input.h>
#include <whb/log_console.h>
#include <whb/log.h>
#include <whb/proc.h>
```
A slightly different set of includes this time, but it's basically the same idea. It's worth noting that the names of the header's lines up with [wut's documentation](https://wut.devkitpro.org/) - so if you're curious about `<vpad/input.h>`, you can open up the wut docs and navigate to Modules->vpad->Input and see what's available! There should be a list of all the structs, functions and enums in that header - and hopefully descriptions of each.

Also note `<whb/log_console.h>` - this pulls in the Console backend for libwhb's logger. As mentioned in Chapter 3, it'll handle OSScreen for you - we'll see how this works shortly.

```c
int main(int argc, char** argv) {
    WHBProcInit();

    WHBLogConsoleInit();
    WHBLogPrint("A Very Cool And Complete Button Tester");
```
Skimming over `WHBProcInit` (check out Chapter 3 for an explanation), the new call of interest here is `WHBLogConsoleInit`. This function sets up OSScreen in a very similar way to how we did it in Chapter 3 - the key difference is that `WHBLogPrint` will add a line of text to the screen. You can use the Console logger alongside the UDP and Cafe backends without any problems.

```c
WHBLogConsoleDraw();
```
Remember how using OSScreen requires you to flush the caches and flip the buffers? Since it'd be really slow if libwhb did this every time you called `WHBLogPrint`, it'll hold off until you call `WHBLogConsoleDraw`. This means that when using the Console backend, log messages *will not show up immediately*. They only get drawn to the screen when `WHBLogConsoleDraw` is called.

We do this here to make the app's welcome message show up right away.

```c
VPADStatus status;
VPADReadError error;
bool vpad_fatal = false;
```
Here we make a few variables to hold key data - the [`VPADStatus`](https://wut.devkitpro.org/group__vpad__input.html#structVPADStatus) struct will hold all the data we read from the Gamepad - as you can see from the docs, there's quite a lot there. `VPADStatus` has buttons, analog sticks, motion controls, touch data, the magnetometer (yes, really), and even the Gamepad's volume and battery levels. We also make room for [`VPADReadError`](https://wut.devkitpro.org/group__vpad__input.html#gaff1e94081b12bc485c6613eb94daccea), an enum that covers the reasons an attempt to read the DRC might go wrong. Last but not least is a bool that gets used for keeping track of whether there's a fatal error - we're gonna use a switch/case inside a loop later, and you know how `break` is.

```c
while (WHBProcIsRunning()) {
    if (VPADRead(VPAD_CHAN_0, &status, 1, &error)) {
```
Here we've got a loop over `WHBProcIsRunning` - pretty standard stuff, Chapter 3 has a refresher. What's really interesting is what's *inside* the loop - a call to [`VPADRead`](https://wut.devkitpro.org/group__vpad__input.html#ga2265b154ad3a6059dc05bda56c8471a6), a function we haven't seen yet! VPAD is Nintendo's library for interacting with the Gamepad, so `VPADRead`, well... reads input data from the Gamepad. Our first parameter is a channel number - some may remember how the Wii U was originally going to have several Gamepads at once, and this is an artifact of that. Since our poor retail consoles can only handle one, it's a safe bet to hard-code this to `VPAD_CHAN_0`. The next parameter is a pointer to a `VPADStatus` struct. It's actually treated as an array, but we're yet to figure out why you'd want more than one buffer - so passing in just one works fine. On that note, parameter 3 is the number of items in the array; one in this case. Last but certainly not least is a pointer to write any error codes to, one of `VPADReadError`. `VPADRead` will fill in all our buffers with data from the Gamepad and return non-zero if something needs our attention - which is why I've put it in an if-check.

So, let's see what happens when something does go wrong:
```c
//TODO
switch (error) {
    case VPAD_READ_NO_SAMPLES: {
        WHBLogPrint("debug: no samp");
        continue;
    }
    case VPAD_READ_INVALID_CONTROLLER: {
        WHBLogPrint("debug: no cont");
        vpad_fatal = true;
        break;
    }
    default: {
        WHBLogPrintf("Unknown VPAD error! %08X", error);
        vpad_fatal = true;
        break;
    }
}
// ...
if (vpad_fatal) break;
```
Remember that this is inside the if-check from before, so we can assume that something has gone wrong. We switch over all the known error codes, and handle each appropriately:
 - `VPAD_READ_NO_SAMPLES` occurs when the console didn't receive any new data between this call to `VPADRead` and our last one. Keep in mind the Gamepad is connected wirelessly, so there's a polling rate and bandwidth limits under the hood - therefore it's entirely possible to check too fast. As you'd guess, this error code comes up all the time, and is nothing to worry about. Here we just `continue` to do loop around again without any consequence - more data will come through eventually.
 - `VPAD_READ_INVALID_CONTROLLER` TODO idk what this actually means, I assume missing gamepad?

Finally, we drop in a `default` case just in case there's error conditions that we don't know about yet. Then we check if there was any fatal errors; before breaking the loop to quit the app if there was - while we don't have to do this, there's not much point for a button tester to keep running if the Gamepad isn't gonna work!

```c
if (status.trigger & VPAD_BUTTON_A) {
    WHBLogPrint("Pressed A!");
} else if (status.release & VPAD_BUTTON_A) {
    WHBLogPrint("Released A!");
}
```
Now we've successfully got some button data, it's time to do something with it! `VPADStatus.trigger` is a bitmask of [`VPADButtons`](https://wut.devkitpro.org/group__vpad__input.html#gac0151a3ca71aa0781a396a89430654a7) - if you're not familiar with bitmasks, it means that every binary bit of `VPADStatus.trigger` corresponds to one button. Thus, we can use a bitwise AND operation (`&`) to extract individual buttons - `VPADStatus.trigger & VPAD_BUTTON_A` will be equal to `VPAD_BUTTON_A` if the corresponding bit in `VPADStatus.trigger` is 1, while the whole thing comes out as 0 otherwise. That is:

*Resource 4.2 - Bitmask Snippet*
```c
VPADStatus status;
//Fill in status
if (status.trigger & VPAD_BUTTON_A == VPAD_BUTTON_A) {
    WHBLogPrint("Triggered A button!");
} else if (status.trigger & VPAD_BUTTON_A == 0) {
    WHBLogPrint("Did not trigger A button!");
} else {
    //this is impossible
}
```
Since the only part of the AND's result we care about is whether or not it's zero, I've simplified the if-checks down to what you see in the code. So, what does `.trigger` actually mean, and what other information do we have?
- `VPADStatus.trigger` is a bitmask of all the buttons that have been pressed down in the time between this call to `VPADRead` and the last one.
- `VPADStatus.release` is a bitmask of all the buttons that have been released in the time between this `VPADRead` call and the one before.
- `VPADStatus.hold` is a bitmask of all the buttons being held at the time of the `VPADRead` call, regardless any previous calls to `VPADRead`.

Since we want this app to do something once when a button is pressed, and once when it's released, checking the `.trigger` and `.release` variables is appropriate. If we wanted to do something constantly while a button is held (like moving a player around), checking `.hold` might be more appropriate.

So, we check if A has been triggered on this poll, and if it hasn't we check if it's been released. We also do the same thing for the B button:
```c
if (status.trigger & VPAD_BUTTON_B) {
    WHBLogPrintf("Pressed B!");
} else if (status.release & VPAD_BUTTON_B) {
    WHBLogPrintf("Released B!");
}
```

Next, we check on the directions:
```c
if (status.trigger & (VPAD_BUTTON_UP |
    VPAD_STICK_L_EMULATION_UP | VPAD_STICK_R_EMULATION_UP)) {
    WHBLogPrint("Going up!");
}
if (status.trigger & (VPAD_BUTTON_LEFT |
    VPAD_STICK_L_EMULATION_LEFT | VPAD_STICK_R_EMULATION_LEFT)) {
    WHBLogPrint("Going... left?");
}
```

This is a slightly more advanced use of bitmasks - we can check for several bits at once by using a bitwise OR (`|`) to combine all the buttons we're checking for. If any one of the bits we OR together is set, we'll get a non-zero value out the other side!

So, in the first case we're seeing if *any of* `VPAD_BUTTON_UP`, `VPAD_STICK_L_EMULATION_UP`, `VPAD_STICK_R_EMULATION_UP` are set. The "emulation" directions are generated from the two analog sticks - if you push the left analog stick up, then `VPAD_STICK_L_EMULATION_UP` will become set. Pretty neat! Of course, the proper analog values are also available in `VPADStatus`, so be sure you're only using either the emulation or the analog values.

```c
WHBLogConsoleDraw();
```
Finally, since we've been printing a lot of text to the Console logger, we should be sure to call `WHBLogConsoleDraw` to make sure it actually makes its way onto the screen.

```c
}
```
That's all, folks! At this point the app will loop back around and continue checking for new input from the Gamepad, showing a message if anything happens. If we break out of this loop, it means we should quit, so let's check that out:

```c
    WHBLogPrint("Quitting... Come back soon!");
    WHBLogConsoleDraw();

    WHBLogConsoleFree();
    WHBProcShutdown();

    return 0;
}
```
Nothing of particular interest, except for the call to `WHBLogConsoleFree` - don't forget to do this! Return something and we're done!

## Suggestions and Further Learning
After you've given this a try (compiling/running instructions in Chapter 3!), it'll be your turn to write some code! Remember, you can't ever break your console, and you can always download a fresh copy of the code if your changes break it. Here's some suggestions for things you might do:

 - Despite my claims, this app is far from a complete button tester. You can see a list of the buttons I've missed spread through a few comments in the code, or you can check on wut's [`VPADButtons`](https://wut.devkitpro.org/group__vpad__input.html#gac0151a3ca71aa0781a396a89430654a7) documentation for a list. Why not add a few more?
 - There's plenty more information than just buttons in [`VPADStatus`](https://wut.devkitpro.org/group__vpad__input.html#structVPADStatus). Try doing something with the analog `.leftStick` and `.rightStick` values - I'll leave the specifics up to your imagination!
 - Did you mess around with [`OSScreenPutPixelEx`](https://devkitpro.github.io/wut/group__coreinit__screen.html#ga3f4b6594fdc62b57e5ceb6cdc0e57d5a) at the end of Chapter 3? It would be cool to graphically show the position of the analog sticks; but libwhb's Console backend doesn't allow us to do custom graphics - try adding Gamepad reading to your Chapter 3 code, so you can draw stuff depending on buttons or analog values. *(note: if you do this, you're basically ready to write a simple game, if you want)*
 - It might be fun to look into the motion control data: values like `.accelorometer`, `.gyro`, `.angle`, `.direction` or `.mag`. I've no idea how most of them work, so let  me know what you find!

***Chapter 4: Thanks for the fish!*** Chapter 5 isn't quite ready yet - please check back later! I'll be sure to mention it on my social accounts; [pick your platform](https://heyquark.com/aboutme/) if you're into that.
