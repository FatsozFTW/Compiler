#include "ast.hpp"

namespace drewgon{
bool inFor = 0;
/*
doIndent is declared static, which means that it can 
only be called in this file (its symbol is not exported).
*/
static void doIndent(std::ostream& out, int indent){
	for (int k = 0 ; k < indent; k++){ out << "\t"; }
}

/*
In this code, the intention is that functions are grouped 
into files by purpose, rather than by class.
If you're used to having all of the functions of a class 
defined in the same file, this style may be a bit disorienting,
though it is legal. Thus, we can have
ProgramNode::unparse, which is the unparse method of ProgramNodes
defined in the same file as DeclNode::unparse, the unparse method
of DeclNodes.
*/


void ProgramNode::unparse(std::ostream& out, int indent){
	/* Oh, hey it's a for-each loop in C++!
	   The loop iterates over each element in a collection
	   without that gross i++ nonsense. 
	 */
	for (auto global : *myGlobals){
		/* The auto keyword tells the compiler
		   to (try to) figure out what the
		   type of a variable should be from 
		   context. here, since we're iterating
		   over a list of DeclNode *s, it's 
		   pretty clear that global is of 
		   type DeclNode *.
		*/
		global->unparse(out, indent);
	}
}

void VarDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
	if(inFor != true)
	{
		out << ";\n";
	}
	
}

void IDNode::unparse(std::ostream& out, int indent){
	out << this->name;
}

void IntTypeNode::unparse(std::ostream& out, int indent){
	out << "int";
}

void BoolTypeNode::unparse(std::ostream& out, int indent){
	out << "bool";
}

void VoidTypeNode::unparse(std::ostream& out, int indent){
	out << "void";
}

void FnDeclNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	this->myType->unparse(out, 0);
	out << " ";
	this->myId->unparse(out, 0);
	out << "(";
	bool once = true;
	if(myFormal != nullptr){
		for(auto formal : *myFormal){
			if(once)
			{
				once = false;
			}
			else{
				out << ", ";
			}
			formal->unparse(out, 0);
		}
	}
	out << ") {\n";
	for(auto stmt : *myBody){
		stmt->unparse(out, indent+1);
	}
	doIndent(out, indent);

	out << "}\n";
}

void FormalDeclNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	myType->unparse(out, 0);
	out << " ";
	myId->unparse(out, 0);
}

void FnTypeNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	out << "fn (";
	bool once = 1;
	if(myTypeList != nullptr) {
		for(auto fntype : *myTypeList){
			if(once)
			{
				once = 0;
			}
			else{
				out << ", ";
			}
			fntype->unparse(out, 0);
		}
	}
	out << ")->";
	myType->unparse(out, 0);


}

void AssignExpNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	myId->unparse(out, 0);
	out << " = ";
	myExp->unparse(out, 0);
}

void AssignStmtNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	myAssign->unparse(out, 0);
	if(inFor != true)
	{
		out << ";\n";
	}

}

void IntLiteralNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	out << myInt;
}

void StringLiteralNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	out << myStr;
}

void TrueNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	out << "true";
}

void FalseNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	out << "false";
}

void MayhemNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	out << "MAYHEM";
}

void CallExpNode::unparse(std::ostream& out, int indent)
{
	doIndent(out, indent);
	myId->unparse(out, indent);
	out << "(";
	
	bool once = 1;
	if(myCallList != nullptr)
	{
		for(auto call : *myCallList){
		if (once) 
		{ 
			once = 0; 
		}
		else { out << ", "; }
		call->unparse(out, indent);
		}	
	}
	
	out << ")";
}

void CallStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myCall->unparse(out, 0);
	if(inFor != true)
	{
		out << ";\n";
	}

}

void MinusNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " - ";
	myRight->unparse(out, 0);
}

void PlusNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " + ";
	myRight->unparse(out, 0);
}
void DivideNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " / ";
	myRight->unparse(out, 0);
}
void TimesNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " * ";
	myRight->unparse(out, 0);
}
void AndNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " && ";
	myRight->unparse(out, 0);
}
void OrNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " || ";
	myRight->unparse(out, 0);
}
void EqualsNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " == ";
	myRight->unparse(out, 0);
}
void NotEqualsNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " != ";
	myRight->unparse(out, 0);
}
void GreaterNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " > ";
	myRight->unparse(out, 0);
}
void GreaterEqNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " >= ";
	myRight->unparse(out, 0);
}
void LessNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " < ";
	myRight->unparse(out, 0);
}
void LessEqNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myLeft->unparse(out, 0); 
	out << " <= ";
	myRight->unparse(out, 0);
}

void NotNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(!";
	myExp->unparse(out, 0);
	out << ")";
}

void NegNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "(-";
	myExp->unparse(out, 0);
	out << ")";
}

void PostIncStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myId->unparse(out, 0);
	out << "++";
	if(inFor != true)
	{
		out << ";\n";
	}
}
void PostDecStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	myId->unparse(out, 0);
	out << "--";
	if(inFor != true)
	{
		out << ";\n";
	}
}

void InputStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "input ";
	myId->unparse(out, 0);
	if(inFor != true)
	{
		out << ";\n";
	}
}

void OutputStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "output ";
	myExp->unparse(out, 0);
	if(inFor != true)
	{
		out << ";\n";
	}
}

void ReturnStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "return";
	if(myExp !=nullptr)
	{
		out << " ";
		myExp->unparse(out, 0);
	}
	
	if(inFor != true)
	{
		out << ";\n";
	}
}

void IfStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "if (";
	myExp->unparse(out, 0);
	out << ") {\n";
	if(myBody != nullptr)
	{
		for(auto body : *myBody){
		
			body->unparse(out, indent+1);
		}	
	}
	doIndent(out, indent);
	out << "}\n";
}

void IfElseStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "if (";
	myExp->unparse(out, 0);
	out << ") {\n";
	if(myTrue != nullptr)
	{
		for(auto body : *myTrue){
		
			body->unparse(out, indent+1);
		}	
	}
	doIndent(out, indent);
	out << "} else {\n";
	if(myFalse != nullptr)
	{
		for(auto body : *myFalse){
		
			body->unparse(out, indent+1);
		}	
	}
	doIndent(out, indent);
	out << "}\n";
}

void WhileStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	out << "while (";
	this->myExp->unparse(out, 0);
	out << ") {\n";
	if(myBody != nullptr)
	{
		for(auto body : *myBody){
		
			body->unparse(out, indent+1);
		}	
	}
	doIndent(out, indent);
	out << "}\n";



}


void ForStmtNode::unparse(std::ostream& out, int indent){
	doIndent(out, indent);
	inFor = true;
	out << "for (";
	this->myInit->unparse(out, 0);
	this->myExp->unparse(out, 0);
	out << "; ";
	this->myUpdate->unparse(out, 0);
	out << ") {\n";
	inFor = false;
	if(myBody != nullptr)
	{
		for(auto body : *myBody){
		
			body->unparse(out, indent+1);
		}	
	}
	doIndent(out, indent);
	out << "}\n";



}

} // End namespace drewgon
