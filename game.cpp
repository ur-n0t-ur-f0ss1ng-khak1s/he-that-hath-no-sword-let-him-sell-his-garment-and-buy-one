#include "game.h"
//#include <memory>
#include "level.h"

game::game()
{
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
    throw std::runtime_error("SDL Initialization failed");
  }

  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 480;
  const int SCREEN_BPP = 32;

  SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_OPENGL);
  if (!screen) {
    std::cerr << "SDL SetVideoMode failed: " << SDL_GetError() << std::endl;
    throw std::runtime_error("Failed to set video mode");
  }

  cam.setLocation(vector3d(10,10,10));
  glClearColor(0.5,0.5,0.5,1.0);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, static_cast<double>(SCREEN_WIDTH) / SCREEN_HEIGHT, 1.0, 500.0);

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    std::cerr << "OpenGL Error: " << gluErrorString(err) << std::endl;
    throw std::runtime_error("OpenGL Error");
  }
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_DEPTH_TEST);

  initskybox();

  std::vector<collisionplane> mapcp;
  std::vector<vector3d> mapsp;
  mapsp.push_back(vector3d(3,4,5));

  unsigned int map=obj.load("map.obj",&mapcp);

  if (map == 0) {
    std::cerr << "Failed to load map.obj." << std::endl;
    throw std::runtime_error("Failed to load map.obj");
  }
  levels.push_back(std::make_unique<level>("test-level",map,mapcp,mapsp));
}

game::~game()
{
  SDL_Quit();
  killskybox();
}

void game::start()
{
	SDL_Event event;
	Uint32 startTime;
	bool running=true;
	int menuoption=0;
  std::cout << "started the game" << std::endl;
	while(running)
	{
		startTime=SDL_GetTicks();

		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					running=false;
					std::cout << "quiting..." << std::endl;
					break;
        case SDL_MOUSEBUTTONDOWN:
          cam.mouseIn(true);
          SDL_ShowCursor(SDL_DISABLE);
          break;
				case SDL_KEYDOWN:
          if(event.key.keysym.sym==SDLK_ESCAPE)
          {
            running=false;
            break;
          }
			}
		}
		update();
		show();

		SDL_GL_SwapBuffers();
//		if(1000/FPS>(SDL_GetTicks()-startTime))
//			SDL_Delay(1000/FPS-(SDL_GetTicks()-startTime));
	}
}

void game::update()
{
  for(int i=0;i<levels.size();i++)
    levels[i]->update();
}

void game::show()
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  cam.Control();
  drawSkybox(50.0);
  cam.UpdateCamera();
  for(int i=0;i<levels.size();i++)
    levels[i]->show();
}
