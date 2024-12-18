#ifndef __AST_H__
#define __AST_H__

#include "Operand.h"
#include "Type.h"
#include <fstream>
#include <iostream>
#include <stack>

class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;

class Node {
private:
  static int counter;
  int seq;
  Node *next;

protected:
  std::vector<Instruction *> true_list;
  std::vector<Instruction *> false_list;
  static IRBuilder *builder;
  void backPatch(std::vector<Instruction *> &list, BasicBlock *bb);
  std::vector<Instruction *> merge(std::vector<Instruction *> &list1,
                                   std::vector<Instruction *> &list2);

public:
  Node();
  int getSeq() const { return seq; };
  static void setIRBuilder(IRBuilder *ib) { builder = ib; };
  void setNext(Node *node);
  Node *getNext() { return next; }
  virtual void genCode() = 0;
  std::vector<Instruction *> &trueList() { return true_list; }
  std::vector<Instruction *> &falseList() { return false_list; }
  virtual void output(int level) = 0;
};

class ExprNode : public Node {
private:
  int kind;

protected:
  enum { EXPR, INITVALUELISTEXPR, IMPLICTCASTEXPR, UNARYEXPR };
  Type *type;
  SymbolEntry *symbolEntry;
  Operand *dst; // The result of the subtree is stored into dst.
public:
  ExprNode(SymbolEntry *symbolEntry, int kind = EXPR)
      : kind(kind), symbolEntry(symbolEntry) {};
  Operand *getOperand() { return dst; };
  virtual int getValue() { return -1; };
  bool isExpr() const { return kind == EXPR; };
  bool isInitValueListExpr() const { return kind == INITVALUELISTEXPR; };
  bool isImplictCastExpr() const { return kind == IMPLICTCASTEXPR; };
  bool isUnaryExpr() const { return kind == UNARYEXPR; };
  SymbolEntry *getSymbolEntry() { return symbolEntry; };
  void genCode();
  virtual Type *getType() { return type; };
  Type *getOriginType() { return type; };
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "ExprNode:" << "kind=" << kind << std::endl;
  }
};

class BinaryExpr : public ExprNode {
private:
  int op;
  ExprNode *expr1, *expr2;

public:
  enum {
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    AND,
    OR,
    LESS,
    LESSEQUAL,
    GREATER,
    GREATEREQUAL,
    EQUAL,
    NOTEQUAL
  };
  BinaryExpr(SymbolEntry *se, int op, ExprNode *expr1, ExprNode *expr2);
  int getValue();
  void genCode();
  void output(int level) {
    std::string op_str;
    switch (op) {
    case ADD:
      op_str = "add";
      break;
    case SUB:
      op_str = "sub";
      break;
    case AND:
      op_str = "and";
      break;
    case OR:
      op_str = "or";
      break;
    case LESS:
      op_str = "less";
      break;
    case GREATER:
      op_str = "greater";
      break;
    case GREATEREQUAL:
      op_str = "greaterequal";
      break;
    case LESSEQUAL:
      op_str = "lessequal";
      break;
    case EQUAL:
      op_str = "equal";
      break;
    case NOTEQUAL:
      op_str = "noequal";
      break;
    case DIV:
      op_str = "div";
      break;
    case MUL:
      op_str = "mul";
      break;
    }

    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "BinaryExpr: op=" << op_str << std::endl;
    if (expr1)
      expr1->output(level + 1);
    if (expr2)
      expr2->output(level + 1);
  }
};

class UnaryExpr : public ExprNode {
private:
  int op;
  ExprNode *expr;

public:
  enum { NOT, SUB, DECREMENT, INCREMENT };
  UnaryExpr(SymbolEntry *se, int op, ExprNode *expr);
  int getValue();
  void genCode();
  int getOp() const { return op; };
  void setType(Type *type) { this->type = type; }
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::string op_str;
    switch (op) {
    case NOT:
      op_str = "not";
      break;
    case SUB:
      op_str = "sub";
      break;
    case INCREMENT:
      op_str = "increment";
      break;
    case DECREMENT:
      op_str = "decrement";
      break;
    }
    std::cout << "UnaryExpr: op=" << op_str << std::endl;
    if (expr)
      expr->output(level + 1);
  }
};

class CallExpr : public ExprNode {
private:
  ExprNode *param;

public:
  CallExpr(SymbolEntry *se, ExprNode *param = nullptr);
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "CallExpr" << std::endl;
    if (param)
      param->output(level + 1);
    Node* tmp = param;
    while(tmp->getNext()){
        tmp = tmp->getNext();
        tmp->output(level + 1);
    }
  }
};

