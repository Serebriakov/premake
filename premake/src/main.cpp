#include "game.h"


//#ifdef _PLATFORM_WIN
//	int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmd, int nShow) 
//#else
	int main(int argc, char *argv[])
//#endif
{
	#if (defined _DEBUG) && (defined _PLATFORM_WIN)
		AllocConsole();
		freopen("CONIN$", "rb", stdin);
		freopen("CONOUT$", "wb", stdout);
		freopen("CONOUT$", "wb", stderr);
	#endif

	Game *game = new Game();
	game->Init();
	game->Run();
	delete game;

	return 0;
}

