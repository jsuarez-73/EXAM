#include "SpellBook.hpp"

SpellBook::SpellBook() {}
SpellBook::~SpellBook() {}

void	SpellBook::learnSpell(ASpell *asp)
{
    if (!asp)
	return ;
    book.insert(std::pair<std::string, ASpell *>(asp->getName(), asp));
}

void	SpellBook::forgetSpell(const std::string &spell)
{
	book.erase(spell);
}

ASpell	*SpellBook::createSpell(std::string const &spell)
{
    try {
	ASpell	*asp = book.at(spell);
	return (asp->clone());
    }
    catch (const std::out_of_range &e) { return ((ASpell *)0); }
}
