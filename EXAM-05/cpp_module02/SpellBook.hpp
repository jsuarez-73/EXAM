#ifndef SPELLBOOK_HPP
#define SPELLBOOK_HPP
#include <string>
#include <iostream>
#include <map>
#include "ASpell.hpp"

class	SpellBook
{
    SpellBook(SpellBook &sb);
    SpellBook	&operator=(SpellBook &sp);
    std::map<std::string, ASpell *> book;

    public:
	SpellBook();
	~SpellBook();
	void	learnSpell(ASpell *);
	void	forgetSpell(std::string const &);
	ASpell	*createSpell(std::string const &);
};
#endif
