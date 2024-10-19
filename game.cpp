#include "game.h"
//#include <memory>
#include "level.h"

game::game()
{
  out.open("report.txt");
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
    throw std::runtime_error("SDL Initialization failed");
  }

  const int SCREEN_WIDTH = 1024;
  const int SCREEN_HEIGHT = 768;
  const int SCREEN_BPP = 32;

  SDL_Surface* screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE | SDL_OPENGL);
  if (!screen) {
    std::cerr << "SDL SetVideoMode failed: " << SDL_GetError() << std::endl;
    throw std::runtime_error("Failed to set video mode");
  }

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

  std::vector<collisionplane> mapcp;
  std::vector<vector3d> mapsp;
  mapsp.push_back(vector3d(3,4,5));

  unsigned int map=obj.load("map.obj",&mapcp);

  if (map == 0) {
    std::cerr << "Failed to load map.obj." << std::endl;
    throw std::runtime_error("Failed to load map.obj");
  }

  // it's skybox time bby!
  unsigned int left=loadTexture("data/skybox/left.bmp");
  unsigned int back=loadTexture("data/skybox/back.bmp");
  unsigned int right=loadTexture("data/skybox/right.bmp");
  unsigned int front=loadTexture("data/skybox/front.bmp");
  unsigned int top=loadTexture("data/skybox/top.bmp");
  unsigned int bottom=loadTexture("data/skybox/bottom.bmp");

  levels.push_back(std::make_shared<level>("test-level",map,mapcp,mapsp,left,back,right,front,top,bottom));
  std::vector<unsigned int> anim;
  loadAnimation(anim, "data/weapon1/weapon",38);
  std::cout << "anim size in game(): " << anim.size() << std::endl;
  weapons.push_back(std::make_shared<weapon>(anim,anim[0],1,16,20,vector3d(-1.3,-1.63,6.7),vector3d(0,0,0),vector3d(0,0,0),vector3d(0,0,0),vector3d(0,0,0),100,1000,30,7,300,20,"weapon1",true));
  player1=std::make_unique<player>("player1",collisionsphere(vector3d(0,0,0),2.0),0.5,0.2,0.2,weapons[0]);
  anim.clear();
  loadAnimation(anim,"data/zombie1/zombie",60);
  zombies.push_back(std::make_shared<zombie>(anim,30,20,10,100,5,0.1,collisionsphere(vector3d(20,20,0),2.0)));
}

game::~game()
{
  SDL_Quit();
}

void game::start()
{
	SDL_Event event;
	Uint32 startTime;
	bool running=true;
	int menuoption=0;
  vector3d direction;
  bool mousebuttondown=false;
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
          player1->cam.mouseIn(true);
          SDL_ShowCursor(SDL_DISABLE);
          if(event.button.button==SDL_BUTTON_LEFT)
          {
            mousebuttondown=true;
            if(player1->getCurrentWeapon()->fire(direction,player1->cam.getVector()))
            {
              for(int i=0;i<zombies.size();i++)
                if(collision::raysphere(
                zombies[i]->getCollisionSphere()->center.x,
                zombies[i]->getCollisionSphere()->center.y,
                zombies[i]->getCollisionSphere()->center.z,
                direction.x,direction.y,direction.z,
                player1->getCollisionSphere().center.x,
                player1->getCollisionSphere().center.y,
                player1->getCollisionSphere().center.z,
                zombies[i]->getCollisionSphere()->r))
                {
                  zombies[i]->decreaseHealth(player1->getCurrentWeapon()->getStrength());
                }
            }
          }else if(event.button.button==SDL_BUTTON_RIGHT)
          {
            player1->getCurrentWeapon()->aim();
          }else if(event.button.button==SDL_BUTTON_WHEELUP)
          {
            player1->changeWeaponUp();
          }else if(event.button.button==SDL_BUTTON_WHEELDOWN)
          {
            player1->changeWeaponDown();
          }
          break;
				case SDL_KEYDOWN:
          switch(event.key.keysym.sym)
          {
            case SDLK_0:
              player1->changeWeapon(0);
              break;
            case SDLK_1:
              player1->changeWeapon(1);
              break;
            case SDLK_2:
              player1->changeWeapon(2);
              break;
            case SDLK_r:
              player1->getCurrentWeapon()->reload();
              break;
            case SDLK_SPACE:
              player1->setJump();
              break;
            case SDLK_LSHIFT:
              player1->setSprint();
              break;
            case SDLK_ESCAPE:
              running=false;
              break;
          }
          break;
        case SDL_KEYUP:
          switch(event.key.keysym.sym)
          {
            case SDLK_LSHIFT:
              player1->stopSprint();
              break;
          }
        case SDL_MOUSEBUTTONUP:
          mousebuttondown=false;
          player1->getCurrentWeapon()->stopfire();
          break;
			}
		}

		update();

		show();

		SDL_GL_SwapBuffers();
    int FPS=60;
		if(1000/FPS>(SDL_GetTicks()-startTime))
			SDL_Delay(1000/FPS-(SDL_GetTicks()-startTime));
	}
}