class Constant : public ExprNode {
public:
  Constant(SymbolEntry *se) : ExprNode(se) {
    dst = new Operand(se);
    type = TypeSystem::intType;
  };
  int getValue();
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "Constant: value=" << getValue() << std::endl;
  }
};

class Id : public ExprNode {
private:
  ExprNode *arrIdx;
  bool left = false;

public:
  Id(SymbolEntry *se, ExprNode *arrIdx = nullptr)
      : ExprNode(se), arrIdx(arrIdx) {
    if (se) {
      type = se->getType();
      if (type->isInt()) {
        SymbolEntry *temp = new TemporarySymbolEntry(TypeSystem::intType,
                                                     SymbolTable::getLabel());
        dst = new Operand(temp);
      } else if (type->isArray()) {
        SymbolEntry *temp = new TemporarySymbolEntry(
            new PointerType(((ArrayType *)type)->getElementType()),
            SymbolTable::getLabel());
        dst = new Operand(temp);
      }
    }
  };
  void genCode();
  int getValue();
  ExprNode *getArrIdx() { return arrIdx; };
  Type *getType();
  bool isLeft() const { return left; };
  void setLeft() { left = true; }
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "Id: name=" << symbolEntry->toStr() << std::endl;
    if (arrIdx)
      arrIdx->output(level + 1);
  }
};

class ImplicitValueInitExpr : public ExprNode {
public:
  ImplicitValueInitExpr(SymbolEntry *se) : ExprNode(se) {};
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "ImplicitValueInitExpr" << std::endl;
  }
};

class InitValueListExpr : public ExprNode {
private:
  ExprNode *expr;
  int childCnt;

public:
  InitValueListExpr(SymbolEntry *se, ExprNode *expr = nullptr)
      : ExprNode(se, INITVALUELISTEXPR), expr(expr) {
    type = se->getType();
    childCnt = 0;
  };
  ExprNode *getExpr() const { return expr; };
  void addExpr(ExprNode *expr);
  bool isEmpty() { return childCnt == 0; };
  bool isFull();
  void genCode();
  void fill();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "InitValueListExpr" << std::endl;
    if (expr)
      expr->output(level + 1);
  }
};

class ImplictCastExpr : public ExprNode {
private:
  ExprNode *expr;

public:
  ImplictCastExpr(ExprNode *expr)
      : ExprNode(nullptr, IMPLICTCASTEXPR), expr(expr) {
    type = TypeSystem::boolType;
    dst = new Operand(new TemporarySymbolEntry(type, SymbolTable::getLabel()));
  };
  ExprNode *getExpr() const { return expr; };
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "ImplictCastExpr" << std::endl;
    if (expr)
      expr->output(level + 1);
  }
};

class StmtNode : public Node {
private:
  int kind;

protected:
  enum { IF, IFELSE, WHILE, FOR, COMPOUND, RETURN };

public:
  StmtNode(int kind = -1) : kind(kind) {};
  bool isIf() const { return kind == IF; };
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "StmtNode: kind=" << kind << std::endl;
  }
};

class CompoundStmt : public StmtNode {
private:
  StmtNode *stmt;

public:
  CompoundStmt(StmtNode *stmt = nullptr) : stmt(stmt) {};
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "CompoundStmt" << std::endl;
    if (stmt)
      stmt->output(level + 1);
  }
};

class SeqNode : public StmtNode {
private:
  StmtNode *stmt1, *stmt2;

public:
  SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2) {};
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "SeqNode" << std::endl;
    if (stmt1)
      stmt1->output(level + 1);
    if (stmt2)
      stmt2->output(level + 1);
  }
};

class DeclStmt : public StmtNode {
private:
  Id *id;
  ExprNode *expr;

public:
  DeclStmt(Id *id, ExprNode *expr = nullptr) : id(id) {
    if (expr) {
      this->expr = expr;
      if (expr->isInitValueListExpr())
        ((InitValueListExpr *)(this->expr))->fill();
    }
  };
  void genCode();
  Id *getId() { return id; };
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "DeclStmt" << std::endl;
    id->output(level + 1);
    if (expr)
      expr->output(level + 1);
    if(getNext())
        getNext()->output(level + 1);
  }
};

class BlankStmt : public StmtNode {
public:
  BlankStmt() {};
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "BlankStmt" << std::endl;
  }
};

class IfStmt : public StmtNode {
private:
  ExprNode *cond;
  StmtNode *thenStmt;

public:
  IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt) {
    if (cond->getType()->isInt() && cond->getType()->getSize() == 32) {
      ImplictCastExpr *temp = new ImplictCastExpr(cond);
      this->cond = temp;
    }
  };
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "IfStmt" << std::endl;
    cond->output(level + 1);
    thenStmt->output(level + 1);
  }
};

