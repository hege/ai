
#pragma once



#include <stdexcept>
#include <string>
#include <vector>
#include <map>


namespace AI {
	namespace Agents {
		class Agent_skeleton;
		class AgentInEnv_skeleton;
		template<class _PercT, class _ActT> class AgentReacting_skeleton;
	}
}


#include "enviroment.h"


namespace AI {
namespace Agents {

class bad_agent : public std::exception
{
public:
	bad_agent(const char *const& msg) : std::exception(msg) { ; };
};



/*               ________________________
				/                        \
		* ---==<	  Agent skeletons     >==--- *
				\________________________/
*/



class Agent_skeleton
{
protected:
	std::vector<std::string>	ops;
protected:
	virtual	double	getCost(const std::string &op) const = 0;
public:
	virtual bool	doOp(const std::string &op, double &cost) = 0;

	virtual	void	enumOps(std::vector<std::string> &opnames) const { opnames.assign(ops.begin(), ops.end()); };
};



template<class _PercT, class _ActT>
class AgentReacting_skeleton : public Agent_skeleton
{
	virtual	_ActT	react(_PercT) = 0;
};



class AgentInEnv_skeleton : public Agent_skeleton
{
protected:
	Enviroment &	myEnv;
public:
	AgentInEnv_skeleton( Enviroment &env ) : myEnv(env) { ; };

	const Enviroment& getEnv( ) const { return myEnv; };

	virtual bool	doOp( Enviroment &env,const std::string &op, double &cost ) = 0;

	virtual double	getCost( const Enviroment &env, const std::string &op ) const = 0;

	virtual	void	enumOps( std::vector<std::string> &opnames ) const { Agent_skeleton::enumOps(opnames); };
	virtual	void	enumOps( const Enviroment &env, std::vector<std::string> &opnames ) const = 0;
};






/*               ________________________
				/                        \
		* ---==<	Table driven agent    >==--- *
				\________________________/
*/



template<class _PercT, class _ActT>
class TableDriven : public AgentReacting_skeleton<_PercT, _ActT>
{
protected:
	std::map<_PercT, _ActT>		_myTable;

public:

	void	addTable( _PercT preception, _ActT reaction ) throw ( bad_agent );

	void	remTable( _PercT perception ) throw ( bad_agent );

	void	clear( ) { return _myTable.clear(); };
	bool	empty( ) const { return _myTable.empty(); };
	size_t	size( ) const { return _myTable.count(); };

	// Overrides
	_ActT	react( _PercT perception ) throw ( bad_agent );
};

template<class _PercT, class _ActT>
void TableDriven<_PercT, _ActT>::addTable(_PercT perception, _ActT reaction) throw ( bad_agent )
{
	if ( _myTable.find(perception) != _myTable.end() )
		throw bad_agent("Perception already defined in table");
	else
		_myTable[perception] = reaction;
}

template<class _PercT, class _ActT>
void TableDriven<_PercT, _ActT>::remTable(_PercT perception) throw ( bad_agent )
{
	if ( _myTable.find(perception) == _myTable.end() )
		throw bad_agent("Perception not defined in table");
	else
		_myTable.erase(perception);
}

template<class _PercT, class _ActT>
_ActT TableDriven<_PercT, _ActT>::react(_PercT perception) throw ( bad_agent )
{
	std::map<_PercT, _ActT>::iterator it = _myTable.find(perception);
	if ( it == _myTable.end() )
		throw bad_agent("Perception not defined in table");
	else
		return it->second;
}



}; /* end of namespace Agents */

}; /* end of namespace AI */
