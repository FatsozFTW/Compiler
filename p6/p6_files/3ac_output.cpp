#include "ast.hpp"
bool inAssign = false;
bool inOutput = false;
namespace drewgon{

IRProgram * ProgramNode::to3AC(TypeAnalysis * ta){
	IRProgram * prog = new IRProgram(ta);
	for (auto global : *myGlobals){
		global->to3AC(prog);
	}
	return prog;
}
////////////////////////////////////////////////////////////////////   Scuffed formals
void FnDeclNode::to3AC(IRProgram * prog){

	Procedure * proc = prog->makeProc(this->myID->getName());
	unsigned int index = 1;
	for(auto stmt: *myFormals){
		stmt->to3AC(proc);
		SemSymbol * sym = stmt->ID()->getSymbol();
		SymOpd * opd = proc->getSymOpd(sym);
		Quad * quad = new GetArgQuad(index, opd);
		proc->addQuad(quad);
		index += 1;
	}
	for(auto stmt: *myBody){
		stmt->to3AC(proc);

	}
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	prog->gatherGlobal(sym);

}

void FnDeclNode::to3AC(Procedure * proc){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A function declaration
	// never occurs within another function)
	throw new InternalError("FnDecl at a local scope");
}

void FormalDeclNode::to3AC(IRProgram * prog){
	//This never needs to be implemented,
	// the function only exists because of
	// inheritance needs (A formal never
	// occurs at global scope)
	throw new InternalError("Formal at a global scope");
}

void FormalDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherFormal(sym);
}
////////////////////////////////////////////////////////////////////////////////////////// Also scuffed(changed 3ac, 3acprog and 3acproc)
Opd * MayhemNode::flatten(Procedure * proc){
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new MayhemQuad(dst);
	proc->addQuad(quad);
	return new LitOpd(dst->valString(), 8);
}
/////////////////////////////////////////////////////////////////////////////////////////
Opd * IntLitNode::flatten(Procedure * proc){
	const DataType * type = proc->getProg()->nodeType(this);
	return new LitOpd(std::to_string(myNum), 8);
}

Opd * StrLitNode::flatten(Procedure * proc){
	Opd * res = proc->getProg()->makeString(myStr);
	return res;
}

Opd * TrueNode::flatten(Procedure * proc){
	return new LitOpd("1",8);
}

Opd * FalseNode::flatten(Procedure * proc){
	return new LitOpd("0",8);
}

Opd * AssignExpNode::flatten(Procedure * proc){
	auto * dst = myDst->flatten(proc);
	auto * src = mySrc->flatten(proc);
	if (src != nullptr && dst != nullptr)
	{
		AssignQuad * quad = new AssignQuad(dst, src);
		proc->addQuad(quad);
		return src;
	}
	else{
		throw new InternalError("Null in AssignExpNode");
	}


}

Opd * CallExpNode::flatten(Procedure * proc){
	unsigned int index = 1;
	//setarg
	for(auto stmt: *myArgs){
		auto opd = stmt->flatten(proc);
		Quad * quad = new SetArgQuad(index, opd);
		proc->addQuad(quad);
		index += 1;

	}
	//call yada yada
	SemSymbol * sym = myID->getSymbol();
	Quad * quad = new CallQuad(sym);
	proc->addQuad(quad);
	
	
	//getret or not
	auto type = sym->getDataType()->asFn();
	if(type->getReturnType()->isVoid())
	{
		return nullptr;
	}
	else{
		Opd * retVal = proc->makeTmp(8);
		if(inAssign || inOutput)
		{
			Quad * getRet = new GetRetQuad(retVal);
			proc->addQuad(getRet);
		}
		
		return retVal;
	}
}

Opd * NegNode::flatten(Procedure * proc){
	Opd * exp = myExp->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new UnaryOpQuad(dst, NEG64, exp);
	proc->addQuad(quad);
	return dst;
}

Opd * NotNode::flatten(Procedure * proc){
	Opd * exp = myExp->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new UnaryOpQuad(dst, NOT8, exp);
	proc->addQuad(quad);
	return dst;
}

Opd * PlusNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, ADD64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * MinusNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, SUB64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * TimesNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, MULT64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * DivideNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, DIV64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * AndNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, AND64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * OrNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, OR64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * EqualsNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, EQ64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * NotEqualsNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, NEQ64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * LessNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, LT64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * GreaterNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, GT64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * LessEqNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, LTE64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}

Opd * GreaterEqNode::flatten(Procedure * proc){
	Opd * lhs = myExp1->flatten(proc);
	Opd * rhs = myExp2->flatten(proc);
	Opd * dst = proc->makeTmp(8);
	Quad * quad = new BinOpQuad(dst, GTE64, lhs, rhs);
	proc->addQuad(quad);
	return dst;
}
//Kina scuffed
void AssignStmtNode::to3AC(Procedure * proc){
	if(myExp != nullptr){
		inAssign = true;
		Opd * res = myExp->flatten(proc);
		inAssign = false;
		
	}
}

