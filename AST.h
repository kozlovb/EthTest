#ifndef AST_H_
#define AST_H_

#include <memory>
#include <unordered_map>

namespace ARTParser {

enum Type {
  ENumber,
  EOperation,
  ELeftBracket,
  ERightBracket
};

enum Operation {
  EPlus,
  EMinus,
  EMultiplication,
  EDivision
};

template <typename T>
struct Tree {
  Tree(T aEl)
  : _value(aEl)
  {}
  T _value;
  std::unique_ptr<Tree<T> > _left;
  std::unique_ptr<Tree<T> > _right;
  void addRight(Tree<T>&& aEl) {
    _right.reset(new Tree<T>(std::move(aEl)));
  }
  void addLeft(Tree<T>&& aEl) {
    _left.reset(new Tree<T>(std::move(aEl)));
  }
};

class CAbsSynTree {
public:
  struct Element {
    const static std::unordered_map<char, double> charToDouble;
	const static std::unordered_map<char, Operation> charToOperation;
    const static std::unordered_map<char,Type> charToBracket;
    static Element createElement(char aValue);
    Operation getOperation() const;
    bool lessPrecedence(const Element& aToCompare);
	Type Is() const { return _type;};
	using TValueType = union {double _number; Operation _operation;};
	Type _type;
	TValueType _value;
  };

  using TreeElement = Tree<Element>;

  CAbsSynTree(std::unique_ptr<TreeElement> aRoot)
  : _root(std::move(aRoot))
  {}

  double Calculate() const;

private:
  double Calculate(const std::unique_ptr<TreeElement>& aRoot) const;

  static double ApplyOperation(double l, double r, Operation op);

  std::unique_ptr<TreeElement> _root;
};
}

#endif /* AST_H_ */
