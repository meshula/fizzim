// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2003, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ----------------------------------------------------------------------------
//
//
// discrete time simulation clock for SteerTest
//
// Keeps track of real clock time and simulation time.  Encapsulates OS's
// time API.  Can be put in either "as fast as possible" variable time step
// mode (where simulation time steps are based on real time elapsed between
// updates), or in fixed "target FPS" mode where the simulation steps are
// constrained to start on 1/FPS boundaries (e.g. on a 60 hertz video game
// console).  Also handles the notion of "pausing" simulation time.
//
// Usage: allocate a clock, set its "paused" or "targetFPS" parameters,
// then call updateGlobalSimulationClock before each simulation step.
//
// 10-04-03 dp:  made members private; added accessors, removed frameRateSync
// 09-24-02 cwr: major overhaul
// 06-26-02 cwr: created
//
//
// ----------------------------------------------------------------------------


#include "Clock.h"


// ----------------------------------------------------------------------------
// XXX This is a bit ad hoc.  Need to revisit conditionalization on operating
// XXX system.  As of 5-5-03, this module knows about Win32 (code thanks to
// XXX Leaf Garland and Bruce Mitchener) and Linux/Unix (Craig's original
// XXX version).  It tests for Win32 and assumes Linux/Unix otherwise.


#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h> 
#endif


// ----------------------------------------------------------------------------
// Constructor


Clock::Clock (void)
{
    // is simulation running or paused?
    paused = false;

    // real "wall clock" time since launch
    totalRealTime = 0;

    // time simulation has run
    totalSimulationTime = 0;

    // time spent paused
    totalPausedTime = 0;

    // sum of (non-realtime driven) advances to simulation time
    totalAdvanceTime = 0;

    // interval since last simulation time 
    elapsedSimulationTime = 0;

    // interval since last clock update time 
    elapsedRealTime = 0;

    // interval since last clock update,
    // exclusive of time spent waiting for frame boundary when targetFPS>0
    elapsedNonWaitRealTime = 0;

    // "manually" advance clock by this amount on next update
    newAdvanceTime = 0;

    // "Calendar time" in seconds and microseconds (obtained from
    // the OS by gettimeofday) when this clock was first updated
    baseRealTimeSec = 0;
    baseRealTimeUsec = 0;
}


// ----------------------------------------------------------------------------
// update this clock, called once per simulation step ("frame")

void Clock::Update (void)
{
	// save previous real time to measure elapsed time
    const TimeVal previousRealTime = totalRealTime;

    // real "wall clock" time since this application was launched
    totalRealTime = RealTimeSinceFirstClockUpdate ();

    // time since last clock update
    elapsedRealTime = totalRealTime - previousRealTime;

    // accumulate paused time
	if (paused) {
		totalPausedTime += elapsedRealTime;
	}

    // save previous simulation time to measure elapsed time
    const TimeVal previousSimulationTime = totalSimulationTime;

    // update total "manual advance" time
    totalAdvanceTime += newAdvanceTime;

    // new simulation time is total run time minus time spent paused
    totalSimulationTime = (totalRealTime
                           + totalAdvanceTime
                           - totalPausedTime);

    // how much time has elapsed since the last simulation step?
    elapsedSimulationTime = (paused ?
                             newAdvanceTime :
                             (totalSimulationTime - previousSimulationTime));

    // reset advance amount
    newAdvanceTime = 0;
}

// ----------------------------------------------------------------------------
// ("manually") force simulation time ahead, unrelated to the passage of
// real time, currently used only for SteerTest's "single step forward"

void Clock::AdvanceSimulationTime (const TimeVal seconds)
{
	if (seconds > 0) {
		newAdvanceTime += seconds;
	}
}


// ----------------------------------------------------------------------------
// Returns the number of seconds of real time (represented as a float) since
// the clock was first updated.
//
// XXX Need to revisit conditionalization on operating system.
//
// XXX as of 5-5-03: two versions, one for Linux/Unix (by Craig Reynolds) and
// XXX one for Windows (by Leaf Garland).  As Leaf's comment suggests, the
// XXX original version of this function mirrored the Linux/Unix clock model.
// XXX This should be redesigned to be more agnostic to operating system.



// original version:
//
// float Clock::realTimeSinceFirstClockUpdate (void)
// {
//     timeval t;
//     if (gettimeofday (&t, 0) != 0)
//     {
//         SteerTest::errorExit ("Problem reading system clock.\n");
//         return (0);  // won't be reached, but prevents compiler warning
//     }
//     else
//     {
//         // ensure the base time is recorded once after launch
//         if (baseRealTimeSec == 0)
//         {
//             baseRealTimeSec = t.tv_sec;
//             baseRealTimeUsec = t.tv_usec;
//         }

//         // real "wall clock" time since launch
//         return (( t.tv_sec  - baseRealTimeSec) +
//                 ((t.tv_usec - baseRealTimeUsec) / 1000000.0f));
//     }
// }

void clockErrorExit (void)
{
	/// @todo - deal with error here
}


TimeVal Clock::RealTimeSinceFirstClockUpdate ()
#ifdef _WIN32
{
    LONGLONG time, freq;
    if (QueryPerformanceCounter((LARGE_INTEGER *)&time))
    {
        if (QueryPerformanceFrequency((LARGE_INTEGER *)&freq))
        {
            // This is complicated by trying to stick with the original
            // method of storing time as two integers instead of a float.
			totalRealTime = (double)time / (double)freq;
/*            int sec = (int)dtime;
            int usec = (int) ((dtime - sec) * 1000000);
			
            if (baseRealTimeSec == 0)
            {
                baseRealTimeSec = sec;
                baseRealTimeUsec = usec;
            }
            // real "wall clock" time since launch
            return (( sec  - baseRealTimeSec) +
                    ((usec - baseRealTimeUsec) / 1000000.0f));*/
			return totalRealTime;
        }
        clockErrorExit ();
        return 0.0f;
    }
    clockErrorExit ();
    return 0.0f;
}
#else
{
    timeval t;
    if (gettimeofday (&t, 0) != 0)
    {
        clockErrorExit ();
        return (0);  // won't be reached, but avoids compiler warning
    }
    else
    {
        // ensure the base time is recorded once after launch
        if (baseRealTimeSec == 0)
        {
            baseRealTimeSec = t.tv_sec;
            baseRealTimeUsec = t.tv_usec;
        }

        // real "wall clock" time since launch
        return (( t.tv_sec  - baseRealTimeSec) +
                ((t.tv_usec - baseRealTimeUsec) / 1000000.0f));
    }
}
#endif


// ----------------------------------------------------------------------------
