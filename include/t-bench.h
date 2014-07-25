#ifndef T_BENCH_H
#define T_BENCH_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <initializer_list>
#include <chrono>
#include <iostream>

namespace TBench {
	class Benchmark;
	class Suite;

	typedef std::vector<std::string> Benchmarks;

	/* Timer */

	class Timer {
	private:
		std::chrono::system_clock::time_point _start, _end;

	public:
		typedef long long Time; // ago?

		void start() {
			_start = std::chrono::high_resolution_clock::now();
		}

		void stop() {
			_end = std::chrono::high_resolution_clock::now();
		}

		Time durationInMilliseconds() const {
			auto time = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
			return time;
		}
	};

	/* Case */

	class Case {
	public:
		virtual void registerCase() = 0;
		void operator()();
		virtual ~Case() {}
	};

	inline void Case::operator()() {
		this->registerCase();
	}

	/* Benchmark */

	class Benchmark {
	public:
		typedef std::function<Timer::Time(std::vector<Timer::Time>)> TimerStrategy;
		template <typename ... TCases>
		Benchmark(TimerStrategy strategy, TCases... cases);

	private:
		TimerStrategy _strategy;
		typedef std::vector<std::function<void()>> FuncVector;
		friend class Suite;
		FuncVector _funcs;

		/* Unpacking functions */
		template <typename TCase, typename ... TCases> void unpack(TCase first, TCases... cases);
		template <typename TCase> void unpack(TCase single);
		void unpack() {};

		void run();
	};

	template <typename ... TCases>
	Benchmark::Benchmark(TimerStrategy strategy, TCases... cases)
		: _strategy(strategy) {
		unpack(cases...);
	}

	template <typename TCase, typename ... TCases>
	void Benchmark::unpack(TCase first, TCases... cases) {
		_funcs.push_back([=]() mutable { first(); });
		unpack(cases...);
	}

	template <typename TCase>
	void Benchmark::unpack(TCase single) {
		_funcs.push_back([=]() mutable { single(); });
	}

	inline void Benchmark::run() {
		std::vector<Timer::Time> times;
		int n = 0;

		for (auto f : _funcs) {
			std::cout << "   Case " << n++ << ": ";
			Timer timer;
			timer.start(); 
			f();
			timer.stop();
			times.push_back(timer.durationInMilliseconds());
			std::cout << timer.durationInMilliseconds() << std::endl;
		}

		// TODO: We need nicer output than this:
		std::cout << "   Time: " << this->_strategy(times) << " ms" << std::endl;
	}

	/* Exceptions */

	class BenchmarkException : public std::exception {
	private:
		const std::string _what;

	public:
		explicit BenchmarkException(std::string const& cs)
			: _what(cs) {}

		char const* what() const override {
			return this->_what.c_str();
		}
	};

	/* Suite - Note: This is really not a "true" class. It's just a couple of subroutines, more or less. */

	class Suite {
	private:
		typedef std::unordered_map<std::string, Benchmark> BenchmarkHash;
		static BenchmarkHash _benches;

	public:
		template <typename ... TCases>
		static void AddBenchmark(const std::string& name, Benchmark::TimerStrategy strategy, TCases... cases);
		static void Run(const std::string& name);
		static void Run(const Benchmarks& names);
	};

	Suite::BenchmarkHash Suite::_benches;

	template <typename ... TCases>
	void Suite::AddBenchmark(const std::string& name, Benchmark::TimerStrategy strategy, TCases... cases) {
		_benches.insert(std::make_pair(name, Benchmark(strategy, cases...)));
	}

	inline void Suite::Run(std::string const& name) {
		std::cout << "-- Running benchmark: " << name << std::endl;
		auto bench = _benches.at(name);
		bench.run();
	}

	inline void Suite::Run(Benchmarks const& names) {
		for (auto name : names) Run(name);
	}

	/* Output calculation functions */

	inline Timer::Time TimeDiff(std::vector<Timer::Time> times) {
		if (times.size() != 2)
			throw BenchmarkException("TimeDiff expects two cases");

		return times[1] - times[0];
	}

	inline Timer::Time TotalTime(std::vector<Timer::Time> times) {
		Timer::Time total = 0;
		for (auto t : times) total += t;
		return total;
	}
}

#endif