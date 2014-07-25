This is the home of `t-bench`, a benchmarking utility for C++ programs.

The name is simply on the same form as my former library `t-unit` (initially `tap-unit`) and I wrote it because I needed a simple way to benchmark different scenarios in a 3D engine.

### Feature overview

* The ability to measure and output the total time one or more test cases take
* Running two different test cases and outputting the time difference between them

### Would-like features

* Repeating a certain benchmark a number of times, outputting the average time for all runs
* Tracking memory usage (this one might be hard to implement in a sane manner, but it would be extremely nice to have)

### Usage

The simplest way to use `t-bench` is by passing lambdas as test cases:

	int main() {
		// Call Suite::AddBenchmark to create a benchmark 
		// The first param is the name, the second is a function to use when producing output. The rest of the params are
	// test cases:
		Suite::AddBenchmark("My first benchmark", TBench::TotalTime, 
			[]{ std::this_thread::sleep_for(std::chrono::seconds(2)); },
			[]{ std::this_thread::sleep_for(std::chrono::seconds(3)); }
	);

		// Will output 5000 ms as total time:
		Suite::Run("My first benchmark");
	}

Sample output from this code:

	-- Running benchmark: My first benchmark
	   Case 0: 2000
	   Case 1: 3000
	   Time: 5000 ms

### Compiler info

#### MSVC++

`t-bench` makes use of a lot of C++11 features. Most notably lambdas, variadic templates and initializer lists. You can compile it using VS2013 (The nov. CTP of 2012 might work. I haven't tried). 

#### Other compilers and operating systems

I haven't gotten around to trying to compile using GCC or other compilers and/or OS:es. It might compile, and if it doesn't, it's probably going to be minor fixes.

### Contributing

Contributions by pull requests are welcome.