void game::update()
{
  for(int i=0;i<levels.size();i++)
    levels[i]->update();
  player1->update(levels[0]->getCollisionPlanes());
  for(int i=0;i<zombies.size();i++)
    if(zombies[i]->update(levels[0]->getCollisionPlanes(),player1->getCollisionSphere().center))
    {
      //zombie is dead
    }
  for(int i=0;i<zombies.size();i++)
    if(zombies[i]->setAttack(player1->getCollisionSphere()))
    {
      //the zombie is attacking the player
      player1->setHealth(player1->getHealth()-zombies[i]->getStrength());
    }
}

void game::show()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  player1->getCamera()->control();
  //drawSkybox(500.0);
  levels[0]->getSkybox()->drawSkybox();
  player1->getCamera()->update();
  for(int i=0;i<levels.size();i++)
    levels[i]->show();
  player1->show();
  for(int i=0;i<zombies.size();i++)
    zombies[i]->show();
}

void game::loadAnimation(std::vector<unsigned int>& anim,const std::string filename,int frames)
{
	char frame[8];
	char tmp[7];
  std::cout << "frames: " << frames << std::endl;
  frames = 38;
	for(int i=1;i<=frames;i++)
	{
		std::string s(filename); //string misformatting here will cause a segfault 
    
		sprintf(frame,"%d",i);
		int len=strlen(frame);
		for(int j=0;j<len;j++)
			tmp[j]=frame[j];
		for(int j=0;j<4-len;j++)
			frame[j]='0';
		for(int j=4-len;j<4;j++)
			frame[j]=tmp[j-4+len];
		frame[4]=NULL;
		s+=frame;
		s+=".obj";
		out << s << std::endl;
    int tmpobj = obj.load(s,NULL);
		anim.push_back(tmpobj);
	}
}

unsigned int game::loadTexture(const char* filename)
{
	unsigned int num;	//the id for the texture
	glGenTextures(1,&num);	//we generate a unique one
	SDL_Surface* img=SDL_LoadBMP(filename);	//load the bmp image
	glBindTexture(GL_TEXTURE_2D,num);	//and use the texture, we have just generated
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//if the texture is smaller, than the image, we get the avarege of the pixels next to it
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); //same if the image bigger
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);	//we repeat the pixels in the edge of the texture, it will hide that 1px wide line at the edge of the cube, which you have seen in the video
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);	//we do it for vertically and horizontally (previous line)
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,img->w,img->h,0,GL_RGB,GL_UNSIGNED_SHORT_5_6_5,img->pixels);	//we make the actual texture
	SDL_FreeSurface(img);	//we delete the image, we don't need it anymore
	return num;	//and we return the id
}
