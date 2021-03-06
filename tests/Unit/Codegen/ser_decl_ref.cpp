XFAIL: *
// RUN: %cxxamp -emit-llvm -S -c %s -o -|%cppfilt|%FileCheck %s
//Serialization object decl
#include <cstdlib>
namespace hc {
class Serialize {
 public:
  void Append(size_t x, const void *s);
};
}

class base{
 public:
  __attribute__((annotate("user_deserialize"))) /* For compiler */
  base(int a_,float b_) [[cpu, hc]] :a(a_), b(b_) {}
  int cho(void) [[hc]];
  int a;
  float b;
};
class baz {
 public:
#if 0 // This declaration is supposed to be generated
  __attribute__((annotate("deserialize"))) /* For compiler */
  baz(base&, int foo) [[hc]];
#endif
  int cho(void) [[hc]] { return 0; };

  base &B; //  reference object is not allowed in amp codes
  int bar;
};

int kerker(void) [[cpu, hc]] {
  base b(1234, 0.0f);
  // Will pass if deserializer declaration and definition are generated
  baz bl(b, 1);
  hc::Serialize s;
  bl.__cxxamp_serialize(s);
  return bl.cho();
}
// The definition should be generated by clang
// CHECK: define {{.*}}void @baz::__cxxamp_serialize(hc::Serialize&)
// CHECK: call void @base::__cxxamp_serialize(hc::Serialize&)
