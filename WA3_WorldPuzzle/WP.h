
#pragma once




#include "../libai/world.h"
#include "../libai/agents.h"
#include "../libai/enviroment.h"


class CrateAgent;


class CrateEnviroment : public AI::Agents::Enviroment
{
friend CrateAgent;

protected:
	size_t	width;
	size_t	height;
	char*	crates;
	size_t	agentX, agentY;

public:
	CrateEnviroment( const AI::Agents::World &world, size_t width, size_t height )
	: AI::Agents::Enviroment(world)
	, width(width)
	, height(height)
	{
		crates = (char*)malloc(width*height);
		memset(crates, 1, width*height);
	};

	CrateEnviroment( const CrateEnviroment &env2 ) 
	: AI::Agents::Enviroment(env2.myWorld)
	, width(env2.width), height(env2.height)
	, agentX(env2.agentX), agentY(env2.agentY)
	{
		crates = (char*)malloc(width*height);
		memcpy(crates, env2.crates, width*height);
	};

	~CrateEnviroment() { free(crates); };

	void	SetInvalid(size_t x, size_t y) { crates[x + y*width] = 0; };
	void	SetValid(size_t x, size_t y) { crates[x + y*width] = 1; };
	bool	IsValid(size_t x, size_t y) { return x < width && y < height && crates[x + y*width] != 0; };
	bool	DefCrate(size_t x, size_t y, char col) { if ( !IsValid(x,y) ) return false; crates[x + y*width] = col * 2; return true; };
	char	UndefCrate(size_t x, size_t y) { if ( !IsValid(x,y) ) return 0; char col = crates[x + y*width]; crates[x + y*width] = 1; return col; };
	bool	HasCrate(size_t x, size_t y) { if ( !IsValid(x,y) ) return false; return crates[x + y*width] > 1; };
	char	GetCrate(size_t x, size_t y) { if ( !HasCrate(x,y) ) return -1; else return crates[x + y*width] / 2; };

	bool	operator==(const AI::Agents::Enviroment *env) const {
				CrateEnviroment &cenv = *((CrateEnviroment*)env);
				if ( width != cenv.width || height != cenv.height )
					return false;
				else
					return !memcmp(crates, cenv.crates, width*height);
			}

	Enviroment*	duplicate() const { return new CrateEnviroment(*this); };

	void	assign(const AI::Agents::Enviroment *env) {
				CrateEnviroment &cenv = *((CrateEnviroment*)env);
				if ( width != cenv.width || height != cenv.height ) {
					free(crates);
					width = cenv.width;
					height = cenv.height;
					crates = (char*)malloc(width*height);
					agentX = cenv.agentX;
					agentY = cenv.agentY;
				}
				memcpy(crates, cenv.crates, width*height);
			}

	void	detachCrate(size_t x, size_t y) {
				if ( !HasCrate(x,y) )
					return;
				else {
					char myCrate = GetCrate(x,y);
					if ( myCrate == GetCrate(x-1,y) ) {
						UndefCrate(x,y);
						detachCrate(x-1,y);
						UndefCrate(x-1,y);
					}
					if ( myCrate == GetCrate(x,y-1) ) {
						UndefCrate(x,y);
						detachCrate(x,y-1);
						UndefCrate(x,y-1);
					}
					if ( myCrate == GetCrate(x+1,y) ) {
						UndefCrate(x,y);
						detachCrate(x+1,y);
						UndefCrate(x+1,y);
					}
					if ( myCrate == GetCrate(x,y+1) ) {
						UndefCrate(x,y);
						detachCrate(x,y+1);
						UndefCrate(x,y+1);
					}
				}
			};

	void	refresh( ) {
				size_t x, y;
				for ( y = 1; y < height - 1; y++ ) {
					for ( x = 1; x < width - 1; detachCrate(x++, y) );
				}
			};
};


class CrateAgent : public AI::Agents::AgentInEnv_skeleton
{
public:
	CrateAgent(CrateEnviroment &env, size_t x = 0, size_t y = 0)
	: AI::Agents::AgentInEnv_skeleton(env)
	{
		ops.push_back("F");
		ops.push_back("L");
		ops.push_back("J");
		ops.push_back("B");

		env.agentX = x;
		env.agentY = y;
	};

	~CrateAgent() { ; };

	void	SetPosition(size_t x, size_t y) {
				CrateEnviroment &cenv = *((CrateEnviroment*)&myEnv);
				cenv.agentX = x;
				cenv.agentY = y;
			};

