#ifndef GAME_H
#define GAME_H
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <memory>
#include "objloader.h"
#include "vector3d.h"
#include "collisionplane.h"
#include "collisionsphere.h"
#include "functions.h"
#include "level.h"
#include "player.h"
#include "weapon.h"

class game{
  objloader obj;
  camera cam;
  std::vector<std::unique_ptr<level>> levels;
  std::vector<std::unique_ptr<weapon>> weapons;
  player* player1;
  std::ofstream out; 
  void update();
  void show();
  unsigned int loadTexture(const char* filename);
  void loadAnimation(std::vector<unsigned int>& anim,std::string filename,int frames);
//  void loadAnimation(std::vector<unsigned int>& frames,std::string filename,unsigned int num);

  public:
    game();
    ~game();
    void start();
};

#endif
