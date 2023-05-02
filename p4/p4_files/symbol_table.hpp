#ifndef DREWGON_SYMBOL_TABLE_HPP
#define DREWGON_SYMBOL_TABLE_HPP
#include <string>
#include <unordered_map>
#include <list>
#include <iostream>

//Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using namespace std;

namespace drewgon{

//A semantic symbol, which represents a single
// variable, function, etc. Semantic symbols 
// exist for the lifetime of a scope in the 
// symbol table. 
class SemSymbol {
	//TODO add the fields that 
	// each semantic symbol should track
	// (i.e. the kind of the symbol (either a variable or function)
	// and functions to get/set those fields
	public:
		SemSymbol(std::string nameIn, std::string typeIn, std::string kindIn) : myName(nameIn), myType(typeIn), myKind(kindIn) { }
		

		std::string getType()
		{
			return(myType);
		}
		std::string getKind()
		{
			return(myKind);
		}
		std::string getName()
		{
			return(myName);
		}
		std::list<std::string> * getList()
		{
			return(formal_l);
		}
		void setList(std::list<std::string> * list)
		{
			formal_l = list;
		}
	private:
		std::string myName;
		std::string myType;
		std::string myKind;
		std::list<std::string> * formal_l;
		
};

//A single scope. The symbol table is broken down into a 
// chain of scope tables, and each scope table holds 
// semantic symbols for a single scope. For example,
// the globals scope will be represented by a ScopeTable,
// and the contents of each function can be represented by
// a ScopeTable.
class ScopeTable {
	public:
		ScopeTable();
		//TODO: add functions for looking up symbols
		// and/or returning information to indicate
		// that the symbol does not exist within the
		// current scope.
		SemSymbol * lookup(std::string name);
		bool insert(SemSymbol * symbol);
		bool isPresent(std::string name);
		



	private:
		HashMap<std::string, SemSymbol *> * symbols;
};

class SymbolTable{
	public:
		SymbolTable();
		//TODO: add functions to create a new ScopeTable
		// when a new scope is entered, drop a ScopeTable
		// when a scope is exited, etc. 
		ScopeTable* enterScope();
		void leaveScope();
		ScopeTable* getCurrentScope();
		void insert(SemSymbol * symbol);
		SemSymbol * find(std::string varName);
		bool isPresent(std::string name);
		bool isVar(std::string var);

	private:
		std::list<ScopeTable *> * scopeTableChain;
};

	
}

#endif
