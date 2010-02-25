
#pragma once

#include <string>
#include <vector>
#include <list>
#include <set>

#include "agents.h"
#include "enviroment.h"


namespace AI {
namespace Search {


class Search_general;

struct sStep {
	std::string	opName;
	double		opCost;
	double		totalCost;

	sStep(std::string opName, double opCost, double totalCost)
		: opName(opName), opCost(opCost), totalCost(totalCost) { ; };

	sStep(const sStep &rhs) : opName(rhs.opName), opCost(rhs.opCost), totalCost(rhs.totalCost) { ; };

	sStep &operator= (const sStep &rhs) { opName = rhs.opName; opCost = rhs.opCost; totalCost = rhs.totalCost; return *this; };
};
	
/*               ________________________
				/                        \
		* ---==<     General searching    >==--- *
				\________________________/
*/


class Search_general
{
protected:
	typedef	Agents::Enviroment state_t;

	class Node
	{
	friend class Search_general;
	protected:
		state_t				*pState;
		Node				*pParent;
		sStep				step;
		std::set<Node*>		children;
		size_t				depth;

	public:
		Node(Agents::Enviroment *envState, Node *parent, const std::string &crtOp, double cost )
			: pState(envState), pParent(parent), step(crtOp, cost, parent ? cost + parent->step.totalCost : cost), depth(parent ? parent->depth + 1 : 0)
			{ if ( parent ) parent->children.insert(this); };
		Node(const Node &rhs) : pState(rhs.pState->duplicate()), pParent(rhs.pParent), step(rhs.step), children(rhs.children), depth(rhs.depth) { ; };
		~Node() {
			for ( std::set<Node*>::iterator it = children.begin(); it != children.end(); delete *it++ );
			if ( pParent ) pParent->children.erase(pParent->children.find(this));
			delete pState;
		};

		Node & operator= (const Node &rhs) { pState->assign(*rhs.pState); pParent = rhs.pParent; step = rhs.step; children = rhs.children; depth = rhs.depth; return *this; };

		const sStep& getStep() const { return step; };
		const state_t& getState() const { return *pState; };
	};

protected:
	std::vector<state_t*>	exStates;
	std::list<Node*>		nodeQueue;

public:
	Search_general( ) { ; };
	virtual ~Search_general( ) { ; };

	virtual	bool	find( Agents::AgentInEnv_skeleton &agent
//						, const _State &initState
						, const Agents::Enviroment &stopState
						, std::vector<sStep> &steps );

protected:
	std::set<Node*>	expand( Agents::AgentInEnv_skeleton &agent
						  , Node *pNode
						  , std::set<Node*> &newNodes );

	virtual	bool	testNode(Node *pNode) const {
		size_t pos, num = exStates.size();
		for ( pos = 0; pos < num; pos++ )
			if ( *exStates[pos] == pNode->getState() ) return false;
		return true;
	};

