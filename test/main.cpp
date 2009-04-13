
#if defined _MSC_VER && ( _MSC_VER == 1300 || _MSC_VER == 1310 )	// VC++ 7.0 & 7.1
#pragma warning ( disable : 4290 )
#endif


#include "test.h"

#include "../libai/search.h"
#include <string>



int main( int argc, char* argv[] )
{
	std::vector<AI::Search::sStep> steps;

	AI::Agents::World wo;

	double cost;

	AI::Agents::EnvWithElems<int, int> env(wo);
	AI::Agents::EnvWithElems<int, int> env2(wo);
	env.addElement(5, 0);
	env2.addElement(2, 0);

	TestAgent ta(env);
	wo.addAgent(0, &ta);

	AI::Search::IterativeDeepeningSearch se(1, 1);
	se.find(ta, env2, steps);

	printf(" No.\t State\t Change\t  Cost\t TotalCost\n");
	printf("-----\t-------\t-------- ------\t-----------\n");
	for ( size_t i = 0; i < steps.size(); i++ )
	{
		env2 = env;
		ta.doOp(steps[i].opName, cost);
		printf(" %2d.)\t %s:\t %d -> %d\t  %.2g\t %.2g\n", i
												 , steps[i].opName.c_str()
												 , env2.getElement(0)
												 , env.getElement(0)
												 , steps[i].opCost
												 , steps[i].totalCost);
	}

	printf("\nPress any key to exit...");
	getchar();

	return 0;
}