void PostIncStmtNode::to3AC(Procedure * proc){
	Opd * inc = myID->flatten(proc);
	Opd * one = new LitOpd("1", 8);
	Quad * quad = new BinOpQuad(inc, ADD64, inc, one);
	proc->addQuad(quad);
}

void PostDecStmtNode::to3AC(Procedure * proc){
	Opd * inc = myID->flatten(proc);
	Opd * one = new LitOpd("1", 8);
	Quad * quad = new BinOpQuad(inc, SUB64, inc, one);
	proc->addQuad(quad);
}

void OutputStmtNode::to3AC(Procedure * proc){
	inOutput = true;
	Opd * temp = mySrc->flatten(proc);
	const DataType * type = proc->getProg()->nodeType(mySrc);
	Quad * quad = new ReportQuad(temp, type);
	proc->addQuad(quad);
	inOutput = false;
}

void InputStmtNode::to3AC(Procedure * proc){
	Opd * temp = myDst->flatten(proc);
	const DataType * type = proc->getProg()->nodeType(myDst);
	Quad * quad = new ReceiveQuad(temp, type);
	proc->addQuad(quad);
}

void IfStmtNode::to3AC(Procedure * proc){
	Opd * cond = myCond->flatten(proc);
	Label * lbl_after = proc->makeLabel();
	Quad * after = new NopQuad();
	after->addLabel(lbl_after);
	Quad * quad = new IfzQuad(cond, lbl_after);
	proc->addQuad(quad);
	for(auto stmt: *myBody){
		stmt->to3AC(proc);
	}
	proc->addQuad(after);

}

void IfElseStmtNode::to3AC(Procedure * proc){
	Opd * cond = myCond->flatten(proc);
	Label * lbl_else = proc->makeLabel();
	Quad * elseNop = new NopQuad();
	elseNop->addLabel(lbl_else);

	Label * lbl_after = proc->makeLabel();
	Quad* after = new NopQuad();
	after->addLabel(lbl_after);

	Quad * quad = new IfzQuad(cond, lbl_else);
	proc->addQuad(quad);
	for(auto stmt: *myBodyTrue)
	{
		stmt->to3AC(proc);
	}

	Quad * after_quad = new GotoQuad(lbl_after);
	proc->addQuad(after_quad);
	proc->addQuad(elseNop);

	for(auto stmt: *myBodyFalse)
	{
		stmt->to3AC(proc);
	}
	proc->addQuad(after);
}

void WhileStmtNode::to3AC(Procedure * proc){
	Quad * headNop = new NopQuad();
	Label * lbl_head = proc->makeLabel();
	headNop->addLabel(lbl_head);

	Quad * after = new NopQuad();
	Label * lbl_after = proc->makeLabel();
	after->addLabel(lbl_after);

	//head start
	proc->addQuad(headNop);
	//while condition check
	Opd * cond = myCond->flatten(proc);
	Quad * quad = new IfzQuad(cond, lbl_after);
	proc->addQuad(quad);
	//body if true
	for(auto stmt: *myBody){
		stmt->to3AC(proc);
	}
	//back to head
	Quad * repeat = new GotoQuad(lbl_head);
	proc->addQuad(repeat);
	proc->addQuad(after);


}

void ForStmtNode::to3AC(Procedure * proc){
	Quad * headNop = new NopQuad();
	Label * lbl_head = proc->makeLabel();
	headNop->addLabel(lbl_head);

	Quad * after = new NopQuad();
	Label * lbl_after = proc->makeLabel();
	after->addLabel(lbl_after);
	
	//head start
	myInit->to3AC(proc);
	proc->addQuad(headNop);
	//for loop check
	

	Opd * cond = myCond->flatten(proc);

	Quad * quad = new IfzQuad(cond, lbl_after);
	proc->addQuad(quad);
	//body loop
	
	for(auto stmt: *myBody){
		stmt->to3AC(proc);
		
	}

	myItr->to3AC(proc);
	//back to head

	Quad * repeat = new GotoQuad(lbl_head);
	proc->addQuad(repeat);
	proc->addQuad(after);

}

void CallStmtNode::to3AC(Procedure * proc){
	Opd* res = myCallExp->flatten(proc);
}

void ReturnStmtNode::to3AC(Procedure * proc){
	if(myExp != nullptr){
		Opd * childOpd = myExp->flatten(proc);
		SetRetQuad * srq = new SetRetQuad(childOpd);
		proc->addQuad(srq);
	}
	Quad * quad = new GotoQuad(proc->getLeaveLabel());
	proc->addQuad(quad);
}

void VarDeclNode::to3AC(Procedure * proc){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	proc->gatherLocal(sym);
}

void VarDeclNode::to3AC(IRProgram * prog){
	SemSymbol * sym = ID()->getSymbol();
	assert(sym != nullptr);
	prog->gatherGlobal(sym);
}

//We only get to this node if we are in a stmt
// context (DeclNodes protect descent)
Opd * IDNode::flatten(Procedure * proc){
	SemSymbol * sym = this->getSymbol();
	Opd * res = proc->getSymOpd(sym);
	assert(res != nullptr);
	return res;
}

}
