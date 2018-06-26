#include <functional>
#include <iostream>
#include <type_traits>
#include <utility>

using std::cout;
using std::endl;

template<typename Callable>
class Wrapper
{
   Callable f;
public:
   Wrapper(Callable&& f) : f{ std::forward<Callable>(f) }
   {
      cout << "No decay occurred." << endl;
   }

   template<typename ...Arguments>
   auto operator()(Arguments&&... args)
   {
      return f(std::forward<Arguments>(args)...);
   }
};

template<>
class Wrapper<void(*)()>
{
   void(*f)();
public:
   Wrapper(void(*f)()) : f{ f } {}
   void operator()() { (*f)(); }
};

enum CallableType
{
   Lambda,
   CapturingLambda,
   Function
};

template<CallableType>
auto ScopeTest()
{
   return Wrapper{ 0 };
}

template<>
auto ScopeTest<Lambda>()
{
   return Wrapper{ [] { cout << "Lambda Scope still around" << endl; } };
}

template<>
auto ScopeTest<CapturingLambda>()
{
   constexpr const char output[]{ "Capturing lambda scope still around" };
   return Wrapper{ [output] { cout << output << endl; } };
}

template<>
auto ScopeTest<Function>()
{
   return Wrapper{ std::function([] { cout << "Function scope still around" << endl; }) };
}

template<unsigned value, typename Type>
struct unroll_wrapper
{
   static void unroll()
   {
      ScopeTest<static_cast<Type>(value)>()();
      unroll_wrapper<value - 1, Type>::unroll();
   }
};

template<typename Type>
struct unroll_wrapper<0, Type>
{
   static void unroll()
   {
      ScopeTest<static_cast<Type>(0)>()();
   }
};

void Call(void(*callable)())
{
   (*callable)();
}

constexpr void DecayTest()
{
   constexpr auto noncapturingLambda = [] { cout << "Lambda decayed" << endl; };
   Call(noncapturingLambda);
}

int main()
{
   unroll_wrapper<static_cast<unsigned>(Function), CallableType>::unroll();
   DecayTest();
}