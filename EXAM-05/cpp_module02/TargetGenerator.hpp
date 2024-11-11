#ifndef TARGETGENERATOR_HPP
#define TARGETGENERATOR_HPP
#include <string>
#include <map>
#include "ATarget.hpp"

class	TargetGenerator
{
    TargetGenerator(const TargetGenerator &tgt);
    TargetGenerator &operator=(const TargetGenerator &tgt);
    std::map<std::string, ATarget &>	targets;
    
    public:
	TargetGenerator();
	~TargetGenerator();
	void	learnTargetType(ATarget *);
	void	forgetTargetType(const std::string &);
	ATarget	*createTarget(const std::string &);
};
#endif
