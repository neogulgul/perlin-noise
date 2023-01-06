#include "headers/App.hpp"

#define targetFPS    60
#define viewWidth    512
#define viewHeight   512
#define windowWidth  1024
#define windowHeight 1024
#define windowTitle  "hello, world"

int main()
{
	App app(targetFPS, {viewWidth, viewHeight}, {windowWidth, windowHeight}, windowTitle);

	app.run();

	return 0;
}
