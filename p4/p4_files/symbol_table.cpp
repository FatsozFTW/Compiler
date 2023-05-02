#include "symbol_table.hpp"
#include "errors.hpp"
namespace drewgon{


SymbolTable::SymbolTable(){
	//TODO: implement the list of hashtables approach
	// to building a symbol table:
	// Upon entry to a scope a new scope table will be 
	// entered into the front of the chain and upon exit the 
	// latest scope table will be removed from the front of 
	// the chain.
	scopeTableChain = new std::list<ScopeTable *>();
}
ScopeTable::ScopeTable(){
	symbols = new HashMap<std::string, SemSymbol *>();
}

ScopeTable * SymbolTable::enterScope(){
	
	ScopeTable * newScope = new ScopeTable();
	scopeTableChain->push_front(newScope);
	
	return newScope;
}

void SymbolTable::leaveScope(){
	
	if (scopeTableChain->empty()){
		throw new InternalError("Attempt to pop"
			"empty symbol table");
	}
	scopeTableChain->pop_front();
	
}

ScopeTable * SymbolTable::getCurrentScope(){
	return scopeTableChain->front();
}

bool SymbolTable::isPresent(std::string varName){
	bool hasClash = getCurrentScope()->isPresent(varName);
	return hasClash;
}

SemSymbol * SymbolTable::find(std::string varName){
	for (ScopeTable * scope : *scopeTableChain){
		SemSymbol * sym = scope->lookup(varName);
		if (sym != nullptr) {  return sym; }
	}
	return nullptr;
}

void SymbolTable::insert(SemSymbol * symbol){
	
	scopeTableChain->front()->insert(symbol);
}

bool ScopeTable::isPresent(std::string varName){
	
	SemSymbol * found = lookup(varName);
	
	if (found != nullptr){
		
		return true;
	}
	return false;
}

SemSymbol * ScopeTable::lookup(std::string name){
	auto found = symbols->find(name);
	if (found == symbols->end()){
		
		return NULL;
	}
	
	return found->second;
}

bool ScopeTable::insert(SemSymbol * symbol){
	
	std::string symName = symbol->getName();
	bool alreadyInScope = (this->lookup(symName) != NULL);
	if (alreadyInScope){
		
		return false;
	}
	this->symbols->insert({symName, symbol});
	
	return true;
}

bool SymbolTable::isVar(std::string var){
	if(var != "void")
	{
		return(true);
	}
	else{
		return(false);
	}
}

}

