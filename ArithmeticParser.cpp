#include <iostream>
#include <string>
#include "Calculator.h"
#include "AST.h"
#include <stdexcept>
#include <vector>

using namespace std;

//Possible extensions -
//add gtest framework , unit test each class separately
//add cmake
//think of multithreading - for calculation and probably parcing
//Compilers usually preoptimise, would look better as I assume you are writing a compiler.
//Some checks might be redundant.
//Add a logger.
//Probably there is a generic way to create an AST given some set of rules/policies.
//So may be it's possible to write such an "universal compiler",
//at list for different arithmetic operators
//-1 doesn't work, i.e. something like (-1)*(2+2) will fail, I considered that all numbers
//are represented by one character. Probably you didn't mean that and -1 should work then.
//Can fix this later.

int main() {

// Exemple of usage
  ARTParser::CCalculator calc;

  const std::vector<std::string> example_inputs {
    "    (4+5*(7-3) ) -2 ",
	"(4+5/(7- 7))-2",
	"(3 * 2) a - (1 + 2) / 4)",
	"(3 * 2) - (1 + 2)  / (4)4 ",
	"(-1)*(2+2)",
	"",
	"3--2",
	"((3*2-5/5+2)-(6-5))*4",
	"(3*2-5/5+2)",
	"1-7-2"
  };
  std::string output_string;
  for(const auto& input_string : example_inputs) {
    const auto res = calc.Calculate(input_string, output_string);
    if(res.first) {
      std::cout<< "result is " << res.second << std::endl;
    } else {
	  std::cout<< output_string <<std::endl;
    }
  }
// Tests without framework, TODO use gtest
// Test CHECK
  const std::vector<std::string> test_inputs_check {
	" (( 3 * 2)  - (1 + 2) / 4)",
	"",
	"(3 * 2) a - (1 + 2) / 4)",
	"(3**2)",
    "    (44+5*(7-3) ) -2 ",
	")(4+5/(7- 7))-2",
	"(3( * 2) a - (1 + 2) / 4)",
	"(3 * 2) - (1 + 2)  / ()*(4)4 ",
	"(2*)(2+2)",
	"(-1)(*(2+2)",
	"+",
	"(2+2)+3)+(3",
	"(2+2)(+",
	"(2+2)3",
	"((2+2)",
	"(2+2)+"
  };
  for(size_t i = 0; i < test_inputs_check.size(); ++i) {
	const std::string s = std::to_string(i);
    if(i == static_cast<int>(ARTParser::CCalculator::Check(test_inputs_check[i]))) {
      std::cout<<"Test " << s << " of Check succeed" << std::endl;
    }
    else {
      std::cout<<"Test " << s << " of Check failed" << std::endl;
    }
  }
//TEST CALCULATE
  const std::vector<std::string> test_inputs_calculate{
	" (( 3 * 2)  - (2 + 2) / 4)",
	"4/2/2",
	"3 - 2  * (2 + 2) / 2",
	"(((((5)))))*(1-1)",
	"2+2*3",
	"((3*2-5/5+2)-(6-5))*4",
	"1-7-2"
  };
  const std::vector<int> results {5, 1, -1, 0, 8, 24, -8};
  for(size_t i = 0; i < test_inputs_calculate.size(); ++i) {
    const auto res = calc.Calculate(test_inputs_calculate[i], output_string);
    const std::string s = std::to_string(i);
    if(res.first && res.second == results[i]) {
      std::cout<<"Test " << s << " of Calculate succeed" << std::endl;
    } else {
      std::cout<<"Test " << s << " of Calculate failed" << std::endl;
      std::cout<< res.second<<std::endl;
    }
  }
  return 0;
}
