#include "ATarget.hpp"

ATarget::ATarget(const std::string type) : type(type) {}

ATarget::ATarget(const ATarget &tg) : type(tg.type) {}

ATarget	&ATarget::operator=(const ATarget &tg)
{
	if (this != &tg)
		type = tg.type;
	return (*this);
}

ATarget::~ATarget() {}

const std::string	&ATarget::getType() const { return (type); }

void	ATarget::getHitBySpell(const ASpell &asp) const
{
	std::cout << type << " has been " << asp.getEffects() << "!" << std::endl;
}
