#include <Windows.h>
#include <memory>

#include "MainGame.h"
#include "Framework.h"

// Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	std::unique_ptr<Framework> myGame = std::make_unique<MainGame>();


	myGame->Run();


	return 0;
}