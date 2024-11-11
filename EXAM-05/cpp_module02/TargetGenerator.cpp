#include "TargetGenerator.hpp"

TargetGenerator::TargetGenerator() {}
TargetGenerator::~TargetGenerator() {}

void	TargetGenerator::learnTargetType(ATarget *tg)
{
    if (!tg)
	return ;
    targets.insert(std::pair<std::string, ATarget &>(tg->getType(), *tg));
}
void	TargetGenerator::forgetTargetType(const std::string &type)
{
    targets.erase(type);
}

ATarget	*TargetGenerator::createTarget(const std::string &type)
{
    try {
	ATarget	&tg = targets.at(type);
	return (tg.clone());
    }
    catch (const std::out_of_range &e) { return ((ATarget *)0); }
}
