#ifndef ASPELL_HPP
#define ASPELL_HPP
#include <string>
#include <iostream>
#include "ATarget.hpp"

class	ATarget;

class	ASpell
{
	ASpell();

	protected:
		std::string	name;
		std::string	effects;

	public:
		ASpell(const std::string name, const std::string effects);
		ASpell(const ASpell &asp);
		ASpell	&operator=(const ASpell &asp);
		std::string	getName() const;
		std::string	getEffects() const;
		void		launch(const ATarget &tg) const;
		virtual ASpell	*clone() const=0;
		virtual ~ASpell();
};

#endif
