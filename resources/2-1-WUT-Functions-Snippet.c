/* ProgrammingOnTheU - WUT function example snippet
 * See Chapter 2 for details.
 */

//include the wut header containing the function we want.
//Documentation is available for these headers - we'll get to that later.
#include <coreinit/thread.h>

//All the coreinit thread-related functions are now available.
//Let's use OSSleepTicks to sleep our thread for some time.
OSSleepTicks(5000);

//that's it! Let's add another header with macros to convert time units.
#include <coreinit/time.h>

//We now have a bunch of useful time-related macros.
//Let's use one to sleep for 5 seconds.
OSSleepTicks(OSSecondsToTicks(5));
