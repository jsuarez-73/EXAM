#ifndef ATARGET_HPP
#define ATARGET_HPP
#include <string>
#include <iostream>
#include "ASpell.hpp"

class	ASpell;

class	ATarget
{
	ATarget();

	protected:
		std::string	type;

	public:
		ATarget(const std::string type);
		ATarget(const ATarget &atg);
		ATarget	&operator=(const ATarget &atg);
		const std::string	&getType() const;
		void				getHitBySpell(const ASpell &asp) const;
		virtual ATarget	*clone() const=0;
		virtual ~ATarget();
};

#endif
