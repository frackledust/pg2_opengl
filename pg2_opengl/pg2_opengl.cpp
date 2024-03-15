#include "pch.h"
#include "tutorials.h"
#include "Application.h"

int main()
{
	//return tutorial_1();
	//return tutorial_2();
	//return tutorial_3( "../../../data/environments/abandoned_hopper_terminal_04_1k.jpg" );
	//return tutorial_4( "../../../data/environments/abandoned_hopper_terminal_04_1k.exr" );
	//return tutorial_5();
	//return tutorial_6();
	//return tutorial_7( "../../../data/adjacent_triangles.obj" );
	//return tutorial_8();
	auto app = new Application();
	app->run();
	//generate_irradiance_map("../../../data/maps/lebombo_prefiltered_env_map_001_2048.exr");
}
