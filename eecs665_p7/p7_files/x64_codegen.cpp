#include <ostream>
#include "3ac.hpp"

namespace drewgon{

void IRProgram::allocGlobals(){
	//Choose a label for each global
	for(auto g : globals)
	{
		SymOpd * globalOpd = g.second;
		std::string memLoc = "gbl_";
		const auto sym = globalOpd->getSym();
		memLoc += sym->getName();
		globalOpd->setMemoryLoc("("+memLoc+")");

	}
	int count = 0;
	for(auto s : strings){
		std::string memLoc = "$str" + to_string(count);
		s.first->setMemoryLoc(memLoc);
		count++;

	}
}

void IRProgram::datagenX64(std::ostream& out){
	out << ".globl main\n";
	out << ".data\n";
	for(auto g : globals){
		SymOpd * globalOpd = g.second;
		std::string memLoc = "gbl_";
		const auto sym = globalOpd->getSym();
		memLoc += sym->getName();
		auto width = sym->getDataType()->getSize();
		out << memLoc << ":";
		if(width == 8){
			out << "\t.quad 0\n";
		}
		else{
			out << "\t.space" << width << "\n";
		}
	}
	int count = strings.size()-1;
	for(auto s : strings){
		
		auto globalOpd = s.second;
		auto width = s.second.length();
		std::string memLoc = "str_" + to_string(count)+ ":\n";
		out << memLoc << "\t.asciz " << s.second << "\n";
		//out << "\t.quad " << 8 << "\n";
		count--;

	}
	//Put this directive after you write out strings
	// so that everything is aligned to a quadword value
	// again
	out << ".align 8\n";
	out << ".text\n";
	

}

void IRProgram::toX64(std::ostream& out){
	allocGlobals();
	datagenX64(out);
	// Iterate over each procedure and codegen it
	for(auto proc: *this->procs)
	{
		proc->toX64(out);
	}
}

void Procedure::allocLocals(){
	//Allocate space for locals
	// Iterate over each procedure and codegen it
	int loc = 24;
	for( auto t : temps){
		t->setMemoryLoc("-" + to_string(loc) + "(%rbp)");
		loc += 8;
	}
	for( auto t : locals){
		t.second->setMemoryLoc("-"+ to_string(loc) + "(%rbp)");
		loc+= 8;
	}
	loc = 8 * formals.size() - 8;
	for( auto t : formals){
		t->setMemoryLoc(to_string(loc) + "(%rbp)");
		loc-= 8;
	}
	for (auto t : addrOpds){
		//ignore
		TODO(Implement me)
	}
	
}

void Procedure::toX64(std::ostream& out){
	//Allocate all locals
	allocLocals();

	enter->codegenLabels(out);
	enter->codegenX64(out);
	out << "#Fn body " << myName << "\n";
	for (auto quad : *bodyQuads){
		quad->codegenLabels(out);
		out << "#" << quad->toString() << "\n";
		quad->codegenX64(out);
	}
	out << "#Fn epilogue " << myName << "\n";
	leave->codegenLabels(out);
	leave->codegenX64(out);
}

void Quad::codegenLabels(std::ostream& out){
	if (labels.empty()){ return; }

	size_t numLabels = labels.size();
	size_t labelIdx = 0;
	for ( Label * label : labels){
		out << label->getName() << ": ";
		if (labelIdx != numLabels - 1){ out << "\n"; }
		labelIdx++;
	}
}

void BinOpQuad::codegenX64(std::ostream& out){
	src1->genLoadVal(out, A); 
	src2->genLoadVal(out, B);
	switch (opr) {
	case ADD64:
		out << "addq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		dst->genStoreVal(out, A); 
		break;
	case SUB64:
		out << "subq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		dst->genStoreVal(out, A);
		break;
	case DIV64:
		out << "movq $0, " << RegUtils::reg64(D) << "\n";
		out << "idivq " << RegUtils::reg64(B) << "\n";
		dst->genStoreVal(out, A);
		break;
	case MULT64:
		out << "imulq " << RegUtils::reg64(B) << "\n";
		dst->genStoreVal(out, A);
		break;
	case EQ64:
		out << "cmpq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		out << "sete " << RegUtils::reg8(A) << "\n";
		out << "movq " << RegUtils::reg64(A) << ", " << RegUtils::reg64(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case NEQ64:
		out << "cmpq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		out << "setne " << RegUtils::reg8(A) << "\n";
		out << "movq " << RegUtils::reg64(A) << ", " << RegUtils::reg64(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case LT64:
		out << "cmpq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		out << "setl " << RegUtils::reg8(A) << "\n";
		out << "movq " << RegUtils::reg64(A) << ", " << RegUtils::reg64(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case GT64:
		out << "cmpq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		out << "setg " << RegUtils::reg8(A) << "\n";
		out << "movq " << RegUtils::reg64(A) << ", " << RegUtils::reg64(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case LTE64:
		out << "cmpq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		out << "setle " << RegUtils::reg8(A) << "\n";
		out << "movq " << RegUtils::reg64(A) << ", " << RegUtils::reg64(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case GTE64:
		out << "cmpq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		out << "setge " << RegUtils::reg8(A) << "\n";
		out << "movq " << RegUtils::reg64(A) << ", " << RegUtils::reg64(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case OR64:
		out << "orq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		dst->genStoreVal(out, A);
		break;
	case AND64:
		out << "andq " << RegUtils::reg64(B) << ", " << RegUtils::reg64(A) << "\n";
		dst->genStoreVal(out, A);
		break;
 	case ADD8:
		out << "addb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		dst->genStoreVal(out, A); 
  		break;
	case SUB8:
		out << "subb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		dst->genStoreVal(out, A);
		break;
	case DIV8:
		out << "movq $0, " << RegUtils::reg8(D) << "\n";
		out << "idivb " << RegUtils::reg8(B) << "\n";
		dst->genStoreVal(out, A);
		break;
	case MULT8:
		out << "imulb " << RegUtils::reg8(B) << "\n";
		dst->genStoreVal(out, A);
		break;
	case EQ8:
		out << "cmpb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		out << "sete " << RegUtils::reg8(A) << "\n";
		out << "movb " << RegUtils::reg8(A) << ", " << RegUtils::reg8(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case NEQ8:
		out << "cmpb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		out << "setne " << RegUtils::reg8(A) << "\n";
		out << "movb " << RegUtils::reg8(A) << ", " << RegUtils::reg8(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case LT8:
		out << "cmpb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		out << "setl " << RegUtils::reg8(A) << "\n";
		out << "movb " << RegUtils::reg8(A) << ", " << RegUtils::reg8(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case GT8:
		out << "cmpb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		out << "setg " << RegUtils::reg8(A) << "\n";
		out << "movb " << RegUtils::reg8(A) << ", " << RegUtils::reg8(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case LTE8:
		out << "cmpb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		out << "setle " << RegUtils::reg8(A) << "\n";
		out << "movb " << RegUtils::reg8(A) << ", " << RegUtils::reg8(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case GTE8:
		out << "cmpb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		out << "setge " << RegUtils::reg8(A) << "\n";
		out << "movb " << RegUtils::reg8(A) << ", " << RegUtils::reg8(D) << "\n";
		dst->genStoreVal(out, A);
		break;
	case OR8:
		out << "orb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		dst->genStoreVal(out, A);
		break;
	case AND8:
		out << "andb " << RegUtils::reg8(B) << ", " << RegUtils::reg8(A) << "\n";
		dst->genStoreVal(out, A);
		break;
}
}
void UnaryOpQuad::codegenX64(std::ostream& out){
	src->genLoadVal(out, A); 
	switch (op) {
	case NEG64:
		out << "negq " << RegUtils::reg64(A) << "\n";
		dst->genStoreVal(out, A); 
		break;
	case NEG8:
		out << "negb " << RegUtils::reg8(A) << "\n";
		dst->genStoreVal(out, A);
		break;
	
	case NOT64:
		out << "notq " << RegUtils::reg64(A) << "\n";
		dst->genStoreVal(out, A); 
		break;
	case NOT8:
		out << "notb " << RegUtils::reg8(A) << "\n";
		dst->genStoreVal(out, A);
		break;
	}
}

void AssignQuad::codegenX64(std::ostream& out){
	src->genLoadVal(out, A);
	dst->genStoreVal(out, A);
}

void GotoQuad::codegenX64(std::ostream& out){
	out << "jmp " << tgt->getName() << "\n";
}

void IfzQuad::codegenX64(std::ostream& out){
	cnd->genLoadVal(out, EL);
	out << "cmpq $0, %r11\n";
	out << "je " << tgt->toString() << "\n";
}

void NopQuad::codegenX64(std::ostream& out){
	out << "nop" << "\n";
}

void IntrinsicMayhemQuad::codegenX64(std::ostream& out){
	TODO(Implement me)
}

void IntrinsicOutputQuad::codegenX64(std::ostream& out){
	if (myType->isBool()){
		myArg->genLoadVal(out, DI);
		out << "callq printBool\n";
	} 
	else if(myType->isString()) {
		myArg->genLoadVal(out, DI);
		out << "callq printString\n";
	}
	else{ //int
		myArg->genLoadVal(out, DI);
		out << "callq printInt\n";
	}
}

void IntrinsicInputQuad::codegenX64(std::ostream& out){
	if (myType->isBool()){
		out << "callq getBool\n";
		myArg->genStoreVal(out, A);
		
	} 
	else if(myType->isString()) {
		out << "callq getString\n";
		myArg->genStoreVal(out, A);
		
	}
	else{ //int
		out << "callq getInt\n";
		myArg->genStoreVal(out, A);
		
	}
}
int glbl_index = 0;
void CallQuad::codegenX64(std::ostream& out){
	out << "callq fun_" << callee->getName() << "\n";
	int amount = 8*(glbl_index-6);
	out << "addq $" << amount << ", %rsp\n";

}

void EnterQuad::codegenX64(std::ostream& out){
	out << "subq $8, %rsp\n";
	out << "movq %rbp, (%rsp)\n";
	out << "movq %rsp, %rbp\n";
	out << "addq $16, %rbp\n";
	out << "subq $" << ( myProc->arSize()) << ", %rsp\n"; //myProc->numLocals() +
}

void LeaveQuad::codegenX64(std::ostream& out){
	out << "addq $" << (myProc->arSize()) << ", %rsp\n"; //myProc->numLocals() + 
	out << "movq (%rsp), %rbp\n";
	out << "addq $8, %rsp\n";
	out << "retq\n";
}

void SetArgQuad::codegenX64(std::ostream& out){
	glbl_index = index;
	if(index == 1){
		opd->genLoadVal(out, Register::DI);
	}
	if(index == 2){
		opd->genLoadVal(out, Register::SI);
	}
	if(index == 3){
		opd->genLoadVal(out, Register::D);
	}
	if(index == 4){
		opd->genLoadVal(out, Register::C);
	}
	if(index == 5){
		opd->genLoadVal(out, Register::EIGHT);
	}
	if(index == 6){
		opd->genLoadVal(out, Register::NINE);
	}
	if(index >= 7)
	{
		if(opd->getMemoryLoc() == "UNINIT"){
			out << "pushq $" << opd->valString() << "\n";
		}
		else{
			out << "movq " << opd->getMemoryLoc() << ", %r12\n";
			out << "pushq %r12\n";
		}
		
		

	}

}

void GetArgQuad::codegenX64(std::ostream& out){
	//We don't actually need to do anything here LIES!!!!
	if(index == 1){
		
		opd->genStoreVal(out, Register::DI);
	}
	if(index == 2){
		opd->genStoreVal(out, Register::SI);
	}
	if(index == 3){
		opd->genStoreVal(out, Register::D);
	}
	if(index == 4){
		opd->genStoreVal(out, Register::C);
	}
	if(index == 5){
		opd->genStoreVal(out, Register::EIGHT);
	}
	if(index == 6){
		opd->genStoreVal(out, Register::NINE);
	}
	if(index >= 7)
	{

	}
}

void SetRetQuad::codegenX64(std::ostream& out){
	//pop
			
	opd->genLoadVal(out, A);
}

void GetRetQuad::codegenX64(std::ostream& out){
	opd->genStoreVal(out, A);
}

void LocQuad::codegenX64(std::ostream& out){
	TODO(Implement me) //Ignore?
}

void SymOpd::genLoadVal(std::ostream& out, Register reg){
	//fix me:
	auto regstr = this->getReg(reg);
	out << getMovOp()<< " " << this->getMemoryLoc() << ", " << regstr << "\n";
}

void SymOpd::genStoreVal(std::ostream& out, Register reg){
	//fix me:
	auto regstr = this->getReg(reg);
	out << getMovOp() << " "<< regstr << ", " << this->getMemoryLoc() << "\n"; 
}

void SymOpd::genLoadAddr(std::ostream& out, Register reg) {
	TODO(Implement me if necessary) //no use?
}

void AuxOpd::genLoadVal(std::ostream& out, Register reg){
	auto regstr = this->getReg(reg);
	out << getMovOp() << " "<< this->getMemoryLoc() << ", " << regstr << "\n";
}

void AuxOpd::genStoreVal(std::ostream& out, Register reg){
	auto regstr = this->getReg(reg);
	out << getMovOp() << " " << regstr << ", " << this->getMemoryLoc() << "\n";
}
void AuxOpd::genLoadAddr(std::ostream& out, Register reg){
	TODO(Implement me) // no use?
}

/// No do??

void AddrOpd::genStoreVal(std::ostream& out, Register reg){
	TODO(Implement me)
}

void AddrOpd::genLoadVal(std::ostream& out, Register reg){
	TODO(Implement me)
}

void AddrOpd::genStoreAddr(std::ostream& out, Register reg){
	TODO(Implement me)
}

void AddrOpd::genLoadAddr(std::ostream & out, Register reg){
	TODO(Implement me)
}
///
void LitOpd::genLoadVal(std::ostream & out, Register reg){
	out << getMovOp() << " $" << val << ", " << getReg(reg) << "\n";
}

}
