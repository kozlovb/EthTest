#include "AST.h"
#include <stdexcept>

namespace ARTParser {

double CAbsSynTree::ApplyOperation(const double l, const double r,const Operation op) {
  switch(op) {
    case Operation::EPlus: return l+r;
	case Operation::EMinus: return l-r;
    case Operation::EMultiplication: return l*r;
	case Operation::EDivision:
	  if(r != 0) {
	    return l/r;
	  }
	  else {
	    throw std::overflow_error("Can't divide by zero");
	  }
	default: throw std::runtime_error("Unknown operation");
  }
}

const std::unordered_map<char, double> CAbsSynTree::Element::charToDouble = {{'0',0}, {'1',1},
    {'2',2}, {'3',3}, {'4',4}, {'5',5}, {'6',6}, {'7',7}, {'8',8}, {'9',9}};
const std::unordered_map<char, Operation>  CAbsSynTree::Element::charToOperation =
    {{'+', Operation::EPlus}, {'-', Operation::EMinus},
	    {'*', Operation::EMultiplication},{'/', Operation::EDivision}};
const std::unordered_map<char,Type> CAbsSynTree::Element::charToBracket =
    {{'(',Type::ELeftBracket}, {')',Type::ERightBracket}};


CAbsSynTree::Element CAbsSynTree::Element::createElement(char aValue) {
  Element result;
  const auto it_int = charToDouble.find(aValue);
  if(it_int != charToDouble.end()) {
    result._value._number = it_int->second;
    result._type = Type::ENumber;
    return result;
  }
  const auto it_op = charToOperation.find(aValue);
  if(it_op != charToOperation.end()) {
    result._value._operation = it_op->second;
    result._type = Type::EOperation;
    return result;
  }
  const auto it_br = charToBracket.find(aValue);
  if(it_br != charToBracket.end()) {
    result._type = it_br->second;
    return result;
  }
  else {
    throw std::runtime_error("Unknown character");
  }
  return result;
}

Operation  CAbsSynTree::Element::getOperation() const {
  if (_type != Type::EOperation){
    throw std::runtime_error("Trying to get operation from something else");
  }
  return _value._operation ;
}

bool CAbsSynTree::Element::lessPrecedence(const Element& aToCompare) {
  const auto toCompareOp = aToCompare.getOperation();
  const auto this_operation = getOperation();
  if(toCompareOp == Operation::EDivision && this_operation == Operation::EDivision) {
    return true;
  }
  if(this_operation < toCompareOp /*&& toCompareOp != Operation::EMinus*/) {
    return true;
  }
  return false;

}

double CAbsSynTree::Calculate() const
{
  return Calculate(_root);
}

double CAbsSynTree::Calculate(const std::unique_ptr<TreeElement>& aRoot) const
{
  if(aRoot->_value.Is()==Type::ENumber) {
    return aRoot->_value._value._number;
  }
  else {
    const auto op = aRoot->_value._value._operation;
    const auto l = Calculate(aRoot->_left);
    const auto r = Calculate(aRoot->_right);
    return ApplyOperation(l,r,op);
  }
}
} //namespace
