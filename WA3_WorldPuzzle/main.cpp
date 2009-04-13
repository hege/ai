
#if defined _MSC_VER && ( _MSC_VER == 1300 || _MSC_VER == 1310 )	// VC++ 7.0 & 7.1
#pragma warning ( disable : 4290 )
#endif



#include "WP.h"
#include "../libai/agents.h"
#include "../libai/search.h"


void waitChar()
{
	printf("Press any key to exit...\n\n");
	getchar();
}

int main(int argc, char *argv[])
{
	atexit(waitChar);



	FILE *f = fopen("map.txt", "r");
	if ( f == NULL ) {
		printf("Cant open map file!\n");
		return -1;
	}

	int w, h, x ,y;
	fscanf(f, "%d %d\n", &w, &h);

	AI::Agents::World wo;
	CrateEnviroment env(wo, w, h);
	CrateEnviroment env2(wo, w, h);
	CrateAgent ag(env);

	char c, cnt = 0;
	for ( y = 0; y < h; y++ ) {
		for ( x = 0; x < w; x++ ) {
			c = fgetc(f);
			switch ( c ) {
				case '.':
					break;
				case ' ':
					env.SetInvalid(x,y);
					env2.SetInvalid(x,y);
					break;
				case 'A':
					ag.SetPosition(x,y,env);
					break;
				default:
					env.DefCrate(x,y,c);
					break;
			}
		}
	}

	AI::Search::BreadthFirstSearch se;
	std::vector<AI::Search::sStep> steps;
	se.find(ag, env2, steps);

	for ( size_t i = 0; i < steps.size(); putchar(steps[i++].opName[0]) );
}

