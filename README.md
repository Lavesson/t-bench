This is the home of `t-bench`, a benchmarking utility for C++ programs.

The name is simply on the same form as my former library `t-unit` (initially `tap-unit`) and 
I wrote it because I needed a simple way to benchmark different scenarios in a 3D engine.

### Feature overview

* The ability to measure and output the total time one or more test cases take
* Running two different test cases and outputting the time difference between them

### Would-like features

* Repeating a certain benchmark a number of times, outputting the average time for all runs
* Tracking memory usage (this one might be hard to implement in a sane manner, but it would be extremely nice to have)

### Usage

#### Example 1 - Using lambdas

The simplest way to use `t-bench` is by passing lambdas as test cases:

	#include <t-bench.h>
	using TBench::Suite;
	using TBench::Benchmarks;

	int main() {
		// Call Suite::AddBenchmark to create a benchmark. The first param 
		// is the name, the second is a function to use when producing 
		// output. The rest of the params are test cases:
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

#### Example 2 - Using `Case` instances

The test case parameter passing to `AddBenchmark` is implemented using variadic templates, so
there's no explicit type enforcement. Basically, what it expects, is something callable. So any
lambda, functor or function pointer will do.

To make things simpler, there's an abstract functor base class (`TBench::Case`) that you can use
by overriding the pure virtual function `registerCase`:

	#include <t-bench.h>
	using TBench::Suite;
	using TBench::Benchmarks;

	class FibonacciRecursive : public TBench::Case {
		const int _n;

		void registerCase() override {
			fib(_n);
		}

		int fib(int n) {
			return 
				n == 1 || n == 2 ? 1 : fib(n - 1) + fib(n - 2);
		}

	public:
		explicit FibonacciRecursive(int n)
			: _n(n) {}
	};
	
You can then use this in the following way:

	Suite::AddBenchmark("Recursive fibonacci", TBench::TotalTime, 
		FibonacciRecursive(10), FibonacciRecursive(20), FibonacciRecursive(30), FibonacciRecursive(40));
	
	Suite::Run("Recursive fibonacci");

You could, of course, mix lambdas and case instances too. 

Sample output from this example:

	-- Running benchmark: My first benchmark
	Case 0: 2000
	Case 1: 3000
	Time: 5000 ms
	Press any key to continue . . .

#### Example 3 - Time measurement functions

To calculate the difference in time between two cases, use `TBench::TimeDiff`
instead of `TBench::TotalTime`:

	Suite::AddBenchmark("Recursive vs iterative", TBench::TimeDiff, 
		FibonacciRecursive(30), FibonacciIterative(30));
		
The only constraint is that when `TBench::TimeDiff` is used, you're expected to
pass exactly two test cases, or an exception of type `TBench::BenchmarkException`
will get thrown.

If neither `TBench::TimeDiff` or `TBench::TotalTime` works for you, you could
write your own. The simplest way is to pass a lambda instead:

	#include <t-bench.h>
	using TBench::Suite;
	using TBench::Benchmarks;
	using TBench::Timer;

	int main() {
		auto f = FibonacciRecursive(37);
		
		// Do 4 runs of f(37) and calculate the mean time
		Suite::AddBenchmark("Fibonacci mean time", [](std::vector<Timer::Time> times) {
			Timer::Time total = 0;
			for (auto t : times) total += t;
			return total / times.size();
		}, f, f, f, f);
		
		Suite::Run("Fibonacci mean time");
	}
	
#### Example 4 - Running several benchmarks at once

So far, we've used the following code to run benchmarks:

	Suite::Run("Fibonacci mean time");

Your might have registered and want to run more than just one benchmark.
For those cases where you simply want to run everything, use `RunAll`:

	Suite::RunAll();
	
In some cases though, you might have different sets of benchmarks that
you want to run independently of each other. In that case, we can call
`Run` in the following way instead:

	Suite::Run(Benchmarks {
		"Fibonacci mean time",
		"Recursive vs iterative",
		"Recursive fibonacci",
	});
	
`Benchmarks` is simply a `typedef` for `std::vector<TBench::Benchmark>`

### Compiler info

#### MSVC++

`t-bench` makes use of a lot of C++11 features. Most notably lambdas, variadic templates and initializer lists. 
You can compile it using VS2013 (The nov. CTP of 2012 might work. I haven't tried). 

#### Other compilers and operating systems

I haven't gotten around to trying to compile using GCC or other compilers and/or OS:es. It might compile, and if 
it doesn't, it's probably going to be minor fixes.

### Contributing

Contributions by pull requests are welcome.
