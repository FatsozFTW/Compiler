#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"
#include <list>
#include <iterator>
namespace drewgon{

TypeAnalysis * TypeAnalysis::build(NameAnalysis * nameAnalysis){
	//To emphasize that type analysis depends on name analysis
	// being complete, a name analysis must be supplied for 
	// type analysis to be performed.
	TypeAnalysis * typeAnalysis = new TypeAnalysis();
	auto ast = nameAnalysis->ast;	
	typeAnalysis->ast = ast;

	ast->typeAnalysis(typeAnalysis);
	if (typeAnalysis->hasError){
		return nullptr;
	}

	return typeAnalysis;

}

void ProgramNode::typeAnalysis(TypeAnalysis * ta){

	//pass the TypeAnalysis down throughout
	// the entire tree, getting the types for
	// each element in turn and adding them
	// to the ta object's hashMap
	for (auto global : *myGlobals){
		global->typeAnalysis(ta);
	}

	//The type of the program node will never
	// be needed. We can just set it to VOID
	//(Alternatively, we could make our type 
	// be error if the DeclListNode is an error)
	ta->nodeType(this, BasicType::produce(VOID));
}

void FnDeclNode::typeAnalysis(TypeAnalysis * ta){

	//HINT: you might want to change the signature for
	// typeAnalysis on FnBodyNode to take a second
	// argument which is the type of the current 
	// function. This will help you to know at a 
	// return statement whether the return type matches
	// the current function

	
	//Note: this function may need extra code
	
	
	auto * argNodes = new std::list<TypeNode *>();
	for(auto formal : *myFormals)
	{
		argNodes->push_back(formal->getTypeNode());
	}
	auto * args = TypeList::produce(argNodes);
	auto * myRet = myRetType->getType();

	auto * fnType = FnType::produce(args, myRet);

	ta->setCurrentFnType(fnType);
	ta->nodeType(this, fnType);

	for (auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}
}

void CallStmtNode::typeAnalysis(TypeAnalysis *ta){
	myCallExp->typeAnalysis(ta);
	ta->nodeType(this, BasicType::produce(VOID));
}

void StmtNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Implement me in the subclass");
}

void AssignStmtNode::typeAnalysis(TypeAnalysis * ta){
	myExp->typeAnalysis(ta);

	//It can be a bit of a pain to write 
	// "const DataType *" everywhere, so here
	// the use of auto is used instead to tell the
	// compiler to figure out what the subType variable
	// should be
	auto subType = ta->nodeType(myExp);

	// As error returns null if subType is NOT an error type
	// otherwise, it returns the subType itself
	if (subType->asError()){
		ta->nodeType(this, subType);
	} else {
		//std::cout << "here in assign?";
		ta->nodeType(this, subType);
	}
}

void ExpNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void AssignExpNode::typeAnalysis(TypeAnalysis * ta){
	//TODO: Note that this function is incomplete. 
	// and needs additional code

	//Do typeAnalysis on the subexpressions
	myDst->typeAnalysis(ta);
	mySrc->typeAnalysis(ta);

	const DataType * tgtType = ta->nodeType(myDst);
	const DataType * srcType = ta->nodeType(mySrc);

	//While incomplete, this gives you one case for 
	// assignment: if the types are exactly the same
	// it is usually ok to do the assignment. One
	// exception is that if both types are function
	// names, it should fail type analysis
	
	if(tgtType->asError() || srcType->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	else if(tgtType->asFn() || tgtType->isVoid()){
		ta->errAssignOpd(myDst->pos());
		if(srcType->asFn() || srcType->isVoid()){
			ta->errAssignOpd(mySrc->pos());
		}
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	else if(srcType->asFn() || srcType->isVoid()){
		ta->errAssignOpd(mySrc->pos());
		if(tgtType->asFn() || tgtType->isVoid()){
			ta->errAssignOpd(myDst->pos());
		}
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	else if(tgtType == srcType)
	{
		if (!(tgtType->validVarType())) { // void == void
			ta->errAssignOpr(myDst->pos());
			ta->errAssignOpr(mySrc->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		ta->nodeType(this, tgtType);
		return;
	}
	else{ //bool != int
		ta->errAssignOpr(this->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	
	//Some functions are already defined for you to
	// report type errors. Note that these functions
	// also tell the typeAnalysis object that the
	// analysis has failed, meaning that main.cpp
	// will print "Type check failed" at the end
	//ta->errAssignOpr(this->pos());


	//Note that reporting an error does not set the
	// type of the current node, so setting the node
	// type must be done
	//ta->nodeType(this, ErrorType::produce());
}

void DeclNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(VOID));
	TODO("Override me in the subclass");
}

void VarDeclNode::typeAnalysis(TypeAnalysis * ta){
	// VarDecls always pass type analysis, since they 
	// are never used in an expression. You may choose
	// to type them void (like this), as discussed in class
	//std::cout << "here in var?";
	ta->nodeType(this, BasicType::produce(VOID));
}

void FormalDeclNode::typeAnalysis(TypeAnalysis * ta){
	//std::cout << "here in form?";
	ta->nodeType(this, BasicType::produce(VOID));
}

void PlusNode::typeAnalysis(TypeAnalysis * ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);

	const DataType * l = ta->nodeType(myExp1);
	const DataType * r = ta->nodeType(myExp2);

	if (l->isInt()){
		if(r->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else if(r->isInt()){
		if(l->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp1->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else{
		ta->errMathOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		if(!r->isInt()){
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		return;
	}

}

void MinusNode::typeAnalysis(TypeAnalysis * ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);

	const DataType * l = ta->nodeType(myExp1);
	const DataType * r = ta->nodeType(myExp2);

	if (l->isInt()){
		if(r->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else if(r->isInt()){
		if(l->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp1->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else{
		ta->errMathOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		if(!r->isInt()){
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		return;
	}

}

void TimesNode::typeAnalysis(TypeAnalysis * ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);

	const DataType * l = ta->nodeType(myExp1);
	const DataType * r = ta->nodeType(myExp2);

	if (l->isInt()){
		if(r->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else if(r->isInt()){
		if(l->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp1->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else{
		ta->errMathOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		if(!r->isInt()){
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		return;
	}

}

void DivideNode::typeAnalysis(TypeAnalysis * ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);

	const DataType * l = ta->nodeType(myExp1);
	const DataType * r = ta->nodeType(myExp2);

	if (l->isInt()){
		if(r->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else if(r->isInt()){
		if(l->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		else{
			ta->errMathOpd(myExp1->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else{
		ta->errMathOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		if(!r->isInt()){
			ta->errMathOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		return;
	}

}

void PostDecStmtNode::typeAnalysis(TypeAnalysis *ta){
	myID->typeAnalysis(ta);
	auto id = ta->nodeType(myID);
	if(!id->isInt())
	{
		ta->errMathOpd(myID->pos());
		ta->nodeType(this, ErrorType::produce());
	}
	else{
		ta->nodeType(this, BasicType::produce(INT));
	}
	
}

void PostIncStmtNode::typeAnalysis(TypeAnalysis *ta){
	myID->typeAnalysis(ta);
	auto id = ta->nodeType(myID);
	if(!id->isInt())
	{
		ta->errMathOpd(myID->pos());
		ta->nodeType(this, ErrorType::produce());
	}
	else{
		ta->nodeType(this, BasicType::produce(INT));
	}
	
}

void IDNode::typeAnalysis(TypeAnalysis * ta){
	// IDs never fail type analysis and always
	// yield the type of their symbol (which
	// depends on their definition)
	ta->nodeType(this, this->getSymbol()->getDataType());
}

void IntLitNode::typeAnalysis(TypeAnalysis * ta){
	// IntLits never fail their type analysis and always
	// yield the type INT
	ta->nodeType(this, BasicType::produce(INT));
}
void StrLitNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(STRING));
}

void TrueNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(BOOL));
}

void FalseNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(BOOL));
}

void CallExpNode::typeAnalysis(TypeAnalysis * ta){
	myID->typeAnalysis(ta);
	auto myFn = ta->nodeType(myID)->asFn();
	if(myFn == nullptr)
	{
		ta->errCallee(myID->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	auto myFormals = myFn->getFormalTypes();
	auto formalSize = myFormals->count();
	if (myArgs->size() != formalSize){
		ta->errArgCount(this->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	bool in = 0;
	const std::list<const DataType *> temp_types = *myFormals->getTypes();
	std::list<const DataType *> types = temp_types;
	std::list<const DataType *>::iterator it = types.begin();
	
	for (auto arg : *myArgs){
		arg->typeAnalysis(ta);
		if (ta->nodeType(arg)->getString() != (*it)->getString()){
			ta->errArgMatch(arg->pos());
			ta->nodeType(this, ErrorType::produce());
			in = 1;	
			
		}
		advance(it, 1);
	}
	if(in)
	{
		
		return;
	}
	ta->setCurrentFnType(myFn);
	ta->nodeType(this, myFn->getReturnType());
}

void ReturnStmtNode::typeAnalysis(TypeAnalysis *ta) {
	auto fnType = ta->getCurrentFnType();
	if (myExp == nullptr){ //no return value
		if (fnType->getReturnType()->isVoid()){
			ta->nodeType(this, BasicType::produce(VOID));
			return;
		}
		else{
			ta->errRetEmpty(pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	//expNode not empty
	myExp->typeAnalysis(ta);
	auto myType = ta->nodeType(myExp);
	//return with value in void
	if (fnType->getReturnType()->isVoid()){
		ta->extraRetValue(myExp->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	//wrong type
	if (fnType->getReturnType() != myType){
		ta->errRetWrong(myExp->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	ta->nodeType(this, myType);
}

void OutputStmtNode::typeAnalysis(TypeAnalysis *ta){//Attemp to output void not showing up for "output voidfunc()" bc of ERROR type
	mySrc->typeAnalysis(ta);
	auto srcType = ta->nodeType(mySrc);

	if (srcType->asFn())
	{
		ta->errOutputFn(mySrc->pos());
		ta->nodeType(this, ErrorType::produce());
	}
	else if (srcType == BasicType::produce(VOID))
	{
		ta->errOutputVoid(mySrc->pos());
		ta->nodeType(this, ErrorType::produce());
	}
	else
	{
		ta->nodeType(this, srcType);
	}
}

void InputStmtNode::typeAnalysis(TypeAnalysis *ta){
	myDst->typeAnalysis(ta);
	auto dstType = ta->nodeType(myDst);
	if (dstType->asFn())
	{
		ta->errAssignFn(myDst->pos());
		ta->nodeType(this, ErrorType::produce());
	}
	else
	{
		ta->nodeType(this, dstType);
	}
}

void BinaryExpNode::typeAnalysis(TypeAnalysis *ta){ // this only covers == and !=, funcs are allowed
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	std::cout << "Do we ever get in here??";
	ta->nodeType(this, BasicType::produce(BOOL));
}

void EqualsNode::typeAnalysis(TypeAnalysis *ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);
	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	else if(l->asFn() || r->asFn() || l->isVoid() || r->isVoid()){
		if(l->asFn() || l->isVoid()){
			ta->errEqOpd(myExp1->pos());
		}
		if(r->asFn() || r->isVoid()){
			ta->errEqOpd(myExp2->pos());
		}
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	else if (l == r) {
		if((l->isInt()) || (l->isBool()) || (l->isString())) {
			ta->nodeType(this, BasicType::produce(BOOL));
		}
		else {
			ta->errEqOpd(myExp1->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		if((r->isInt()) || (r->isBool()) || (r->isString())) {
			ta->nodeType(this, BasicType::produce(BOOL));
		}
		else {
			ta->errEqOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else { // not the same type, so right side is the problem
		ta->errEqOpr(this->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}

}

void NotEqualsNode::typeAnalysis(TypeAnalysis *ta) {
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);
	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	else if(l->asFn() || r->asFn() || l->isVoid() || r->isVoid()){
		if(l->asFn() || l->isVoid()){
			ta->errEqOpd(myExp1->pos());
		}
		if(r->asFn() || r->isVoid()){
			ta->errEqOpd(myExp2->pos());
		}
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	else if (l == r) {
		if((l->isInt()) || (l->isBool()) || (l->isString())) {
			ta->nodeType(this, BasicType::produce(BOOL));
		}
		else {
			ta->errEqOpd(myExp1->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
		if((r->isInt()) || (r->isBool()) || (r->isString())) {
			ta->nodeType(this, BasicType::produce(BOOL));
		}
		else {
			ta->errEqOpd(myExp2->pos());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	else { // not the same type, so right side is the problem
		ta->errEqOpr(this->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}


}

void OrNode::typeAnalysis(TypeAnalysis *ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);

	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}


	if(l->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errLogicOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		//return;
	}
	if(r->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errLogicOpd(myExp2->pos());
		ta->nodeType(this, ErrorType::produce());
		
	}
	return;
}

void AndNode::typeAnalysis(TypeAnalysis *ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);

	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}


	if(l->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errLogicOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		//return;
	}
	if(r->isBool()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errLogicOpd(myExp2->pos());
		ta->nodeType(this, ErrorType::produce());
		
	}
	return;
}

void LessNode::typeAnalysis(TypeAnalysis *ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);

	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}


	if(l->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		//return;
	}
	if(r->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp2->pos());
		ta->nodeType(this, ErrorType::produce());
		
	}
	return;
}

void LessEqNode::typeAnalysis(TypeAnalysis *ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);

	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}


	if(l->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		//return;
	}
	if(r->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp2->pos());
		ta->nodeType(this, ErrorType::produce());
		
	}
	return;
}

void GreaterEqNode::typeAnalysis(TypeAnalysis *ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);

	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}


	if(l->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		//return;
	}
	if(r->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp2->pos());
		ta->nodeType(this, ErrorType::produce());
		
	}
	return;
}

void GreaterNode::typeAnalysis(TypeAnalysis *ta){
	myExp1->typeAnalysis(ta);
	myExp2->typeAnalysis(ta);
	
	auto l = ta->nodeType(myExp1); 
	auto r = ta->nodeType(myExp2);

	if (l->asError() || r->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}


	if(l->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp1->pos());
		ta->nodeType(this, ErrorType::produce());
		//return;
	}
	if(r->isInt()) {
		ta->nodeType(this, BasicType::produce(BOOL));
	}
	else {
		ta->errRelOpd(myExp2->pos());
		ta->nodeType(this, ErrorType::produce());
		
	}
	return;
}

void NegNode::typeAnalysis(TypeAnalysis *ta){ 
  myExp->typeAnalysis(ta);
  auto exp = ta->nodeType(myExp);
  if (exp->isInt()){
    ta->nodeType(this, exp);
  } else {
    ta->errMathOpd(myExp->pos());
    ta->nodeType(this, ErrorType::produce());
  }
}

void NotNode::typeAnalysis(TypeAnalysis *ta){
	myExp->typeAnalysis(ta);
  	auto exp = ta->nodeType(myExp);	
	if (exp->isBool()) {
    	ta->nodeType(this, exp);
	}
	else {
		ta->errLogicOpd(myExp->pos());
		ta->nodeType(this, ErrorType::produce());
	}
}

void WhileStmtNode::typeAnalysis(TypeAnalysis *ta){
	myCond->typeAnalysis(ta);
	if (!ta->nodeType(myCond)->isBool())
	{
		ta->errLoopCond(myCond->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for (auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void ForStmtNode::typeAnalysis(TypeAnalysis *ta){
	myCond->typeAnalysis(ta);
	myInit->typeAnalysis(ta);
	myItr->typeAnalysis(ta);
	bool in = 0;
	
	if (!ta->nodeType(myInit)->isInt())
	{

		ta->errLoopCond(myInit->pos());
		
		in =1;
	}
	if (!ta->nodeType(myCond)->isBool())
	{

		ta->errLoopCond(myCond->pos());
		
		in =1;
	}
	if (!ta->nodeType(myItr)->isInt())
	{
		ta->errLoopCond(myItr->pos());
		
		in =1;
	}
	if(in)
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for (auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void IfStmtNode::typeAnalysis(TypeAnalysis *ta){
	myCond->typeAnalysis(ta);

	if (!ta->nodeType(myCond)->isBool())
	{
		ta->errIfCond(myCond->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for (auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis *ta){
	myCond->typeAnalysis(ta);

	if (!ta->nodeType(myCond)->isBool())
	{
		ta->errIfCond(myCond->pos());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	for (auto stmt : *myBodyTrue){
		stmt->typeAnalysis(ta);
	}
	for (auto stmt : *myBodyFalse){
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void MayhemNode::typeAnalysis(TypeAnalysis *ta){
	ta->nodeType(this, BasicType::produce(INT));
}

}