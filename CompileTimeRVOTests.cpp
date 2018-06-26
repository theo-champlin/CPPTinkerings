#include <array>
#include <iostream>

struct Test
{
   constexpr Test() : data{}, copied{ 0 }, moved{ 0 } {};

   constexpr Test(const Test& other) :
      data{ other.data },
      copied{ other.copied + 1 },
      moved{ other.moved } {};

   constexpr Test(Test&& other) :
      data{ std::move(other.data) },
      copied{ other.copied },
      moved{ other.moved + 1 } {};

   std::array<char, 8192> data;
   unsigned copied;
   unsigned moved;
};

constexpr Test TestFromUnnamed()
{
   return {};
}

constexpr Test TestFromLocal()
{
   constexpr Test localTest{};
   return localTest;
}

constexpr Test TestFromFunc()
{
   constexpr auto localCopy = TestFromUnnamed();
   return localCopy;
}

int main()
{
   constexpr std::array<std::pair<Test(*)(), const char*>, 4> testCalls
   {{
      { TestFromUnnamed, "TestFromUnnamed" },
      { TestFromLocal, "TestFromLocal" },
      { TestFromFunc, "TestFromFunc" }
   }};

   for (auto&& func : testCalls)
   {
      auto testObject = (*func.first)();
      std::cout << "Object from " << func.second << " has been copied " << testObject.copied << " times\n";
      std::cout << "Object from " << func.second << " has been moved " << testObject.moved << " times\n";
      std::cout << std::endl;
   }

   std::cin.ignore();
}