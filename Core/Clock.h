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
// Keeps track of real clock time and simulation time.  Encapsulates the time
// API of the underlying operating system.  Can be put in either "as fast as
// possible" variable time step mode (where simulation time steps are based on
// real time elapsed between updates), or in fixed "target FPS" mode where the
// simulation steps are constrained to start on 1/FPS boundaries (e.g. on a 60
// hertz video game console).  Also handles the notion of "pausing" simulation
// time.
//
// Usage: allocate a clock, set its "paused" or "targetFPS" parameters, then
// call updateGlobalSimulationClock before each simulation step.
//
// 10-04-03 dp:  made members private; added accessors
// 09-24-02 cwr: major overhaul
// 06-26-02 cwr: created
//
//
// ----------------------------------------------------------------------------


#ifndef OPENSTEER_CLOCK_H
#define OPENSTEER_CLOCK_H

#ifdef WIN32
typedef double TimeVal;
#else
typedef float TimeVal;
#endif

class Clock
{
public:
    // constructor
    Clock ();

    // ------------------------------------------------------- external API

	// ----------------------------------------------------------------------------
	/** update this clock, call once per simulation step ("frame") to:
	     track passage of real time
	     manage passage of simulation time (modified by Paused state)
	     measure time elapsed between time updates ("frame rate") */
    void update ();

	void reset();

    /// returns the number of seconds of real time (represented as a float)
    /// since the clock was first updated.
    TimeVal realTimeSinceFirstClockUpdate ();

	/// returns time taking into account paused time
	TimeVal getSimulationTime() const { return totalSimulationTime; }

	/// returns the total amount of time spent in the paused state
	TimeVal getTotalPausedTime() const { return totalPausedTime; }

	/// set and get the pause condition
	void setPausedState (bool val) { paused = val; }
	bool getPausedState () const { return paused; }

    /// "manually" force simulation time ahead, unrelated to the passage of
    /// real time.
	/// @param seconds The period to advance the clock. Only positive values have an effect
    void advanceSimulationTime (float seconds);

    /// wait until next frame time (optional functionality)
    void frameRateSync ();

	/// set target frame rate for frameRateSync, 0 means as fast as possible
	void setFrameRateTarget (int fps) { targetFPS = fps; }
	int  getFrameRateTarget () const { return targetFPS; }

private:
    // is simulation running or paused?
    bool paused;

    /// the desired rate of frames per second,
    /// or zero to mean "as fast as possible"
	int targetFPS;

    // ---------------------------------- treat these as "read only" state

    /// real "wall clock" time since launch
    TimeVal totalRealTime;

    /// total time simulation has run
    TimeVal totalSimulationTime;

    /// total time spent paused
    TimeVal totalPausedTime;

    /// sum of (non-realtime driven) advances to simulation time
    TimeVal totalAdvanceTime;

    /// interval since last simulation time
    /// (xxx does this need to be stored in the instance? xxx)
    TimeVal elapsedSimulationTime;

    /// interval since last clock update time 
    /// (xxx does this need to be stored in the instance? xxx)
    TimeVal elapsedRealTime;

    /// interval since last clock update,
    /// exclusive of time spent waiting for frame boundary when targetFPS>0
    TimeVal elapsedNonWaitRealTime;

    /// "manually" advance clock by this amount on next update
    TimeVal newAdvanceTime;

    // "Calendar time" when this clock was first updated
#ifdef _WIN32
    // from QueryPerformanceCounter on Windows
	__int64 basePerformanceCounter;					// use __int64 to avoid including windows.h for LONGLONG
#else
    // from gettimeofday on Linux and Mac OS X
    int baseRealTimeSec;
    int baseRealTimeUsec;
#endif
};


// ----------------------------------------------------------------------------
#endif // OPENSTEER_CLOCK_H
