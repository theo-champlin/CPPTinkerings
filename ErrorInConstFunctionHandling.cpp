// The following compiles with GCC 4.9.4.
#include <iostream>
#include <type_traits>

class Counter
{
   int value = 0;
   void Increment()
   {
      static_assert(
         std::is_same<decltype(this), Counter*>::value,
         "'this' pointer does not match expected type.");

      ++value;
   }

public:
   const auto& GetIncrementor() const
   {
      static_assert(
         !std::is_same<decltype(this), const Counter*>::value,
         "'this' pointer does not match expected type.");

      const static auto incrementor = [this] { Increment(); };

      return incrementor;
   }

   void PrintValue() const
   {
      std::cout << "Value: " << value << std::endl;
   }
};

int main()
{
   Counter localCount;
   localCount.GetIncrementor()();
   localCount.PrintValue();
   std::cin.ignore();
}
