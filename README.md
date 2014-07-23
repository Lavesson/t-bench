This is the home of `t-bench`, a benchmarking utility for C++ programs.
Well, at least that's the idea. There's nothing concrete yet.

The name is simply on the same form as my former library `t-unit` (initially `tap-unit`)

I need a benchmark utility for a 3D engine I'm constructing. Nice-to-have would be:

* The ability to output the total time a certain piece of code would take to execute
* Being able to run two different cases and outputting the time difference between them
* Repeating a certain case a number of times, outputting the average time for all runs
* Tracking memory usage (this one might be hard to implement in a sane manner, but it would be extremely nice to have)

I'm imagining something along the following lines:

	#include <t-bench.h>

	/* Benchmarks */

	BENCHMARK(TotalLoadingTime) {
		OUTPUT_TIME(case1);
	}

	BENCHMARK(LoadingTimeDiff) {
		OUTPUT_TIME_DIFF(case1, case2);
	}

	/* Cases */

	CASE(case1) {
		// Benchmark code 1
	}

	CASE(case2) {
		// Benchmark code 2
	}

There's a couple of things to think through: For instance, an application such as a game engine will use a render window. As an effect (on Windows at least) we'll never get a console window (where we would probably like to output results). Luckily, opening up a console window doesn't seem to be too hard:

http://justcheckingonall.wordpress.com/2008/08/29/console-window-win32-app/

Contributions by pull requests are welcome.
