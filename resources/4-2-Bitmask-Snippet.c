VPADStatus status;
//Fill in status
if (status.trigger & VPAD_BUTTON_A == VPAD_BUTTON_A) {
    WHBLogPrint("Triggered A button!");
} else if (status.trigger & VPAD_BUTTON_A == 0) {
    WHBLogPrint("Did not trigger A button!");
} else {
    //this is impossible
}
