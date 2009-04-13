
#pragma once



#include <stdexcept>
#include <map>


namespace AI {
	namespace Agents {
		class World;
	}
}


#include "agents.h"
#include "enviroment.h"


namespace AI {
namespace Agents {

class bad_world : public std::exception
{
public:
	bad_world(const char *const& msg) : std::exception(msg) { ; };
};




/*               ________________________
				/                        \
		* ---==<	     The World        >==--- *
				\________________________/
*/

class World
{
protected:
	typedef std::map<size_t, AgentInEnv_skeleton*> agentMap_t;

protected:
	agentMap_t		agents;

public:
	Enviroment *	enviroment;

public:
	World( ) { ; };
	~World( ) { ; };

	virtual void			update( ) { ; };

	void					addAgent( size_t agentID, AgentInEnv_skeleton *agent );
	void					remAgent( size_t agentID );
	AgentInEnv_skeleton *	getAgent( size_t agentID );
};


void World::addAgent( size_t agentID, AI::Agents::AgentInEnv_skeleton *agent ) throw ( bad_world )
{
	if ( agents.find(agentID) != agents.end() )
		throw bad_world("Agent id already exists");
	agents[agentID] = agent;
}

void World::remAgent( size_t agentID ) throw ( bad_world )
{
	if ( agents.find(agentID) == agents.end() )
		throw bad_world("Agent id not exists");
	agents.erase(agentID);
}

AgentInEnv_skeleton* World::getAgent( size_t agentID ) throw ( bad_world )
{
	agentMap_t::iterator it(agents.find(agentID));
	if ( it == agents.end() )
		throw bad_world("Agent id not exists");
	return it->second;
}


}; /* end of namespace Agents */

}; /* end of namespace AI */