class IfElseStmt : public StmtNode {
private:
  ExprNode *cond;
  StmtNode *thenStmt;
  StmtNode *elseStmt;

public:
  IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt)
      : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt) {
    if (cond->getType()->isInt() && cond->getType()->getSize() == 32) {
      ImplictCastExpr *temp = new ImplictCastExpr(cond);
      this->cond = temp;
    }
  };
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "IfElseStmt" << std::endl;
    cond->output(level + 1);
    thenStmt->output(level + 1);
    elseStmt->output(level + 1);
  }
};

class WhileStmt : public StmtNode {
private:
  ExprNode *cond;
  StmtNode *stmt;
  BasicBlock *cond_bb;
  BasicBlock *end_bb;

public:
  WhileStmt(ExprNode *cond, StmtNode *stmt = nullptr) : cond(cond), stmt(stmt) {
    if (cond->getType()->isInt() && cond->getType()->getSize() == 32) {
      ImplictCastExpr *temp = new ImplictCastExpr(cond);
      this->cond = temp;
    }
  };
  void setStmt(StmtNode *stmt) { this->stmt = stmt; };
  void genCode();
  BasicBlock *get_cond_bb() { return this->cond_bb; };
  BasicBlock *get_end_bb() { return this->end_bb; };
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "WhileStmt" << std::endl;
    cond->output(level + 1);
    stmt->output(level + 1);
  }
};

class ForStmt : public StmtNode {
private:
    StmtNode *init;
    ExprNode *cond;
    ExprNode *loop;
    StmtNode *stmt;

public:
    ForStmt(StmtNode *init, ExprNode *cond, ExprNode *loop, StmtNode *stmt)
        : init(init), cond(cond), loop(loop), stmt(stmt) {}
    void genCode();
    StmtNode *getInit() const { return init; }
    ExprNode *getCond() const { return cond; }
    ExprNode *getLoop() const { return loop; }
    StmtNode *getStmt() const { return stmt; }
    void output(int level) {
        for (int i = 0; i < level; i++)
            std::cout << "  ";
        std::cout << "ForStmt" << std::endl;
        if (init)
            init->output(level + 1);
        if (cond)
            cond->output(level + 1);
        if (loop)
            loop->output(level + 1);
        if (stmt)
            stmt->output(level + 1);
    }
};
class BreakStmt : public StmtNode {
private:
  StmtNode *whileStmt;

public:
  BreakStmt(StmtNode *whileStmt) { this->whileStmt = whileStmt; };
  void genCode();
};

class ContinueStmt : public StmtNode {
private:
  StmtNode *whileStmt;

public:
  ContinueStmt(StmtNode *whileStmt) { this->whileStmt = whileStmt; };
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "BreakStmt" << std::endl;
  }
};

class ReturnStmt : public StmtNode {
private:
  ExprNode *retValue;

public:
  ReturnStmt(ExprNode *retValue = nullptr) : retValue(retValue) {};
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "ReturnStmt" << std::endl;
    if (retValue)
      retValue->output(level + 1);
  }
};

class AssignStmt : public StmtNode {
private:
  ExprNode *lval;
  ExprNode *expr;

public:
  AssignStmt(ExprNode *lval, ExprNode *expr);
  bool typeCheck(Type *retType = nullptr);
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "AssignStmt" << std::endl;
    if (lval)
      lval->output(level + 1);
    if (expr)
      expr->output(level + 1);
  }
};

class ExprStmt : public StmtNode {
private:
  ExprNode *expr;

public:
  ExprStmt(ExprNode *expr) : expr(expr) {};
  void genCode();
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "ExprStmt" << std::endl;
    if (expr)
      expr->output(level + 1);
  }
};

class FunctionDef : public StmtNode {
private:
  SymbolEntry *se;
  DeclStmt *decl;
  StmtNode *stmt;

public:
  FunctionDef(SymbolEntry *se, DeclStmt *decl, StmtNode *stmt)
      : se(se), decl(decl), stmt(stmt) {};
  void genCode();
  SymbolEntry *getSymbolEntry() { return se; };
  void output(int level) {
    for (int i = 0; i < level; i++)
      std::cout << "  ";
    std::cout << "FunctionDef: name=" << se->toStr() << std::endl;
    if (decl)
      decl->output(level + 1);
    if (stmt)
      stmt->output(level + 1);
  }
};

class Ast {
private:
  Node *root;

public:
  Ast() { root = nullptr; }
  void setRoot(Node *n) { root = n; }
  void genCode(Unit *unit);
  void output() {
    std::cout << "Program\n";
    if (root)
      root->output(1);
  }
};
#endif