	virtual void	putInQueue( std::set<Node*> &nodes ) = 0;
};

bool AI::Search::Search_general::find( AI::Agents::AgentInEnv_skeleton &agent
//									 , const _State &initState
									 , const Agents::Enviroment &stopState
									 , std::vector<sStep> &steps )
{
	std::set<Node*> nodes;
	Node *pSpot, *pRoot(new Node(agent.getEnv().duplicate(), NULL, std::string("_init"), 0));

	exStates.push_back( pRoot->getState().duplicate() );
	nodeQueue.push_back(pRoot);
	while ( !nodeQueue.empty() )
	{
		pSpot = nodeQueue.front();
		nodeQueue.pop_front();
		if ( *pSpot->pState == stopState )
		{
			steps.clear();
			while ( pSpot )
			{
				steps.insert(steps.begin(), pSpot->step);
				pSpot = pSpot->pParent;
			}
			nodeQueue.clear();
			while ( !exStates.empty() ) {
				delete *(exStates.end() - 1);
				exStates.pop_back();
			}
			delete pRoot;
			return true;
		}
		else
		{
			nodes.clear();
			putInQueue( expand(agent, pSpot, nodes) );
		}
	}
	return false;
}

std::set<AI::Search::Search_general::Node*>
	AI::Search::Search_general::expand( AI::Agents::AgentInEnv_skeleton &agent
									  , AI::Search::Search_general::Node *pNode
									  , std::set<AI::Search::Search_general::Node*> &newNodes )
{
	std::vector<std::string> ops;
	agent.enumOps(ops);
	size_t opPos, opNum = ops.size();
	double cost;
	Node *pNewNode;
	state_t *newEnv;

	for ( opPos = 0; opPos < opNum; opPos++ )
	{
		newEnv = pNode->pState->duplicate();
		if ( agent.doOp(*newEnv, ops[opPos], cost) ) {
			if ( testNode(pNewNode = new Node(newEnv, pNode, ops[opPos], cost)) ) {
				newNodes.insert( pNewNode );
				exStates.push_back(newEnv->duplicate());
			} else
				delete pNewNode;
		} else
			delete newEnv;
	}
	return newNodes;
}




/*               ________________________
				/                        \
		* ---==<   Breadth-First Search   >==--- *
				\________________________/
*/


class BreadthFirstSearch : public Search_general
{
public:
	BreadthFirstSearch( ) { ; };
	~BreadthFirstSearch( ) { ; };

protected:
	virtual void	putInQueue( std::set<Node*> &nodes ) {
		for ( std::set<Node*>::iterator it = nodes.begin(); it != nodes.end(); nodeQueue.push_back(*it++) );
	};
};





/*               ________________________
				/                        \
		* ---==<    Uniform Cost Search   >==--- *
				\________________________/
*/


class UniformConstSearch : public Search_general
{
public:
	UniformConstSearch( ) { ; };
	~UniformConstSearch( ) { ; };

protected:
	virtual void	putInQueue( std::set<Node*> &nodes ) {
		for ( std::set<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++ ) {
			std::list<Node*>::iterator lit = nodeQueue.begin();
			for ( ; lit != nodeQueue.end() && (*lit)->getStep().totalCost < (*it)->getStep().totalCost; lit++ );
			nodeQueue.insert(lit, *it);
		}
	};
};





/*               ________________________
				/                        \
		* ---==<    Depth-First Search    >==--- *
				\________________________/
*/


class DepthFirstSearch : public Search_general
{
public:
	DepthFirstSearch( ) { ; };
	~DepthFirstSearch( ) { ; };

protected:
	virtual void	putInQueue( std::set<Node*> &nodes )
		{ for ( std::set<Node*>::iterator it = nodes.begin(); it != nodes.end(); nodeQueue.push_front(*it++) ); };
};





/*               ________________________
				/                        \
		* ---==<   Depth-Limited Search   >==--- *
				\________________________/
*/


class DepthLimitedSearch : public DepthFirstSearch
{
protected:
	double	costLimit;

public:
	DepthLimitedSearch( double initLimit = 1.0 ) : costLimit(initLimit) { ; };
	~DepthLimitedSearch( ) { ; };

	void	setCostLimit( double limit ) { costLimit = limit; };
	double	getCostLimit( ) const { return costLimit; };

protected:
	virtual	bool testNode(Node *pNode) const { return Search_general::testNode(pNode) && pNode->getStep().totalCost <= costLimit; };
};




/*               __________________________
				/                          \
		* ---==< Iterative Deepening Search >==--- *
				\__________________________/
*/


class IterativeDeepeningSearch : public DepthLimitedSearch
{
protected:
	double	step;

public:
	IterativeDeepeningSearch( double initLimit = 1.0, double step = 1.0 )
		: DepthLimitedSearch(initLimit)
		, step(step)
		{ ; };

	~IterativeDeepeningSearch( ) { ; };

	void	setCostLimit( double limit ) { costLimit = limit; };
	double	getCostLimit( ) const { return costLimit; };
	void	setStep( double step ) { this->step = step; };
	double	getStep( ) const { return step; };

protected:
	virtual	bool testNode(Node *pNode) const { return pNode->getStep().totalCost <= costLimit; };

public:
	virtual bool find(Agents::AgentInEnv_skeleton &agent
					 , const Agents::Enviroment &stopState
					 , std::vector<sStep> &steps ) {
						 while ( !DepthLimitedSearch::find(agent, stopState, steps) )
							 costLimit += step;
						 return true;
					 }
};



}; /* end of namespace Search */
}; /* end of namespace AI */