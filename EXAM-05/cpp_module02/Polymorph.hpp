#ifndef	POLYMORPH_HPP
#define	POLYMORPH_HPP
#include <string>
#include <iostream>
#include "ASpell.hpp"

class	Polymorph : public ASpell
{
    public:
	Polymorph();
	~Polymorph();
	ASpell	*clone() const;
};

#endif
