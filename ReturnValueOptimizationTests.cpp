#include <array>
#include <iostream>

class Noisy
{
public:
   constexpr Noisy(bool) {}
   Noisy() { std::cout << "Constructor" << std::endl; }
   Noisy(const Noisy&) { std::cout << "Copy constructor" << std::endl; }
   Noisy(Noisy&&) { std::cout << "Move constructor" << std::endl; }
};

Noisy RVO()
{
   std::cout << __FUNCTION__ << std::endl;
   return {};
}

Noisy NRVO()
{
   std::cout << __FUNCTION__ << std::endl;
   Noisy namedNoisyObject;
   return namedNoisyObject;
}

Noisy NRVOFromConst()
{
   std::cout << __FUNCTION__ << std::endl;
   const Noisy namedNoisyObject;
   return namedNoisyObject;
}

Noisy Branching(bool flag)
{
   std::cout << __FUNCTION__ << std::endl;
   Noisy namedNoisyObject;
   if (flag) return {};
   return namedNoisyObject;
}

Noisy BranchingCopy()
{
   std::cout << __FUNCTION__ << std::endl;
   const Noisy namedNoisyObject;
   if (false) return {};
   return namedNoisyObject;
}

Noisy PostBranchCopy()
{
   std::cout << __FUNCTION__ << std::endl;
   if (false) return {};
   const Noisy namedNoisyObject;
   return namedNoisyObject;
}

Noisy PostBranchNRVOOrMove()
{
   std::cout << __FUNCTION__ << std::endl;
   if (false) return {};
   Noisy namedNoisyObject;
   return namedNoisyObject;
}

// clang output:
//    FromFunctionRVO
//    RVO
//    Constructor
//
// vc++ output:
//    FromFunctionRVO
//    RVO
//    Constructor
//    Move Constructor
Noisy FromFunctionRVO()
{
   std::cout << __FUNCTION__ << std::endl;
   return RVO();
}

// clang output:
//    FromFunctionNRVO
//    RVO
//    Constructor
//
// vc++ output:
//    FromFunctionNRVO
//    RVO
//    Constructor
//    Copy constructor
//    Move constructor
Noisy FromFunctionNRVO()
{
   std::cout << __FUNCTION__ << std::endl;
   const Noisy namedNoisyObject{ RVO() };
   return namedNoisyObject;
}

// clang output:
//    RefFromFunctionCopy
//    RVO
//    Constructor
//    Copy constructor
//
// vc++ output:
//    RefFromFunctionCopy
//    RVO
//    Constructor
//    Copy constructor
//    Move constructor
Noisy RefFromFunctionCopy()
{
   std::cout << __FUNCTION__ << std::endl;
   const Noisy& namedNoisyObject{ RVO() };
   return namedNoisyObject;
}

// clang output:
//    RRefFromFunctionCopy
//    RVO
//    Constructor
//    Copy constructor
//
// vc++ output:
//    RRefFromFunctionCopy
//    RVO
//    Constructor
//    Copy constructor
//    Move constructor
Noisy RRefFromFunctionCopy()
{
   std::cout << __FUNCTION__ << std::endl;
   const auto&& namedNoisyObject{ RVO() };
   static_assert(std::is_rvalue_reference<decltype(namedNoisyObject)>::value, "Expected rvalue");
   return namedNoisyObject;
}

// clang output:
//    FromConstexpr
//
// vc++ output:
//    FromConstexpr
//    Copy constructor
Noisy FromConstexpr()
{
   std::cout << __FUNCTION__ << std::endl;
   constexpr Noisy namedNoisyObject{ true };
   return namedNoisyObject;
}

Noisy FromStaticConstexpr()
{
   std::cout << __FUNCTION__ << std::endl;
   static constexpr Noisy namedNoisyObject{ true };
   return namedNoisyObject;
}

int main()
{
   const auto branchingRVO = [] () -> auto { return Branching(true); };
   const auto branchingMove = [] () -> auto { return Branching(false); };

   std::array<Noisy(*)(), 13> noisyFunctions
   {
      RVO,
      NRVO,
      NRVOFromConst,
      branchingRVO,
      branchingMove,
      BranchingCopy,
      PostBranchCopy,
      PostBranchNRVOOrMove,
      FromFunctionRVO,
      FromFunctionNRVO,
      RefFromFunctionCopy,
      RRefFromFunctionCopy,
      FromConstexpr
   };

   for (auto&& func : noisyFunctions)
   {
      auto noisyObject = (*func)();
      std::cout << std::endl;
   }

   std::cin.ignore();
}
