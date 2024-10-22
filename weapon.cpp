#include "weapon.h"

weapon::weapon(std::vector<unsigned int>& anim,unsigned int ol,unsigned int na,unsigned int fa,unsigned int ra,vector3d ofset,vector3d pos,vector3d rot,vector3d apos,vector3d arot,float prec,float aprec,int str,int maxb,int allbul,int speedd,const char* namee,bool isa)
{
  frames=anim;
  outerview=ol;
  normalanimation=na;
  reloadanimation=ra;
  fireanimation=fa;
  precision=(prec!=0 ? prec : 0.00001);
  aimprecision=(aprec!=0 ? aprec : 0.00001);

  yaw=0.0;
  pitch=0.0;

  offset=ofset;

  position=pos;
  rotation=rot;

  aimposition=apos;
  aimrotation=arot;

  strength=str;
  allbullets=allbul;
  maxBulletsInMag=maxb;
  speed=speedd;
  name=namee;

  position_expected=position;
  rotation_expected=rotation;

  curpos=position;
  currot=rotation;

  isaim=false;
  isreloading=false;
  isautomatic=isa;
  isfired=false;
  istest=true;
  bulletsInMag=maxBulletsInMag;
  
  lastshot=1000;

  curframe=0;
  curmode=1;
}

weapon::~weapon()
{

}

void weapon::update()
{
  lastshot++;
  curframe++;
  if(curmode==1) //normal
  {
    if (curframe>=normalanimation)
      curframe=0;
  }else if(curmode==2)
  {
    if(curframe>normalanimation+fireanimation)
    {
      if(isautomatic && isfired) //may be a BUG
      {
        curframe=normalanimation;
      }else{
        curmode=1;
        curframe=0;
      }
    }
  }else if(curmode==3)
  {
    if(curframe>=normalanimation+fireanimation+reloadanimation-1)
    {
      curmode=1;
      curframe=0;
      isreloading=false;
    }
  }

  vector3d forwardVector = currot; 
  vector3d rightVector(forwardVector.z, 0, -forwardVector.x);
  rightVector.normalize();
  curpos = curpos + forwardVector * offset.z //offset forward
                  + rightVector * offset.x //offset to the right
                  + vector3d(0,1,0) * offset.y; //offset slight down

//  vector3d tmpVec(position_expected-curpos);
//  tmpVec.normalize();
//  tmpVec*=0.3;
//  curpos+=tmpVec;
//  if(std::abs(position_expected.x-curpos.x)<0.3 && std::abs(position_expected.y-curpos.y)<0.3 && std::abs(position_expected.z-curpos.z)<0.3)
//    curpos=position_expected;
//
//  tmpVec.change(rotation_expected-currot);
//  tmpVec.normalize();
//  tmpVec*=0.3;
//  currot+=tmpVec;
//  if(std::abs(rotation_expected.x-currot.x)<0.3 && std::abs(rotation_expected.y-currot.y)<0.3 && std::abs(rotation_expected.z-currot.z)<0.3)
//    currot=rotation_expected;

  //std::cout << "offset: " << offset << "pitch: " << pitch << "yaw: " << yaw << std::endl;
}

void weapon::show()
{
  test();
  glPushMatrix();
    glTranslatef(curpos.x,curpos.y,curpos.z);
    glRotatef(yaw-86.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(-pitch, 0.0f, 0.0f, 1.0f);
    if(curframe >= 0 && curframe < frames.size()) {
      glCallList(frames[curframe]);
    } else {
      std::cerr << "curframe is out of bounds!" << std::endl;
    }
  glPopMatrix();
}

bool weapon::fire(vector3d& direction,vector3d camdirection)
{
  if(isreloading)
    return false;
  if((!isautomatic && !isfired || isautomatic))
  {
    if(lastshot>=speed)
    {
      if(bulletsInMag>0)
      {
        if(isaim)
        {
          direction.x=camdirection.x+((float)(rand()%3-1)/aimprecision);
          direction.y=camdirection.y+((float)(rand()%3-1)/aimprecision);
          direction.z=camdirection.z+((float)(rand()%3-1)/aimprecision);
        }else{
           direction.x=camdirection.x+((float)(rand()%3-1)/precision);
          direction.y=camdirection.y+((float)(rand()%3-1)/precision);
          direction.z=camdirection.z+((float)(rand()%3-1)/precision);
        }
        isfired=true;
        lastshot=0;
        bulletsInMag--;
        curframe=normalanimation;
        curmode=2;
        return true;
      }else{
        reload();
        return false;
      }
    }
  }
  return 0;
}

void weapon::stopfire()
{
  isfired=false;
}

void weapon::reload()
{
  if(!isreloading && maxBulletsInMag!=bulletsInMag)
  {
    isreloading=true;
    if(allbullets>maxBulletsInMag-bulletsInMag)
    {
      allbullets-=maxBulletsInMag-bulletsInMag;
      bulletsInMag=maxBulletsInMag;
    }else{
      bulletsInMag=allbullets+bulletsInMag;
      allbullets=0;
    }
    curframe=normalanimation+fireanimation;
    curmode=3;
  }
}

void weapon::aim()
{
  isaim=!isaim;
  if(isaim)
  {
    offset.y+=1;
    offset.x+=1;
    rotation_expected+=aimrotation;
    position_expected+=aimposition;
  }else
  {
    offset.y-=1;
    offset.x-=1;
    rotation_expected+=rotation;
    position_expected+=position;
  }
}

void weapon::test()
{
  if(istest)
  {
    const Uint8* keys=SDL_GetKeyboardState(NULL);
    if(keys[SDLK_j])
      offset.x-=0.2;
    if(keys[SDLK_k])
      offset.x+=0.2;
    if(keys[SDLK_l])
      offset.z-=0.2;
    if(keys[SDLK_o])
      offset.z+=0.2;
    if(keys[SDLK_u])
      offset.y-=0.01;
    if(keys[SDLK_i])
      offset.y+=0.01;
  }
}

void weapon::addBullets(unsigned int num)
{
  allbullets+=num;
}

void weapon::setBullets(unsigned int num)
{
  allbullets=num;
}

int weapon::getStrength()
{
  return strength;
}

int weapon::getAmmo()
{
  return bulletsInMag;
}

int weapon::getAllAmmo()
{
  return allbullets;
}

std::string weapon::getName()
{
  return name;
}

std::vector<unsigned int>& weapon::getAnimation()
{
  return frames;
}

bool weapon::isAimed()
{
  return isaim;
}

unsigned int weapon::getOuterView()
{
  return outerview;
}

void weapon::setCurpos(vector3d newpos)
{
  curpos = newpos;
}

void weapon::setCurrot(vector3d newrot)
{
  currot = newrot;
}

void weapon::setPitchAndYaw(float newpitch, float newyaw)
{
  pitch=newpitch;
  yaw=newyaw;
}