	void	SetPosition(size_t x, size_t y, CrateEnviroment &cenv) {
				cenv.agentX = x;
				cenv.agentY = y;
			};

	double	getCost(const std::string &op) const { return 1.0; };
	double	getCost(const AI::Agents::Enviroment &env, const std::string &op) const { return 1.0; };

	void	enumOps(const AI::Agents::Enviroment &env, std::vector<std::string> &opnames) const
			{ return AI::Agents::Agent_skeleton::enumOps(opnames); };

	bool	doOp(const std::string &op, double &cost);
	bool	doOp(AI::Agents::Enviroment &env, const std::string &op, double &cost);
};





bool CrateAgent::doOp(const std::string &op, double &cost)
{
	CrateEnviroment &cenv = *((CrateEnviroment*)&myEnv);
	size_t &x = cenv.agentX, &y = cenv.agentY;

	switch ( op[0] )
	{
	case 'F':
		if ( !cenv.IsValid(x, y-1) )
			return false;
		else if ( !cenv.HasCrate(x, y-1) )
			y--;
		else if ( !cenv.IsValid(x, y-2) || cenv.HasCrate(x, y-2) )
			return false;
		else {
			cenv.DefCrate(x, y-2, cenv.UndefCrate(x, y-1));
			y--;
		}
		break;
	case 'L':
		if ( !cenv.IsValid(x, y+1) )
			return false;
		else if ( !cenv.HasCrate(x, y+1) )
			y++;
		else if ( !cenv.IsValid(x, y+2) || cenv.HasCrate(x, y+2) )
			return false;
		else {
			cenv.DefCrate(x, y+2, cenv.UndefCrate(x, y+1));
			y++;
		}
		break;
	case 'B':
		if ( !cenv.IsValid(x-1, y) )
			return false;
		else if ( !cenv.HasCrate(x-1, y) )
			x--;
		else if ( !cenv.IsValid(x-2, y) || cenv.HasCrate(x-2, y) )
			return false;
		else {
			cenv.DefCrate(x-2, y, cenv.UndefCrate(x-1, y));
			x--;
		}
		break;
	case 'J':
		if ( !cenv.IsValid(x+1, y) )
			return false;
		else if ( !cenv.HasCrate(x+1, y) )
			x++;
		else if ( !cenv.IsValid(x+2, y) || cenv.HasCrate(x+2, y) )
			return false;
		else {
			cenv.DefCrate(x+2, y, cenv.UndefCrate(x+1, y));
			x++;
		}
		break;
	}

	cenv.refresh();
	
	return true;
}




bool CrateAgent::doOp(AI::Agents::Enviroment &env, const std::string &op, double &cost)
{
	CrateEnviroment &cenv = *((CrateEnviroment*)&env);
	size_t &x = cenv.agentX, &y = cenv.agentY;

	switch ( op[0] )
	{
	case 'F':
		if ( !cenv.IsValid(x, y-1) )
			return false;
		else if ( !cenv.HasCrate(x, y-1) )
			y--;
		else if ( !cenv.IsValid(x, y-2) || cenv.HasCrate(x, y-2) )
			return false;
		else {
			cenv.DefCrate(x, y-2, cenv.UndefCrate(x, y-1));
			y--;
		}
		break;
	case 'L':
		if ( !cenv.IsValid(x, y+1) )
			return false;
		else if ( !cenv.HasCrate(x, y+1) )
			y++;
		else if ( !cenv.IsValid(x, y+2) || cenv.HasCrate(x, y+2) )
			return false;
		else {
			cenv.DefCrate(x, y+2, cenv.UndefCrate(x, y+1));
			y++;
		}
		break;
	case 'B':
		if ( !cenv.IsValid(x-1, y) )
			return false;
		else if ( !cenv.HasCrate(x-1, y) )
			x--;
		else if ( !cenv.IsValid(x-2, y) || cenv.HasCrate(x-2, y) )
			return false;
		else {
			cenv.DefCrate(x-2, y, cenv.UndefCrate(x-1, y));
			x--;
		}
		break;
	case 'J':
		if ( !cenv.IsValid(x+1, y) )
			return false;
		else if ( !cenv.HasCrate(x+1, y) )
			x++;
		else if ( !cenv.IsValid(x+2, y) || cenv.HasCrate(x+2, y) )
			return false;
		else {
			cenv.DefCrate(x+2, y, cenv.UndefCrate(x+1, y));
			x++;
		}
		break;
	}

	cenv.refresh();
	
	return true;
};



