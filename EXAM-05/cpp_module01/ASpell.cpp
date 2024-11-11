#include "ASpell.hpp"

ASpell::ASpell(const std::string name, const std::string effects) : name(name), effects(effects) {}

ASpell::ASpell(const ASpell &asp) : name(asp.name), effects(asp.effects) {}

ASpell	&ASpell::operator=(const ASpell &asp)
{
	if (this != &asp)
	{
		name = asp.name;
		effects = asp.effects;
	}
	return (*this);
}

ASpell::~ASpell() {}

std::string	ASpell::getName() const { return (name); }
std::string ASpell::getEffects() const { return (effects); }

void	ASpell::launch(const ATarget &tg) const
{
	tg.getHitBySpell(*this);
}
