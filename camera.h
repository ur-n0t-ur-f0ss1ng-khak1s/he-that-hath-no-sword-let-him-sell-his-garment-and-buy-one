#ifndef CAMERA_H
#define CAMERA_H
#include "vector3d.h"
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

class camera{
  vector3d loc;
  float camPitch,camYaw;
  float movevel;
  float mousevel;
  bool mi,ismoved;
  void lockCamera();
  void moveCamera(float dir);
  void moveCameraUp(float dir);
  public:
    camera();
    camera(vector3d loc);
    camera(vector3d loc,float yaw,float pitch);
    camera(vector3d loc,float yaw,float pitch,float mv,float mov);
    void control(SDL_Window* win);
    void update();
    vector3d getVector();
    vector3d getLocation();
    float getCamPitch();
    float getCamYaw();
    float getMovevel();
    float getMousevel();
    bool isMouseIn();

    void setLocation(vector3d vec);
    void lookAt(float pitch,float yaw);
    void mouseIn(bool b);
    void setSpeed(float mv,float mov);

    bool isMoved();
};

#endif
