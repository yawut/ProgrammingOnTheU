#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include <vpad/input.h>
#include <whb/log_console.h>
#include <whb/log.h>
#include <whb/proc.h>

/*  Note that "DRC" and "Gamepad" both refer to the large tablet controller.
    Calling it a "proto-Switch" or similar will result in me talking at length
    about how good Wind Waker HD is. */

int main(int argc, char** argv) {
    WHBProcInit();

/*  Use the Console backend for WHBLog - this one draws text with OSScreen
    Don't mix with other graphics code! */
    WHBLogConsoleInit();
    WHBLogPrint("A Very Cool And Complete Button Tester");
/*  WHBLog's Console backend won't actually show anything on-screen until you
    call this */
    WHBLogConsoleDraw();

    VPADStatus status;
    VPADReadError error;
    bool vpad_fatal = false;

    while (WHBProcIsRunning()) {
    /*  Read button, touch and sensor data from the Gamepad */
        if (VPADRead(VPAD_CHAN_0, &status, 1, &error)) {
        /*  VPADRead didn't return 0 - what went wrong? */
            switch (error) {
            /*  No data available from the DRC yet - we're asking too often!
                This is really common, and nothing to worry about. */
                case VPAD_READ_NO_SAMPLES: {
                    WHBLogPrint("debug: no samp");
                /*  Just keep looping, we'll get data eventually */
                    continue;
                }
            /*  Either our channel was bad, or the controller is. Since this app
                hard-codes channel 0, we can assume something's up with the
                controller - maybe it's missing or off? */
                case VPAD_READ_INVALID_CONTROLLER: {
                    WHBLogPrint("debug: no cont");
                /*  Not much point testing buttons for a controller that's not
                    actually there */
                    vpad_fatal = true;
                    break;
                }
            /*  If you hit this, good job! As far as we know VPADReadError will
                always be one of the above. */
                default: {
                    WHBLogPrintf("Unknown VPAD error! %08X", error);
                    vpad_fatal = true;
                    break;
                }
            }
        }
    /*  If there was some kind of fatal issue reading the VPAD, break out of
        the ProcUI loop and quit. */
        if (vpad_fatal) break;

    /*  Check the buttons, and log appropriate messages */
        if (status.trigger & VPAD_BUTTON_A) {
            WHBLogPrint("Pressed A!");
        } else if (status.release & VPAD_BUTTON_A) {
            WHBLogPrint("Released A!");
        }
        if (status.trigger & VPAD_BUTTON_B) {
            WHBLogPrintf("Pressed B!");
        } else if (status.release & VPAD_BUTTON_B) {
            WHBLogPrintf("Released B!");
        }
    /*  etc. etc. Other similar buttons:
        - VPAD_BUTTON_X
        - VPAD_BUTTON_Y
        - VPAD_BUTTON_ZL
        - VPAD_BUTTON_ZR
        - VPAD_BUTTON_L
        - VPAD_BUTTON_R
        - VPAD_BUTTON_PLUS (aka Start)
        - VPAD_BUTTON_MINUS (aka Select)
        - VPAD_BUTTON_HOME (this one caught by ProcUI)
        - VPAD_BUTTON_SYNC
        - VPAD_BUTTON_STICK_R (clicking in the right stick)
        - VPAD_BUTTON_STICK_L (clicking in the left stick)
        - VPAD_BUTTON_TV (this one will make the DRC open the TV Remote menu) */

    /*  Check the directions - VPAD_BUTTON_UP is the D-pad, while
        VPAD_STICK_L_EMULATION_UP and VPAD_STICK_R_EMULATION_UP allow you to
        treat the analog sticks like two extra D-pads. Bitwise OR means any
        one of these three will return true. */
        if (status.trigger & (VPAD_BUTTON_UP |
            VPAD_STICK_L_EMULATION_UP | VPAD_STICK_R_EMULATION_UP)) {
            WHBLogPrint("Going up!");
        }
        if (status.trigger & (VPAD_BUTTON_LEFT |
            VPAD_STICK_L_EMULATION_LEFT | VPAD_STICK_R_EMULATION_LEFT)) {
            WHBLogPrint("Going... left?");
        }
    /*  etc, etc. See:
        - VPAD_BUTTON_DOWN
        - VPAD_STICK_L_EMULATION_DOWN
        - VPAD_STICK_R_EMULATION_DOWN
        - VPAD_BUTTON_RIGHT
        - VPAD_STICK_L_EMULATION_RIGHT
        - VPAD_STICK_R_EMULATION_RIGHT */

    /*  There's plenty of other interesting things in the VPADStatus struct,
        like the analog sticks, touch screen, headphone status, volume (yes,
        really) gyroscope, accelerometer, angle sensor etc. etc. See wut's
        documentation for info on all this stuff! */

    /*  Render the log to the screen */
        WHBLogConsoleDraw();
    }

/*  If we get here, ProcUI said we should quit. */
    WHBLogPrint("Quitting... Come back soon!");
    WHBLogConsoleDraw();

    WHBLogConsoleFree();
    WHBProcShutdown();

    return 0;
}
