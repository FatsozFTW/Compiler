#ifndef CMINUSMINUS_AST_HPP
#define CMINUSMINUS_AST_HPP

#include <ostream>
#include <list>
#include "tokens.hpp"
#include <cassert>


/* You'll probably want to add a bunch of ASTNode subclasses */

namespace drewgon{

/* You may find it useful to forward declare AST subclasses
   here so that you can use a class before it's full definition
*/
class DeclNode;
class TypeNode;
class StmtNode;
class IDNode;
class IntLiteralNode;
class FormalDeclNode;

/** 
* \class ASTNode
* Base class for all other AST Node types
**/
class ASTNode{
public:
	ASTNode(const Position * p) : myPos(p){ }
	virtual void unparse(std::ostream& out, int indent) = 0;
	const Position * pos() { return myPos; }
	std::string posStr() { return pos()->span(); }
protected:
	const Position * myPos = nullptr;
};

/** 
* \class ProgramNode
* Class that contains the entire abstract syntax tree for a program.
* Note the list of declarations encompasses all global declarations
* which includes (obviously) all global variables and struct declarations
* and (perhaps less obviously), all function declarations
**/
class ProgramNode : public ASTNode{
public:
	ProgramNode(std::list<DeclNode *> * globalsIn) ;
	void unparse(std::ostream& out, int indent) override;
private:
	std::list<DeclNode * > * myGlobals;
};

class StmtNode : public ASTNode{
public:
	StmtNode(const Position * p) : ASTNode(p){ }
	void unparse(std::ostream& out, int indent) override = 0;
};


/** \class DeclNode
* Superclass for declarations (i.e. nodes that can be used to 
* declare a struct, function, variable, etc).  This base class will 
**/
class DeclNode : public StmtNode{
public:
	DeclNode(const Position * p) : StmtNode(p) { }
	void unparse(std::ostream& out, int indent) override = 0;
};

/**  \class ExpNode
* Superclass for expression nodes (i.e. nodes that can be used as
* part of an expression).  Nodes that are part of an expression
* should inherit from this abstract superclass.
**/
class ExpNode : public ASTNode{
protected:
	ExpNode(const Position * p) : ASTNode(p){ }
};

/**  \class TypeNode
* Superclass of nodes that indicate a data type. For example, in 
* the declaration "int a", the int part is the type node (a is an IDNode
* and the whole thing is a DeclNode).
**/
class TypeNode : public ASTNode{
protected:
	TypeNode(const Position * p) : ASTNode(p){
	}
public:
	virtual void unparse(std::ostream& out, int indent) = 0;
};

/** An identifier. Note that IDNodes subclass
 * ExpNode because they can be used as part of an expression. 
**/
class IDNode : public ExpNode{
public:
	IDNode(const Position * p, std::string nameIn) 
	: ExpNode(p), name(nameIn){ }
	void unparse(std::ostream& out, int indent);
private:
	/** The name of the identifier **/
	std::string name;
};

 
/** A variable declaration.
**/
class VarDeclNode : public DeclNode{
public:
	VarDeclNode(const Position * p, TypeNode * type, IDNode * id) 
	: DeclNode(p), myType(type), myId(id){
		assert (myType != nullptr);
		assert (myId != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode * myType;
	IDNode * myId;
};

class IntTypeNode : public TypeNode{
public:
	IntTypeNode(const Position * p) : TypeNode(p){ }
	void unparse(std::ostream& out, int indent);
};


class BoolTypeNode : public TypeNode{
public:
	BoolTypeNode(const Position * p) : TypeNode(p){ }
	void unparse(std::ostream& out, int indent);
};

class VoidTypeNode : public TypeNode{
	public:
	VoidTypeNode(const Position * p) : TypeNode(p){ }
	void unparse(std::ostream& out, int indent);
};

class FnDeclNode : public DeclNode{
public:
	FnDeclNode(const Position * p, TypeNode * type, IDNode * id, std::list<FormalDeclNode *> * formalIn, std::list<StmtNode *> * bodyIn ) : DeclNode(p), myType(type), myId(id),myBody(bodyIn), myFormal(formalIn) { 
		assert (myType != nullptr);
		assert (myId != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	TypeNode * myType;
	IDNode * myId;
	std::list<StmtNode * > * myBody;
	std::list<FormalDeclNode * > * myFormal;
};

class FormalDeclNode : public VarDeclNode{
public:
	FormalDeclNode(const Position * p, TypeNode * type, IDNode * id) : VarDeclNode(p, type, id), myType(type), myId(id){
		assert (myType != nullptr);
		assert (myId != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
TypeNode* myType;
IDNode* myId;
};


class FnTypeNode : public TypeNode{
public:
	FnTypeNode(const Position * p, std::list<TypeNode *> * typeListIn, TypeNode * typeIn) : TypeNode(p), myTypeList(typeListIn), myType(typeIn)
	{
		assert (myType != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	std::list<TypeNode *> * myTypeList;
	TypeNode* myType;
};

class AssignExpNode : public ExpNode{
public:
	AssignExpNode(const Position * p, IDNode * id, ExpNode * exp) : ExpNode(p), myId(id), myExp(exp)
	{
		assert (myId != nullptr);
		assert (myExp != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myId;
	ExpNode * myExp;
};

class AssignStmtNode : public StmtNode{
public:
	AssignStmtNode(const Position * p, AssignExpNode * assign) : StmtNode(p), myAssign(assign)
	{
		assert (myAssign != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	AssignExpNode * myAssign;
};

class IntLiteralNode : public ExpNode{
public:
	IntLiteralNode(const Position * p, int intIn) : ExpNode(p), myInt(intIn) { }
	void unparse(std::ostream& out, int indent);
private:
int myInt;
};

class StringLiteralNode : public ExpNode{
public:
	StringLiteralNode(const Position * p, std::string strIn) : ExpNode(p), myStr(strIn) { }
	void unparse(std::ostream& out, int indent);
private:
std::string myStr;
};

class TrueNode : public ExpNode{
public:
	TrueNode(const Position * p) : ExpNode(p) { }
	void unparse(std::ostream& out, int indent);

};

class FalseNode : public ExpNode{
public:
	FalseNode(const Position * p) : ExpNode(p) { }
	void unparse(std::ostream& out, int indent);

};

class MayhemNode : public ExpNode{
public:
	MayhemNode(const Position * p) : ExpNode(p) { }
	void unparse(std::ostream& out, int indent);

};

class CallExpNode : public ExpNode{
public:
	CallExpNode(const Position * p, IDNode * id, std::list<ExpNode * > * callList) : ExpNode(p), myId(id), myCallList(callList) { 
		assert (myId != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myId;
	std::list<ExpNode *> * myCallList;
};

class CallStmtNode : public StmtNode{
public:
	CallStmtNode(const Position * p, CallExpNode * call) : StmtNode(p), myCall(call)
	{
		assert (myCall != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	CallExpNode * myCall;
};

class BinaryExpNode : public ExpNode{
public:
	BinaryExpNode(const Position * p, ExpNode * left, ExpNode * right) : ExpNode(p), myLeft(left), myRight(right) {
		assert (myLeft != nullptr);
		assert (myRight != nullptr);
	}
protected:
	ExpNode* myLeft;
	ExpNode* myRight;
};

class MinusNode : public BinaryExpNode{
public:
	MinusNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class PlusNode : public BinaryExpNode{
public:
	PlusNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class TimesNode : public BinaryExpNode{
public:
	TimesNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class DivideNode : public BinaryExpNode{
public:
	DivideNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class AndNode : public BinaryExpNode{
public:
	AndNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class OrNode : public BinaryExpNode{
public:
	OrNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class EqualsNode : public BinaryExpNode{
public:
	EqualsNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class NotEqualsNode : public BinaryExpNode{
public:
	NotEqualsNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class GreaterNode : public BinaryExpNode{
public:
	GreaterNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class GreaterEqNode : public BinaryExpNode{
public:
	GreaterEqNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class LessNode : public BinaryExpNode{
public:
	LessNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class LessEqNode : public BinaryExpNode{
public:
	LessEqNode(const Position * p, ExpNode * left, ExpNode * right) : BinaryExpNode(p, left, right){ }
	void unparse(std::ostream& out, int indent);
};

class UnaryExpNode : public ExpNode{
public:
	UnaryExpNode(const Position * p, ExpNode * exp) : ExpNode(p), myExp(exp) {
		assert (myExp != nullptr);
	}
protected:
	ExpNode* myExp;

};

class NotNode : public UnaryExpNode{
public:
	NotNode(const Position * p, ExpNode * exp) : UnaryExpNode(p, exp){ }
	void unparse(std::ostream& out, int indent);
};
class NegNode : public UnaryExpNode{
public:
	NegNode(const Position * p, ExpNode * exp) : UnaryExpNode(p, exp){ }
	void unparse(std::ostream& out, int indent);
};

class PostIncStmtNode : public StmtNode{
public:
	PostIncStmtNode(const Position * p, IDNode * id) : StmtNode(p), myId(id) {
		assert(myId != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myId;
};

class PostDecStmtNode : public StmtNode{
public:
	PostDecStmtNode(const Position * p, IDNode * id) : StmtNode(p), myId(id) {
		assert(myId != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myId;
};

class InputStmtNode : public StmtNode{
public:
	InputStmtNode(const Position * p, IDNode * id) : StmtNode(p), myId(id) {
		assert(myId != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	IDNode * myId;
};

class OutputStmtNode : public StmtNode{
public:
	OutputStmtNode(const Position * p, ExpNode * exp) : StmtNode(p), myExp(exp) {
		assert(myExp != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
};

class ReturnStmtNode : public StmtNode{
public:
	ReturnStmtNode(const Position * p, ExpNode * exp) : StmtNode(p), myExp(exp) {
		//assert(myExp != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
};

class IfStmtNode : public StmtNode{
public:
	IfStmtNode(const Position * p, ExpNode * exp, std::list<StmtNode *> * bodyIn) : StmtNode(p), myExp(exp), myBody(bodyIn) {
		//assert(myExp != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
	std::list<StmtNode *> * myBody;
};

class IfElseStmtNode : public StmtNode{
public:
	IfElseStmtNode(const Position * p, ExpNode * exp, std::list<StmtNode *> * trueIn, std::list<StmtNode *> * falseIn) : StmtNode(p), myExp(exp), myTrue(trueIn), myFalse(falseIn) {
		//assert(myExp != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
	std::list<StmtNode *> * myTrue;
	std::list<StmtNode *> * myFalse;
};

class WhileStmtNode : public StmtNode{
public:
	WhileStmtNode(const Position * p, ExpNode * exp, std::list<StmtNode *> * bodyIn) : StmtNode(p), myExp(exp), myBody(bodyIn) {
		//assert(myExp != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
	std::list<StmtNode *> * myBody;
};

class ForStmtNode : public StmtNode{
public:
	ForStmtNode(const Position * p, ExpNode * exp, StmtNode * initIn, StmtNode * updateIn, std::list<StmtNode *> * bodyIn) : StmtNode(p), myExp(exp), myInit(initIn), myUpdate(updateIn), myBody(bodyIn) {
		//assert(myExp != nullptr);
	}
	void unparse(std::ostream& out, int indent);
private:
	ExpNode * myExp;
	StmtNode * myInit;
	StmtNode * myUpdate;
	std::list<StmtNode *> * myBody;
};

} //End namespace drewgon


#endif
