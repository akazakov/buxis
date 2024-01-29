#include <nanobench.h>

struct Arg16 {
    int a01;
    int a02;
    int a03;
    int a04;
    int a05;
    int a06;
    int a07;
    int a08;
    int a09;
    int a10;
    int a11;
    int a12;
    int a13;
    int a14;
    int a15;
};

__attribute__((noinline)) int FunctionWith16Args(
  int a00, int a01, int a02, int a03, int a04, int a05, int a06, int a07,
  int a08, int a09, int a10, int a11, int a12, int a13, int a14, int a15) {
  if (a00 > 0) {
    return a00 + a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 + a09 + a10 +
           a11 + a12 + a13 + a14 + a15;
  } else {
    return FunctionWith16Args(--a00, a01, a02, a03, a04, a05, a06, a07, a08,
                              a09, a10, a11, a12, a13, a14, a15);
  }
}
__attribute__((noinline)) int FunctionWith16ArgsByPTR(int a00, int a01, int a02,
                                                      int a03, int a04, int a05,
                                                      int a06, int a07,
                                                      Arg16& args) {
  if (a00 > 0) {
    return a00 + a01 + a02 + a03 + a04 + a05 + a06 + a07 + args.a08 + args.a09 +
           args.a10 + args.a11 + args.a12 + args.a13 + args.a14 + args.a15;
  } else {
    a00--;
    return FunctionWith16ArgsByPTR(--a00, a01, a02, a03, a04, a05, a06, a07,
                                   args);
  }
}

int main() {
  const auto kNumCalls = 10000;
  auto bench = ankerl::nanobench::Bench().epochIterations(1000).relative(true);
  int count = 0;

  bench.run("Function w/ 16 args", [&] {
    for (int i = 1; i < kNumCalls; i++) {
      count +=
        //        FunctionWith16Args(i, i, i, i, i, i, i, i, i, i, i, i, i, i,
        //        i, i);
        FunctionWith16Args(i, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
    }
  });
  bench.run("Function w/ Struct 16", [&] {
    auto args = Arg16{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    for (int i = 1; i < kNumCalls; i++) {
      count += FunctionWith16ArgsByPTR(i, 1, 1, 1, 1, 1, 1, 1, args);
    }
  });
}