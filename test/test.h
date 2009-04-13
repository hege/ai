

#include "../libai/agents.h"
#include "../libai/enviroment.h"

class TestAgent : public AI::Agents::AgentInEnv_skeleton
{
public:
	TestAgent(AI::Agents::EnvWithElems<int,int> &env) : AgentInEnv_skeleton(env) {
		ops.push_back("fel");
		ops.push_back("le");
	};

	bool	doOp( const std::string &op, double &cost ) {
		int &szam = ((AI::Agents::EnvWithElems<int,int>*)(&myEnv))->getElement(0);
		if ( op == "fel" )
			szam++;
		else if ( op == "le" ) {
			if ( szam > 0 )
				szam--;
			else
				return false;
		} else
			return false;
		cost = getCost(op);
		return true;
	};

	bool	doOp( const AI::Agents::Enviroment &env, const std::string &op, double &cost ) {
		int &szam = ((AI::Agents::EnvWithElems<int,int>*)(&env))->getElement(0);
		if ( op == "fel" )
			szam++;
		else if ( op == "le" ) {
			if ( szam > 0 )
				szam--;
			else
				return false;
		} else
			return false;
		cost = getCost(env, op);
		return true;
	};

	double	getCost( const std::string &op ) const { return 1; };

	double	getCost( const AI::Agents::Enviroment &env, const std::string &op ) const { return 1; };

	void	enumOps(const AI::Agents::Enviroment &env, std::vector<std::string> &opnames) const
			{ return AI::Agents::AgentInEnv_skeleton::enumOps(opnames); };
};

