#ifndef WARLOCK_HPP
#define WARLOCK_HPP

#include <string>
#include <iostream>
#include <map>
#include "ASpell.hpp"
#include "ATarget.hpp"

class	Warlock
{
	std::string						name;
	std::string						title;
	std::map<std::string, ASpell *>	spells;
	Warlock(Warlock &wl);
	Warlock	&operator=(Warlock &wl);
	Warlock();

	public:
	const std::string	&getName()	const;
	const std::string	&getTitle()	const;
	void				setTitle(const std::string &title);
	void				introduce() const;
	void				learnSpell(ASpell *const asp);
	void				forgetSpell(const std::string spell);
	void				launchSpell(const std::string name, const ATarget &tg) const;
	Warlock(const std::string name, const std::string title);
	~Warlock();
};

#endif
