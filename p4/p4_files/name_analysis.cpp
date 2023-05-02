#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "errName.hpp"
#include <iostream>

namespace drewgon{

//TODO here is a subset of the nodes needed to do nameAnalysis, 
// you should add the rest to allow for a complete treatment
// of any AST

bool ASTNode::nameAnalysis(SymbolTable * symTab){
	throw new ToDoError("This function should have"
		"been overriden in the subclass!");
}

bool ProgramNode::nameAnalysis(SymbolTable * symTab){
	bool res = true;
	symTab->enterScope();
	for (auto global : *myGlobals){
		res = global->nameAnalysis(symTab) && res;
	}
	symTab->leaveScope();
	return res;
}

bool VarDeclNode::nameAnalysis(SymbolTable * symTab){

	std::string type = getTypeNode()->getType();
	std::string name = ID()->getName();
	
	if(!symTab->isVar(type))
	{
		NameErr::badVarType(pos());
	}
	if(symTab->isPresent(name))
	{
		NameErr::multiDecl(ID()->pos());
	}
	if(symTab->isVar(type) && !symTab->isPresent(name))
	{
		
		SemSymbol* temp = new SemSymbol(name, type, "var");
		myID->attachSymbol(temp);
		symTab->insert(temp);
		
		
		return(true);
	}
	else {
		
		return( false);
	}
	
}

bool FnDeclNode::nameAnalysis(SymbolTable * symTab){
	std::string fnName = ID()->getName();
	bool validRet = myRetType->nameAnalysis(symTab);
	//symTab->addScope();
	ScopeTable* atFnScope = symTab->getCurrentScope();
	ScopeTable* inFnScope = symTab->enterScope();

	bool validName = true;
	if(atFnScope->isPresent(fnName)) {
		NameErr::multiDecl(ID()->pos());
		validName = false;
	}
	//Look in myFormals and myType to create a symbol table entry for this FnDecl
	bool validFormal = true;
	std::list< std::string> * formalTypes = new std::list< std::string> ();
	for(auto formal : *(this->myFormals)) {
		validFormal = formal->nameAnalysis(symTab) && validFormal;
		TypeNode* typeNode = formal->getTypeNode();
		std::string formalType = typeNode->getType();
		formalTypes->push_back(formalType);
	}
	
	std::string retType = this->getRetTypeNode()->getType();

	
	
	if(validName){

		SemSymbol* temp = new SemSymbol(fnName, retType, "fn");
		temp->setList(formalTypes);
		myID->attachSymbol(temp);
		atFnScope->insert(temp);
	}

	//Create a new scope for the funcion's body
	
	//Add myFormals to the function's body scope

	//Descend into the body to continue name Analysis
	bool validBody = true;
	for(auto stmt: *myBody){
		validBody = stmt->nameAnalysis(symTab) && validBody;
	}
	symTab->leaveScope();
	return(validRet && validFormal && validName && validBody);
	
}

bool AssignStmtNode::nameAnalysis(SymbolTable * symTab){
	return myExp->nameAnalysis(symTab);
}

bool AssignExpNode::nameAnalysis(SymbolTable * symTab){
	bool temp = myDst->nameAnalysis(symTab);
	myDst->attachSymbol(symTab->find(myDst->getName()));
	bool temp2 = mySrc->nameAnalysis(symTab);
	return(temp && temp2);
}

bool IntTypeNode::nameAnalysis(SymbolTable* symTab){
	// Name analysis may never even recurse down to IntTypeNode,
	// but if it does, just return true to indicate that 
	// name analysis has not failed, and add nothing to the symbol table
	return true;
}

bool TrueNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool FalseNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool TypeNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool MayhemNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool StrLitNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool IntLitNode::nameAnalysis(SymbolTable* symTab){
	return true;
}

bool NegNode::nameAnalysis(SymbolTable* symTab){
	return myExp->nameAnalysis(symTab);
}

bool NotNode::nameAnalysis(SymbolTable* symTab){
	return myExp->nameAnalysis(symTab);
}

///
bool BinaryExpNode::nameAnalysis(SymbolTable * symTab){
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return temp1 && temp2;
}

bool PlusNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool MinusNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool TimesNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool DivideNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool AndNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool OrNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool EqualsNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool NotEqualsNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool LessNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool LessEqNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool GreaterNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool GreaterEqNode::nameAnalysis(SymbolTable *symTab)
{
	bool temp1 = myExp1->nameAnalysis(symTab);
	bool temp2 = myExp2->nameAnalysis(symTab);
	return (temp1 && temp2);
}

bool BoolTypeNode::nameAnalysis(SymbolTable * symTab)
{
	return true;
}

bool VoidTypeNode::nameAnalysis(SymbolTable * symTab) {

	return true;

}

bool FnTypeNode::nameAnalysis(SymbolTable * symTab) {

	return true;

}

///
bool InputStmtNode::nameAnalysis(SymbolTable *symTab) {
	return myDst->nameAnalysis(symTab);

}

bool OutputStmtNode::nameAnalysis(SymbolTable *symTab) {
	return mySrc->nameAnalysis(symTab);
}

bool CallStmtNode::nameAnalysis(SymbolTable * symTab) { 

	bool temp = myCallExp->nameAnalysis(symTab);
	return temp;
}

bool PostDecStmtNode::nameAnalysis(SymbolTable * symTab){
	return myID->nameAnalysis(symTab);
}

bool PostIncStmtNode::nameAnalysis(SymbolTable * symTab){
	return myID->nameAnalysis(symTab);
}

bool ReturnStmtNode::nameAnalysis(SymbolTable * symTab) {
	if(myExp != nullptr)
	{
		return myExp->nameAnalysis(symTab);
	}
	return true;
}

bool IfStmtNode::nameAnalysis(SymbolTable * symTab) {
	bool weGood = true;
	weGood = myCond->nameAnalysis(symTab) && weGood;
	symTab->enterScope();
	for (auto stmt : *myBody){
		weGood = stmt->nameAnalysis(symTab) && weGood;
	}	
	symTab->leaveScope();
	return weGood;
}

bool IfElseStmtNode::nameAnalysis(SymbolTable * symTab){
	bool weGood = true;
	weGood = myCond->nameAnalysis(symTab) && weGood;
	symTab->enterScope();
	for (auto stmt : *myBodyTrue){
		weGood = stmt->nameAnalysis(symTab) && weGood;
	}	
	symTab->leaveScope();
	
	symTab->enterScope();
	for (auto stmt : *myBodyFalse){
		weGood = stmt->nameAnalysis(symTab) && weGood;
	}	
	symTab->leaveScope();
	return weGood;
}

bool WhileStmtNode::nameAnalysis(SymbolTable * symTab){
	bool weGood = true;
	weGood = myCond->nameAnalysis(symTab) && weGood;
	symTab->enterScope();
	for (auto stmt : *myBody){
		weGood = stmt->nameAnalysis(symTab) && weGood;
	}	
	symTab->leaveScope();
	
	return weGood;
}

bool ForStmtNode::nameAnalysis(SymbolTable * symTab){
	bool weGood = true;
	weGood = myInit->nameAnalysis(symTab) && weGood;
	weGood = myCond->nameAnalysis(symTab) && weGood;
	weGood = myItr->nameAnalysis(symTab) && weGood;
	symTab->enterScope();
	for (auto stmt : *myBody){
		weGood = stmt->nameAnalysis(symTab) && weGood;
	}	
	symTab->leaveScope();
	
	return weGood;
}


bool CallExpNode::nameAnalysis(SymbolTable *symTab) {
	bool nameAnalysisIsOk = false;
	bool temp1 = myID->nameAnalysis(symTab);
	bool temp2;
	for(auto args : *myArgs) {
		temp2 = args->nameAnalysis(symTab);
	}

	return (temp1 && temp2);
}

bool FormalDeclNode::nameAnalysis(SymbolTable *symTab) {
	std::string type = getTypeNode()->getType();
	std::string name = ID()->getName();
	
	if(!symTab->isVar(type))
	{
		NameErr::badVarType(pos());
	}
	if(symTab->isPresent(name))
	{
		NameErr::multiDecl(ID()->pos());
	}
	if(symTab->isVar(type) && !symTab->isPresent(name))
	{
		
		SemSymbol* temp = new SemSymbol(name, type, "var");
		ID()->attachSymbol(temp);
		symTab->insert(temp);
		
		
		return(true);
	}
	else {
		
		return( false);
	}
}

bool IDNode::nameAnalysis(SymbolTable * symTab){
	std::string myName = this->getName();
	SemSymbol * sym = symTab->find(myName);
	if(sym == nullptr){
		return NameErr::undeclID(pos());
	}
	
	this->attachSymbol(sym);
	return true;
}

bool UnaryExpNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

bool ExpNode::nameAnalysis(SymbolTable * symTab){
	return true;
}

}
