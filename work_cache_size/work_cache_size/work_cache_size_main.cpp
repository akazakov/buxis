#include <fmt/format.h>
#include <omp.h>

#include <random>
#include <thread>

#include "iprof/hitime.hpp"
double Multer(std::vector<double>& X, std::vector<double>& Y,
              std::vector<double>& Z) {
  auto sz = X.size();
  double result = 0;
  for (size_t i = 0; i < sz; i++) {
    auto mult = std::max(X[i], Y[i]);
    result += Z[i] * mult;
  }
  return result;
}

std::vector<double> InitVector(size_t size) {
  static std::mt19937_64 rnd(1);
  static std::uniform_real_distribution<double> rd;
  std::vector<double> r(size);
  std::generate(r.begin(), r.end(), [&]() { return rd(rnd); });
  return r;
}

int main() {
  const size_t Xs = 1000;
  const size_t Ys = 1000;
  const size_t Zs = 1000;
  const size_t kNumX = 10;
  const size_t kNumY = 100;
  const size_t kNumZ = 100;
  auto total_work = kNumX * kNumY * kNumZ;
  fmt::println(" KB: X: {},  Y: {}, : Z {}", kNumX * Xs * sizeof(double) / 1000,
               kNumY * Ys * sizeof(double) / 1000,
               kNumZ * Zs * sizeof(double) / 1000);

  fmt::println("Hi");
  std::vector<std::vector<double>> x_v(kNumX, InitVector(Xs));
  std::vector<std::vector<double>> y_v(kNumY, InitVector(Ys));
  std::vector<std::vector<double>> z_v(kNumZ, InitVector(Zs));
  auto result_size = kNumX;
  auto start = HighResClock::now();
  std::vector<double> result(kNumX);
  auto num_threads = std::thread::hardware_concurrency() / 2;
  int n_threads = 0;
  omp_set_num_threads(num_threads);
  {
// #pragma omp parallel for
    for (size_t i = 0; i < kNumX; i++) {
      auto my_thread = omp_get_thread_num();
      // if (my_thread == 0) {
        // fmt::println("{}", i);
      // }
      for (size_t j = 0; j < kNumY; j++) {
        for (size_t k = 0; k < kNumZ; k++) {
          result[i] += Multer(x_v[i], y_v[j], z_v[k]);
        }
      }
    }
  }

  auto end = HighResClock::now();
  auto time = MILLI_SECS(end- start);
  fmt::println("Elapsed: {}ms, total_work: {}, work_per_ms {}", time, total_work, total_work/time);
  fmt::println("Res: {} ", result[0]);
  fmt::println("threads used: {}, wanted:  {}", n_threads, num_threads);
  return 0;
}