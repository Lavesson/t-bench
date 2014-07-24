#ifndef T_BENCH_H
#define T_BENCH_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <chrono>

namespace TBench {
	class Benchmark;
	class Suite;

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
		virtual void Register() = 0;
		void operator()();
		virtual ~Case() {}
	};

	inline void Case::operator()() {
		this->Register();
	}

	/* Benchmark */

	class Benchmark {
	public:
		template <typename ... TCases>
		Benchmark(TCases... cases) {
			unpack(cases...);
		}

	private:
		typedef std::vector<std::function<void()>> FuncVector;
		friend class Suite;
		FuncVector _funcs;

		/* Unpacking functions */
		template <typename TCase, typename ... TCases> void unpack(TCase first, TCases... cases);
		template <typename TCase> void unpack(TCase single);
		void unpack() {};

		void run();
	};

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
		for (auto f : _funcs) f();
	}

	/* Suite - Note: This is really not a "true" class. It's just a couple of subroutines, more or less. */
	class Suite {
	private:
		typedef std::unordered_map<std::string, Benchmark> BenchmarkHash;
		static BenchmarkHash _benches;

	public:
		template <typename ... TCases>
		static void AddBenchmark(const std::string& name, TCases... cases);
		static void Run(const std::string& name);
	};

	Suite::BenchmarkHash Suite::_benches;

	template <typename ... TCases>
	void Suite::AddBenchmark(const std::string& name, TCases... cases) {
		_benches.insert(std::make_pair(name, Benchmark(cases...)));
	}

	inline void Suite::Run(std::string const& name) {
		auto bench = _benches.at(name);
		bench.run();
	}
}

#endif