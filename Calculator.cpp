#include "Calculator.h"
#include "AST.h"
#include <queue>
#include <stack>
#include <stdexcept>

namespace ARTParser {
namespace {

void AddToOutput(CAbsSynTree::TreeElement aEl, std::stack<CAbsSynTree::TreeElement>& aOutput) {
  if(aEl._value._type != Type::EOperation) {
    throw std::runtime_error("AddToOutput should be used for an operation");
  }
  aEl.addRight(std::move(aOutput.top()));
  aOutput.pop();
  aEl.addLeft(std::move(aOutput.top()));
  aOutput.pop();
  aOutput.push(std::move(aEl));
}

size_t FindNextNotSpace(const size_t aPosition,const std::string& aInput) {
  for(auto i = aPosition; i < aInput.size();++i) {
    if(aInput[i] != ' ') {
	  return i;
	}
  }
  return aInput.size();
}

bool checkCharacter(const char c) {
  if(CAbsSynTree::Element::charToDouble.find(c) ==
		  CAbsSynTree::Element::charToDouble.end()
     && CAbsSynTree::Element::charToOperation.find(c) ==
		  CAbsSynTree::Element::charToOperation.end()
     && CAbsSynTree::Element::charToBracket.find(c) ==
		  CAbsSynTree::Element::charToBracket.end()) {
    return false;
  }
  else {
    return true;
  }
}
}

const std::unordered_map<ResultCode, std::string, EnumClassHash> CCalculator::ErrorMessages = {
 {ResultCode::EOk, "Correct expression"},
 {ResultCode::EEmpty, "Input is empty"},
 {ResultCode::EUnknownCharacter, "Unknown character in input"},
 {ResultCode::ETwoOrMoreOperationsInARow, "Two or more operators in a row"},
 {ResultCode::ETwoOrMoreNumbersInARow, "Two or more numbers in a row"},
 {ResultCode::EshouldStarWithNumberOrLeftBracket,
		 "Expression should start with a number or left bracket"},
 {ResultCode::EBracketAfterNumber, "A bracket is after number"},
 {ResultCode::ERightBracketAfterLeft, "Right bracket is after a left one"},
 {ResultCode::ERightBracketAfterOp, "Right bracket is after an operation"},
 {ResultCode::EOperationAfterLeftBracket, "An operation is after a left bracket"},
 {ResultCode::EOfOneMustBeNumber, "Expression with only one character must be a number"},
 {ResultCode::ETooMuchRightBracketBeforeLeft, "An unclosed right bracket found"},
 {ResultCode::ELeftBracketAfterRight, "Nothing between two brackets"},
 {ResultCode::ENumberAfterRightBracket, "A number after right bracket is found"},
 {ResultCode::ENumberOfLeftAndRightBracketArenotEq, "Quantities of left and right brackets are not equal"},
 {ResultCode::LastMustBeNumberOrRightBr, "A last character(space excluded) must be a number or right bracket"}
};

ResultCode CCalculator::Check(const std::string& aInput) {
  if(aInput.empty()) {
    return ResultCode::EEmpty;
  }
  if(aInput.size() == 1) {
    if(CAbsSynTree::Element::createElement(aInput[0])._type != Type::ENumber)
      return ResultCode::EOfOneMustBeNumber;
  }

  CAbsSynTree::Element next;
  CAbsSynTree::Element cur;
  unsigned left_br_counter = 0;
  unsigned right_br_counter = 0;
  const auto start = FindNextNotSpace(0, aInput);
  size_t i = start;
  while(i < aInput.size()) {
    //check the character validity
    if(!checkCharacter(aInput[i])) {
      return ResultCode::EUnknownCharacter;
    }
    cur = CAbsSynTree::Element::createElement(aInput[i]);
    if(i == start) {
      if(cur.Is() == Type::EOperation || cur.Is() == Type::ERightBracket) {
        return ResultCode::EshouldStarWithNumberOrLeftBracket;
      }
    }
    auto i_next = FindNextNotSpace(i+1, aInput);
    if(i_next != aInput.size()) {
      if(!checkCharacter(aInput[i_next])) {
        return ResultCode::EUnknownCharacter;
      }
      next = CAbsSynTree::Element::createElement(aInput[i_next]);
      //check a first number
      //Number might be followed by operation or ).
      if(cur.Is() == Type::ENumber) {
        if(next.Is() == Type::ENumber) {
          return ResultCode::ETwoOrMoreNumbersInARow;
        }
        if(next.Is() == Type::ELeftBracket) {
          return ResultCode::EBracketAfterNumber;
        }
      }
      //Operation Might be followed by ( or number.
      if(cur.Is() == Type::EOperation) {
        if(next.Is() == Type::EOperation) {
          return ResultCode::ETwoOrMoreOperationsInARow;
        }
        if(next.Is() == Type::ERightBracket) {
          return ResultCode::ERightBracketAfterOp;
        }
      }
      //( might be followed by number or (
      if(cur.Is() == Type::ELeftBracket) {
        ++left_br_counter;
        if(next.Is() == Type::ERightBracket) {
          return ResultCode::ERightBracketAfterLeft;
        }
        if(next.Is() == Type::EOperation) {
    	  return ResultCode::EOperationAfterLeftBracket;
        }
      }
      //) might be followed by operation or )
      //number of ) is always <= number of (
      if(cur.Is() == Type::ERightBracket) {
        ++right_br_counter;
        if(right_br_counter > left_br_counter) {
          return ResultCode::ETooMuchRightBracketBeforeLeft;
        }
        if(next.Is() == Type::ELeftBracket) {
          return ResultCode::ELeftBracketAfterRight;
        }
        if(next.Is() == Type::ENumber) {
    	  return ResultCode::ENumberAfterRightBracket;
        }
      }
    }
    //check last element
    else {
      if(cur.Is() == Type::ELeftBracket || next.Is() == Type::EOperation) {
        return ResultCode::LastMustBeNumberOrRightBr;
      }
      if(cur.Is() == Type::ERightBracket) {
        ++right_br_counter;
      }
    }
    i = i_next;
  }
  //check number of ( equals number of )
  if(left_br_counter != right_br_counter) {
    return ResultCode::ENumberOfLeftAndRightBracketArenotEq;
  }
  return ResultCode::EOk;
}

void CCalculator::Parse(const std::string& aInput) {

  std::stack<CAbsSynTree::TreeElement> output;
  std::stack<CAbsSynTree::Element> operatorsBrackets;

  for(auto e : aInput) {
    if(e != ' ') {
      CAbsSynTree::Element cur = CAbsSynTree::Element::createElement(e);
      switch (cur.Is()) {
        case Type::ENumber: output.push(CAbsSynTree::TreeElement(cur));
	    break;
      case Type::EOperation:
    	//"1-5*5+1"
        while(!operatorsBrackets.empty() && operatorsBrackets.top().Is() != Type::ELeftBracket
    	   	  && cur.lessPrecedence(operatorsBrackets.top())) {
          AddToOutput(CAbsSynTree::TreeElement(operatorsBrackets.top()), output);
          operatorsBrackets.pop();
        }
        operatorsBrackets.push(cur);
        break;
      case Type::ELeftBracket:
    	operatorsBrackets.push(cur);
        break;
      case Type::ERightBracket:
        while(operatorsBrackets.top().Is() != Type::ELeftBracket) {
          AddToOutput(CAbsSynTree::TreeElement(operatorsBrackets.top()), output);
          operatorsBrackets.pop();
        }
        operatorsBrackets.pop();
        break;
      default:
	    throw std::runtime_error("Unkown type of element");
      }
    }
  }

  while(!operatorsBrackets.empty()) {
    AddToOutput(CAbsSynTree::TreeElement(operatorsBrackets.top()), output);
    operatorsBrackets.pop();
  }

  iAbsSynTree = std::unique_ptr<CAbsSynTree>( new CAbsSynTree(std::unique_ptr<CAbsSynTree::TreeElement>(new
             CAbsSynTree::TreeElement(std::move(output.top())))));
}
std::pair<bool,double> CCalculator::Calculate(const std::string& aInput,std::string& aOutputMessage) {
  ResultCode resCode = Check(aInput);
  const std::pair<bool, double> invalidResult = {false, 0};
  const auto it = ErrorMessages.find(resCode);
  aOutputMessage = it->second;
  if(resCode != ResultCode::EOk) {
    return invalidResult;
  }
  Parse(aInput);
  try {
    const auto result = iAbsSynTree->Calculate();
	return {true, result};
  } catch(const std::overflow_error& e) {
	  aOutputMessage = "Can't divide by zero";
    return invalidResult;
  } catch(const std::runtime_error& e) {
  //TODO log error message by some logger
    aOutputMessage= "Internal error occurred";
    return invalidResult;
  } catch (...) {
	//TODO log error
    //std::cout<< "Unknown Error";
	aOutputMessage= "Internal error occurred";
	return invalidResult;
  }
}
} //namespaces
