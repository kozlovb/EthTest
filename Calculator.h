/*
 * Calculator.h
 *
 *  Created on: Jan 24, 2018
 *      Author: bk
 */
#ifndef CALCULATOR_H_
#define CALCULATOR_H_
#include "AST.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace ARTParser {

enum ResultCode {
  EOk,
  EEmpty,
  EUnknownCharacter,
  ETwoOrMoreOperationsInARow,
  ETwoOrMoreNumbersInARow,
  EshouldStarWithNumberOrLeftBracket,
  EBracketAfterNumber,
  ERightBracketAfterLeft,
  ERightBracketAfterOp,
  EOperationAfterLeftBracket,
  EOfOneMustBeNumber,
  ETooMuchRightBracketBeforeLeft,
  ELeftBracketAfterRight,
  ENumberAfterRightBracket,
  ENumberOfLeftAndRightBracketArenotEq,
  LastMustBeNumberOrRightBr
};

namespace {
struct EnumClassHash
{
    template <typename T>
    std::size_t operator()(T t) const
    {
        return static_cast<std::size_t>(t);
    }
};
}

class CCalculator {
public:
  static ResultCode Check(const std::string& aInput);
  std::pair<bool,double>
      Calculate(const std::string& aInput, std::string& aOutputMessage);
private:
  static const std::unordered_map<ResultCode, std::string, EnumClassHash> ErrorMessages;
  void Parse(const std::string& aInput);
  std::unique_ptr<CAbsSynTree> iAbsSynTree;
};
}
#endif /* CALCULATOR_H_ */
