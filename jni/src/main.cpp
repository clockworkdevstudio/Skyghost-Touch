/*

Copyright (c) 2016-2017, Clockwork Development Studio
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "SDL_opengles2.h"
#include "android/log.h"
#include <list>
#include "math.h"
#include "main.h"
#include "multimedia.h"
#include "physics.h"
#include "gradient_text.h"
#include "jni.h"

extern int BB_GRAPHICS_WIDTH;
extern int BB_GRAPHICS_HEIGHT;
extern SDL_Window *WINDOW;
extern SDL_GLContext GL_CONTEXT;
extern double BB_PI;
extern
  std::list <
Entity2D * >
  ENTITY_LIST;
extern float
  LINE_INTERSECTION_S,
  LINE_INTERSECTION_T;
extern float
  LINE_INTERSECTION_X,
  LINE_INTERSECTION_Y;
std::list < Asteroid * >ASTEROID_LIST;
std::list < Particle * >PARTICLE_LIST;
std::list < Bullet * >BULLET_LIST;
std::list < Explosion * >EXPLOSION_LIST;

int
  GAME_STATE;
enum
{
  GS_FRONT_END,
  GS_ABOUT,
  GS_SETTINGS,
  GS_PLAYING,
  GS_GET_READY,
  GS_SECTOR_CLEARED,
  GS_GAME_OVER,
  GS_PAUSED,
  GS_QUIT
};

int
  MSG_FRAME;
int
  MSG_TIME;

const char *
  UNSELECTED_OPTIONS[] = {
  "PLAY",
  "SETTINGS",
  "ABOUT",
  "QUIT"
};

const char *
  SELECTED_OPTIONS[] = {
  "[PLAY]",
  "[SETTINGS]",
  "[ABOUT]",
  "[QUIT]"
};

int
  OPTIONS[4];

double
  DENSITY;

int
  FIRE_BUTTON_X;
int
  FIRE_BUTTON_Y;
int
  FIRE_BUTTON_SIZE;
int
  CONTROLS_X;
int
  CONTROLS_Y;
int
  CONTROLS_RADIUS;
int
  FIRE_BUTTON_ACTIVE;
int
  CONTROLS_ACTIVE;
int
  PAUSE_X1;
int
  PAUSE_Y1;
int
  PAUSE_X2;
int
  PAUSE_Y2;
int
  QUIT_X1;
int
  QUIT_Y1;
int
  QUIT_X2;
int
  QUIT_Y2;
int
  POLY_COUNT;
unsigned int
  GAME_TIMER;
double
  TIMER_PROGRESS;
unsigned int
  CURRENT_TIME;
int
  PLAYER_SCORE;
int
  PLAYER_LIVES;
Craft *
  CRAFT;
double
  CRAFT_SHIELDS;
int
  CRAFT_SIZE;
double
  CRAFT_ANGULAR_VELOCITY;
double
  CRAFT_ACCELERATION;
int
  CRAFT_INVINCIBILITY_TIME;
double
  CRAFT_MAX_SPEED;
int
  CRAFT_THRUSTING;
int
  CRAFT_REVERSE_THRUSTING;
int
  CRAFT_ROTATING_CLOCKWISE;
int
  CRAFT_ROTATING_ANTICLOCKWISE;
int
  CRAFT_COLLISION;
Polygon2D *
  TRANSFORMATION_POLY1;
Polygon2D *
  TRANSFORMATION_POLY2;
int
  ASTEROID_MIN_SIZE;
int
  ASTEROID_MAX_SIZE;
double
  ASTEROID_MAX_SPEED;
double
  ASTEROID_MAX_ANGULAR_VELOCITY;
double
  ASTEROID_MIN_MASS;
int
  ASTEROID_COUNT;
double
  PARTICLE_MAX_VELOCITY;
double
  PARTICLE_LIFESPAN;
int
  BULLET_SIZE;
double
  BULLET_VELOCITY;
int
  BULLET_LIFETIME;
int
  LAST_BULLET_TIME;
int
  BULLET_DELAY;
int
  EXPLOSION_SIZE;
int
  EXPLOSION_LIFESPAN;
int
  SECTOR;
unsigned int
  HEALTH_BAR;
unsigned int
  SND_EXPLODE;
unsigned int
  SND_CRASH;
unsigned int
  SND_SHOOT;
unsigned int
  SND_THRUST;
unsigned int
  CHN_THRUST;
unsigned int
  THRUST_START_TIME;
unsigned int
  SND_TELEPORTING_ARRIVAL;
unsigned int
  SND_TELEPORTING_DEPARTURE;
int
  FONT_WIDTH;
int
  FONT_HEIGHT;
double
  ORIENTATION_CHANGE;
double
  VELOCITY_CHANGE_X;
double
  VELOCITY_CHANGE_Y;
int
  PAUSE_PRESSED;
int
  QUIT_PRESSED;
int
  FRONT_END_OPTION;

enum
{
  OPTION_PLAY,
  OPTION_SETTINGS,
  OPTION_ABOUT,
  OPTION_QUIT
};

extern
  "C"
{
  double
  bb_sin (double angle);
  double
  bb_cos (double angle);
  double
  ScaleItemToDevice (double item_size);
  void
  DoGame ();
  void
  UpdateGame ();
  void
  DoFrontEnd ();
  void
  DoAbout ();
  void
  DoSettings ();
  void
  DrawTriangle (int x, int y, float height, float angle);
  void
  DrawControls (int x, int y);
  void
  DrawFireButton (int x, int y);
  void
  CreateUniverse (int sector);
  void
  DestroyUniverse ();
  void
  DrawDisplay ();
  void
  CreateCraft ();
  void
  UpdateCraft ();
  void
  DrawCraft ();
  void
  DestroyCraft ();
  void
  CreateAsteroid (double x, double y, double r, double direction);
  void
  UpdateAsteroids ();
  void
  DrawAsteroids ();
  void
  CreateParticle (double x, double y, double a, double r, double g, double b);
  void
  UpdateParticles ();
  void
  DrawParticles ();
  void
  DestroyAsteroid (Asteroid * a);
  void
  CreateExplosion (double x, double y);
  void
  UpdateExplosions ();
  void
  DrawExplosions ();
  void
  DrawPolygon2D (Polygon2D * p, float x, float y, float a);
  void
  DestroyPolygon2D (Polygon2D * p);
  void
  CreateBullet (double x, double y, double a);
  void
  UpdateBullets ();
  void
  DrawBullets ();
  void
  BulletCollisions ();
  void
  DrawOptions ();
  void
  DrawAbout ();
  void
  DrawSettings ();
  void
  getDensity ();

  Polygon2D *
  CreatePolygon2D (int num_vertices);
  double
  AWrap (double a);

  void
  InitControls ()
  {
    CONTROLS_X = BB_GRAPHICS_WIDTH / 4 * 3;
    CONTROLS_Y = BB_GRAPHICS_HEIGHT / 6 * 4.5;
    CONTROLS_RADIUS = ScaleItemToDevice (20);
    CONTROLS_ACTIVE = 0;
  } void
  InitFireButton ()
  {
    FIRE_BUTTON_X = BB_GRAPHICS_WIDTH / 4;
    FIRE_BUTTON_Y = BB_GRAPHICS_HEIGHT / 6 * 4.5;
    FIRE_BUTTON_SIZE = ScaleItemToDevice (32);
    FIRE_BUTTON_ACTIVE = 0;
  }

  void
  InitPlayer ()
  {
    CRAFT_SHIELDS = 1.0;
    PLAYER_SCORE = 0;
    CRAFT = 0;
    CRAFT_ANGULAR_VELOCITY = 2;
    CRAFT_MAX_SPEED = ScaleItemToDevice (12);
    CRAFT_ACCELERATION = ScaleItemToDevice (340);
    CRAFT_SIZE = ScaleItemToDevice (50);
    CRAFT_COLLISION = 0;
    CRAFT_THRUSTING = 0;
    CRAFT_REVERSE_THRUSTING = 0;
    CRAFT_ROTATING_CLOCKWISE = 0;
    CRAFT_ROTATING_ANTICLOCKWISE = 0;
    CRAFT_INVINCIBILITY_TIME = 5000;
    SECTOR = 1;
    VELOCITY_CHANGE_X = 0.0;
    VELOCITY_CHANGE_Y = 0.0;
    ORIENTATION_CHANGE = 0.0;
  }

  void
  InitTiming ()
  {
    GAME_TIMER = bb_createtimer (60);
    TIMER_PROGRESS = 0.0;
    CURRENT_TIME = CURRENT_TIME;
  }

  void
  InitAsteroids ()
  {
    ASTEROID_COUNT = 0;
    ASTEROID_MIN_SIZE = ScaleItemToDevice (140);
    ASTEROID_MAX_SIZE = ScaleItemToDevice (20);
    ASTEROID_MAX_SPEED = ScaleItemToDevice (12);
    ASTEROID_MAX_ANGULAR_VELOCITY = 10.0;
    ASTEROID_MIN_MASS = ((BB_GRAPHICS_WIDTH + BB_GRAPHICS_HEIGHT) / 2) * 5;
  }

  void
  InitParticles ()
  {
    PARTICLE_LIFESPAN = 2000;
    PARTICLE_MAX_VELOCITY = 480.0 * DENSITY;
  }

  void
  InitBullets ()
  {
    BULLET_SIZE = ScaleItemToDevice (400);
    BULLET_VELOCITY = ScaleItemToDevice (2.88);
    BULLET_LIFETIME = ScaleItemToDevice(1.7);
    LAST_BULLET_TIME = 0;
    BULLET_DELAY = 500;
  }

  void
  InitExplosions ()
  {
    EXPLOSION_SIZE = ScaleItemToDevice (4);
    EXPLOSION_LIFESPAN = 1000;
  }

  void
  InitResources ()
  {
    FONT_HEIGHT = ScaleItemToDevice (50);
    unsigned int
      Ubuntu = bb_loadfont ("UbuntuMono-B.ttf", FONT_HEIGHT, 0, 0, 0);

    bb_setfont (Ubuntu);
    FONT_WIDTH = bb_stringwidth ("A");

    HEALTH_BAR = bb_loadimage ("health_bar.png");

    SND_EXPLODE = bb_loadsound ("explode.ogg");
    SND_CRASH = bb_loadsound ("collide.ogg");
    SND_SHOOT = bb_loadsound ("shoot.ogg");
    SND_THRUST = bb_loadsound ("thrust.ogg");
    THRUST_START_TIME = 0;
    SND_TELEPORTING_ARRIVAL = bb_loadsound ("teleporting_arrival.ogg");
    SND_TELEPORTING_DEPARTURE = bb_loadsound ("teleporting_departure.ogg");

  }

  void
  InitText ()
  {
    SetGradientTextColor (0, 0, 255, 0, 255, 255);
    SetGradientTextSpeed (1.0);
    SetGradientTextDirection (0);
    SyncGradientText ();
  }

  void
  InitQuitAndPauseCoords ()
  {
    PAUSE_X1 = bb_stringwidth ("SKYGHOST ");
    PAUSE_Y1 = 0;
    PAUSE_X2 = bb_stringwidth ("SKYGHOST [P]");
    PAUSE_Y2 = FONT_HEIGHT;

    QUIT_X1 = bb_stringwidth ("SKYGHOST [P] ");
    QUIT_Y1 = 0;
    QUIT_X2 = bb_stringwidth ("SKYGHOST [P] [Q]");
    QUIT_Y2 = FONT_HEIGHT;
  }

  int
  SDL_main (int argc, char **argv)
  {
    init_multimedia ();
    getDensity ();
    InitControls ();
    InitFireButton ();
    InitPlayer ();
    InitTiming ();
    InitAsteroids ();
    InitParticles ();
    InitBullets ();
    InitExplosions ();
    InitResources ();
    InitText ();
    InitQuitAndPauseCoords ();

    srand (SDL_GetTicks());
    memset (&OPTIONS, 0, 4 * sizeof (int));

    TRANSFORMATION_POLY1 = CreatePolygon2D (20);
    TRANSFORMATION_POLY2 = CreatePolygon2D (20);

    GAME_STATE = GS_FRONT_END;

    PAUSE_PRESSED = 0;
    QUIT_PRESSED = 0;

    MSG_FRAME = 0;
    MSG_TIME = 240;

    while (1)
      {

        switch (GAME_STATE)
          {
          case GS_FRONT_END:
            {
              DoFrontEnd ();
              break;
            }
          case GS_ABOUT:
            {
              DoAbout ();
              break;
            }
          case GS_SETTINGS:
            {
              DoSettings ();
              break;
            }
          case GS_PLAYING:
            {
              DoGame ();
              break;
            }
          case GS_QUIT:
            {
              TTF_Quit ();
              IMG_Quit ();
              Mix_CloseAudio ();
              Mix_Quit ();
              SDL_Quit ();
              return 0;
            }
          }
      }

    return 0;

  }

  void
  getDensity ()
  {
    JNIEnv *
      env = (JNIEnv *) SDL_AndroidGetJNIEnv ();
    jobject
      activity = (jobject) SDL_AndroidGetActivity ();
    jclass
    class_ (env->GetObjectClass (activity));
    jmethodID
      method_id = env->GetMethodID (class_, "getDensity", "()F");
    DENSITY = env->CallFloatMethod (activity, method_id);
    env->DeleteLocalRef (activity);
    env->DeleteLocalRef (class_);
  }

  void
  DoGame ()
  {

    int
      i,
      j,
      k;
    int
      done = 0;
    SDL_Event
      event;

    SECTOR = 1;
    CreateUniverse (SECTOR);
    CreateCraft ();
    CRAFT_SHIELDS = 1.0;
    PLAYER_LIVES = 2;
    PLAYER_SCORE = 0;
    bb_playsound (SND_TELEPORTING_ARRIVAL);
    
    while (1)
      {
	CURRENT_TIME = SDL_GetTicks();
        while (SDL_PollEvent (&event))
          {

            switch (event.type)
              {
              case SDL_QUIT:
                {
                  DestroyCraft ();
                  DestroyUniverse ();

                  GAME_STATE = GS_QUIT;
                  return;
                }
              case SDL_WINDOWEVENT:
                {
                  if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    {
                      SDL_GL_MakeCurrent (WINDOW, GL_CONTEXT);
                      SDL_GL_GetDrawableSize
                        (WINDOW, &BB_GRAPHICS_WIDTH, &BB_GRAPHICS_HEIGHT);
                      glViewport (0, 0,
                                  BB_GRAPHICS_WIDTH, BB_GRAPHICS_HEIGHT);
                      bb_resumetimer (GAME_TIMER);

                    }
                  break;
                }
              case SDL_FINGERDOWN:
                {

                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  if (pixel_x >= PAUSE_X1
                      && pixel_x <= PAUSE_X2
                      && pixel_y >= PAUSE_Y1 && pixel_y <= PAUSE_Y2)
                    {
                      PAUSE_PRESSED = 1;
                    }

                  if (pixel_x >= QUIT_X1
                      && pixel_x <= QUIT_X2
                      && pixel_y >= QUIT_Y1 && pixel_y <= QUIT_Y2)
                    {
                      QUIT_PRESSED = 1;
                    }

                  int
                    i,
                    k,
                    width;

                  if (sqrt
                      ((pixel_x -
                        CONTROLS_X) * (pixel_x -
                                       CONTROLS_X)
                       + (pixel_y -
                          CONTROLS_Y) * (pixel_y -
                                         CONTROLS_Y))
                      < CONTROLS_RADIUS + ScaleItemToDevice (84))
                    {
                      CONTROLS_ACTIVE = 1;

                      double
                        angle = bb_atan2 (pixel_y - CONTROLS_Y,
                                          pixel_x - CONTROLS_X);

                      if (angle >= -135.0 && angle < -45.0)
                        {
                          if (!CRAFT_THRUSTING)
                            {
                              CRAFT_THRUSTING = 1;
                              CHN_THRUST = bb_playsound (SND_THRUST);
                              THRUST_START_TIME = CURRENT_TIME;
                            }
                        }
                      else
                        {
                          CRAFT_THRUSTING = 0;
                        }

                      if (angle >= 45.0 && angle < 135.0)
                        {
                          CRAFT_REVERSE_THRUSTING = 1;
                        }
                      else
                        {
                          CRAFT_REVERSE_THRUSTING = 0;
                        }

                      if (angle >= -45.0 && angle < 45.0)
                        {
                          CRAFT_ROTATING_CLOCKWISE = 1;
                        }

                      if ((angle >= 135.0
                           && angle < 180.0) || angle < -135.0)
                        {
                          CRAFT_ROTATING_ANTICLOCKWISE = 1;
                        }
			
                      if (angle >= -135.0 && angle < -45.0)
                        {
                          VELOCITY_CHANGE_X
                            -=
                            CRAFT_ACCELERATION
                            * bb_cos (CRAFT->entity->orientation);
                          VELOCITY_CHANGE_Y
                            -=
                            CRAFT_ACCELERATION
                            * bb_sin (CRAFT->entity->orientation);
                        }
                      else if (angle >= -45.0 && angle < 45.0)
                        {
		          
                        }
                      else if (angle >= 45.0 && angle < 135.0)
                        {
                          VELOCITY_CHANGE_X
                            +=
                            CRAFT_ACCELERATION
                            * bb_cos (CRAFT->entity->orientation);
                          VELOCITY_CHANGE_Y
                            +=
                            CRAFT_ACCELERATION
                            * bb_sin (CRAFT->entity->orientation);
                        }
                      else
                        if ((angle >= 135.0
                             && angle < 180.0) || angle < -135.0)
                        {
			  
                        }
                    }

                  else if (sqrt
                           ((pixel_x -
                             FIRE_BUTTON_X) *
                            (pixel_x -
                             FIRE_BUTTON_X) +
                            (pixel_y -
                             FIRE_BUTTON_Y) *
                            (pixel_y - FIRE_BUTTON_Y)) < FIRE_BUTTON_SIZE)
                    {
                      if (!FIRE_BUTTON_ACTIVE)
                        {
                          LAST_BULLET_TIME = CURRENT_TIME - BULLET_LIFETIME;
                        }
                      FIRE_BUTTON_ACTIVE = 1;
                    }
                  else if (FIRE_BUTTON_ACTIVE)
                    {
                      FIRE_BUTTON_ACTIVE = 0;
                    }

                  break;
                }
              case SDL_FINGERUP:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  if (sqrt
                      ((pixel_x -
                        CONTROLS_X) * (pixel_x -
                                       CONTROLS_X)
                       + (pixel_y -
                          CONTROLS_Y) * (pixel_y -
                                         CONTROLS_Y))
                      < CONTROLS_RADIUS + ScaleItemToDevice (84))
                    {
                      if (CRAFT_THRUSTING)
                        bb_stopchannel (CHN_THRUST);
                      CRAFT_THRUSTING = 0;
		      CRAFT_ROTATING_CLOCKWISE = 0;
		      CRAFT_ROTATING_ANTICLOCKWISE = 0;
                      CONTROLS_ACTIVE = 0;
                      ORIENTATION_CHANGE = 0.0;
                      VELOCITY_CHANGE_X = 0.0;
                      VELOCITY_CHANGE_Y = 0.0;
                    }

		  if(CRAFT_REVERSE_THRUSTING)
		    {
		      CRAFT_REVERSE_THRUSTING = 0;
		    }

                  if (sqrt
                      ((pixel_x -
                        FIRE_BUTTON_X) *
                       (pixel_x - FIRE_BUTTON_X) +
                       (pixel_y -
                        FIRE_BUTTON_Y) *
                       (pixel_y - FIRE_BUTTON_Y)) < FIRE_BUTTON_SIZE)
                    {
                      FIRE_BUTTON_ACTIVE = 0;
                    }

                  break;
                }
              case SDL_FINGERMOTION:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  if (sqrt
                      ((pixel_x -
                        CONTROLS_X) * (pixel_x -
                                       CONTROLS_X)
                       + (pixel_y -
                          CONTROLS_Y) * (pixel_y -
                                         CONTROLS_Y))
                      < CONTROLS_RADIUS + ScaleItemToDevice (84))
                    {
                      CONTROLS_ACTIVE = 1;
                      double
                        angle = bb_atan2 (pixel_y - CONTROLS_Y,
                                          pixel_x - CONTROLS_X);

                      if (angle >= -135.0 && angle < -45.0)
                        {
                          if (!CRAFT_THRUSTING)
                            {
                              CRAFT_THRUSTING = 1;
                              CHN_THRUST = bb_playsound (SND_THRUST);
                              THRUST_START_TIME = CURRENT_TIME;
                            }
                        }
                      else
                        {
                          CRAFT_THRUSTING = 0;
                        }

		      if (angle >= 45.0 && angle < 135.0)
                        {
                          CRAFT_REVERSE_THRUSTING = 1;
                        }
                      else
                        {
                          CRAFT_REVERSE_THRUSTING = 0;
                        }

                      if (angle >= -45.0 && angle < 45.0)
                        {
                          CRAFT_ROTATING_CLOCKWISE = 1;
                        }

                      if ((angle >= 135.0
                           && angle < 180.0) || angle < -135.0)
                        {
                          CRAFT_ROTATING_ANTICLOCKWISE = 1;
                        }
		      
                      if (angle >= -135.0 && angle < -45.0)
                        {
                          VELOCITY_CHANGE_X
                            -=
                            CRAFT_ACCELERATION
                            * bb_cos (CRAFT->entity->orientation);
                          VELOCITY_CHANGE_Y
                            -=
                            CRAFT_ACCELERATION
                            * bb_sin (CRAFT->entity->orientation);
                        }
                      else if (angle >= -45.0 && angle < 45.0)
                        {
			  
                        }
                      else if (angle >= 45.0 && angle < 135.0)
                        {
                          VELOCITY_CHANGE_X
                            +=
                            CRAFT_ACCELERATION
                            * bb_cos (CRAFT->entity->orientation);
                          VELOCITY_CHANGE_Y
                            +=
                            CRAFT_ACCELERATION
                            * bb_sin (CRAFT->entity->orientation);
                        }
                      else
                        if ((angle >= 135.0
                             && angle < 180.0) || angle < -135.0)
                        {
			  
                        }
		    }

                  else if (sqrt
                           ((pixel_x -
                             FIRE_BUTTON_X) *
                            (pixel_x -
                             FIRE_BUTTON_X) +
                            (pixel_y -
                             FIRE_BUTTON_Y) *
                            (pixel_y - FIRE_BUTTON_Y)) < FIRE_BUTTON_SIZE)
                    {
                      FIRE_BUTTON_ACTIVE = 1;
                    }
                  else if (FIRE_BUTTON_ACTIVE)
                    {
                      FIRE_BUTTON_ACTIVE = 0;
                    }
                }

              }
	    
	  }

	if(CRAFT_ROTATING_CLOCKWISE)
	{
	    ORIENTATION_CHANGE += CRAFT_ANGULAR_VELOCITY;
	}
	else if(CRAFT_ROTATING_ANTICLOCKWISE)
	{
	    ORIENTATION_CHANGE -= CRAFT_ANGULAR_VELOCITY;
	}

	if(CRAFT_THRUSTING == 0 && CRAFT_REVERSE_THRUSTING == 0)
	  {
	    if(CRAFT->entity->velocity->x != 0.0 || CRAFT->entity->velocity->y != 0.0)
	      {
		float
		  speed =
		  sqrt (CRAFT->entity->velocity->x *
			CRAFT->entity->velocity->x +
			CRAFT->entity->velocity->y * CRAFT->entity->velocity->y);

		if(speed <= CRAFT_ACCELERATION)
		  {
		    CRAFT->entity->velocity->x = 0.0;
		    CRAFT->entity->velocity->y = 0.0;
		  }
		else
		  {
		    CRAFT->entity->direction =
		      bb_atan2 (CRAFT->entity->velocity->y, CRAFT->entity->velocity->x);
		    CRAFT->entity->velocity->x = (speed - CRAFT_ACCELERATION) * bb_cos(CRAFT->entity->direction);
		    CRAFT->entity->velocity->y = (speed - CRAFT_ACCELERATION) * bb_sin(CRAFT->entity->direction);
		  }

	      }
	  }

        if (PAUSE_PRESSED)
          {
            if (GAME_STATE == GS_PAUSED)
              {
                GAME_STATE = GS_PLAYING;
                bb_resumetimer (GAME_TIMER);
              }
            else
              {
                GAME_STATE = GS_PAUSED;
              }
            PAUSE_PRESSED = 0;
          }

        if (QUIT_PRESSED)
          {
            GAME_STATE = GS_FRONT_END;
            DestroyCraft ();
            DestroyUniverse ();
            QUIT_PRESSED = 0;
            return;
          }

        if (GAME_STATE != GS_PAUSED)
          {
	      TIMER_PROGRESS = bb_timerprogress(GAME_TIMER);
             UpdateGame ();

             if (GAME_STATE == GS_FRONT_END)
               return;
	     
		bb_resettimer (GAME_TIMER);
          }

        if (CRAFT_COLLISION)
          {
            bb_clscolor (255, 255, 255, 255);
            CRAFT_COLLISION = 0;
          }
        else
          {
            bb_clscolor (0, 0, 0, 255);
          }

        bb_cls ();

        switch (GAME_STATE)
          {
          case GS_PAUSED:
            {
              DrawCraft ();
              DrawAsteroids ();
              //DrawExplosions();
              DrawParticles ();
              DrawBullets ();

              DrawFireButton (FIRE_BUTTON_X, FIRE_BUTTON_Y);
              DrawControls (CONTROLS_X, CONTROLS_Y);
              DrawDisplay ();
              bb_flip (WINDOW);
              break;
            }
          default:
            {
	      DrawCraft ();
	      DrawAsteroids ();
              //DrawExplosions();
              DrawParticles ();
              DrawBullets ();
              DrawFireButton (FIRE_BUTTON_X, FIRE_BUTTON_Y);
              DrawControls (CONTROLS_X, CONTROLS_Y);
              DrawDisplay ();
              bb_flip (WINDOW);
            }
          }

      }

  }

  void
  UpdateGame ()
  {
    EntityCollisions ();
    BulletCollisions ();
    UpdateExplosions ();
    UpdateAsteroids ();
    UpdateParticles ();
    UpdateBullets ();
    UpdateCraft ();

    switch (GAME_STATE)
      {
      case GS_GET_READY:
      case GS_SECTOR_CLEARED:
      case GS_GAME_OVER:
        {
          MSG_FRAME++;
        }
      }

    if (ASTEROID_COUNT == 0 && GAME_STATE != GS_SECTOR_CLEARED)
      {
        GAME_STATE = GS_SECTOR_CLEARED;
        bb_playsound (SND_TELEPORTING_DEPARTURE);
      }

    switch (GAME_STATE)
      {
      case GS_GAME_OVER:
        {
          if (MSG_FRAME == MSG_TIME)
            {
              MSG_FRAME = 0;
              GAME_STATE = GS_FRONT_END;
              DestroyCraft ();
              DestroyUniverse ();
            }
          break;
        }
      case GS_SECTOR_CLEARED:
        {
          if (MSG_FRAME == MSG_TIME)
            {
              bb_playsound (SND_TELEPORTING_ARRIVAL);
              MSG_FRAME = 0;
              GAME_STATE = GS_GET_READY;
              DestroyCraft ();
              DestroyUniverse ();
              SECTOR++;
              CreateUniverse (SECTOR);
              CreateCraft ();
            }
          break;
        }
      case GS_GET_READY:
        {
          if (MSG_FRAME == MSG_TIME)
            {
              MSG_FRAME = 0;
              GAME_STATE = GS_PLAYING;
            }
          break;
        }
      }

  }

  void
  DoFrontEnd ()
  {

    int
      i,
      j,
      k,
      frames;
    int
      done = 0;
    int
      width;
    SDL_Event
      event;
    bb_clscolor (0, 0, 0, 255);
    while (1)
      {
        while (SDL_PollEvent (&event))
          {

            switch (event.type)
              {
              case SDL_QUIT:
                {
                  GAME_STATE = GS_QUIT;
                  return;
                }

              case SDL_WINDOWEVENT:
                {
                  if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    {
                      SDL_GL_MakeCurrent (WINDOW, GL_CONTEXT);
                      SDL_GL_GetDrawableSize
                        (WINDOW, &BB_GRAPHICS_WIDTH, &BB_GRAPHICS_HEIGHT);
                      glViewport (0, 0,
                                  BB_GRAPHICS_WIDTH, BB_GRAPHICS_HEIGHT);
                    }

                  break;
                }
              case SDL_FINGERDOWN:
                {

                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  for (k = 0; k < 4; k++)
                    {
                      width = bb_stringwidth (UNSELECTED_OPTIONS[k]);
                      if (pixel_x >=
                          (0.5 *
                           BB_GRAPHICS_WIDTH -
                           0.5 * width)
                          && pixel_x <=
                          (0.5 * BB_GRAPHICS_WIDTH + 0.5 * width))
                        {
                          if (pixel_y >=
                              (0.5 *
                               BB_GRAPHICS_HEIGHT)
                              + (k -
                                 2) * 2 *
                              FONT_HEIGHT
                              -
                              0.5 *
                              FONT_HEIGHT
                              && pixel_y
                              <=
                              (0.5 *
                               BB_GRAPHICS_HEIGHT)
                              + (k - 2) * 2 * FONT_HEIGHT + 0.5 * FONT_HEIGHT)
                            {

                              for (i = 0; i < 4; i++)
                                {
                                  if (i == k)
                                    {
                                      OPTIONS[i] = 1;
                                    }
                                  else
                                    {
                                      OPTIONS[i] = 0;
                                    }
                                }
                            }
                        }
                    }

                  break;
                }
              case SDL_FINGERUP:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  for (k = 0; k < 4; k++)
                    {
                      width = bb_stringwidth (SELECTED_OPTIONS[k]);
                      if (pixel_x >=
                          (0.5 *
                           BB_GRAPHICS_WIDTH -
                           0.5 * width)
                          && pixel_x <=
                          (0.5 * BB_GRAPHICS_WIDTH + 0.5 * width))
                        {
                          if (pixel_y >=
                              (0.5 *
                               BB_GRAPHICS_HEIGHT)
                              + (k -
                                 2) * 2 *
                              FONT_HEIGHT
                              -
                              0.5 *
                              FONT_HEIGHT
                              && pixel_y
                              <=
                              (0.5 *
                               BB_GRAPHICS_HEIGHT)
                              + (k - 2) * 2 * FONT_HEIGHT + 0.5 * FONT_HEIGHT)
                            {

                              switch (k)
                                {
                                case OPTION_PLAY:
                                  {
                                    GAME_STATE = GS_PLAYING;
                                    for (i = 0; i < 4; i++)
                                      {
                                        OPTIONS[i] = 0;
                                      }

                                    goto exit_;
                                  }
                                case OPTION_ABOUT:
                                  {
                                    GAME_STATE = GS_ABOUT;
                                    goto exit_;
                                  }
                                case OPTION_SETTINGS:
                                  {
                                    GAME_STATE = GS_SETTINGS;
                                    goto exit_;
                                  }
                                case OPTION_QUIT:
                                  {
                                    GAME_STATE = GS_QUIT;
                                  }
                                }
                            }
                        }
                    }

                  break;
                }

              case SDL_FINGERMOTION:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  for (k = 0; k < 4; k++)
                    {
                      width = bb_stringwidth (UNSELECTED_OPTIONS[k]);
                      if (pixel_x >=
                          (0.5 *
                           BB_GRAPHICS_WIDTH -
                           0.5 * width)
                          && pixel_x <=
                          (0.5 * BB_GRAPHICS_WIDTH + 0.5 * width))
                        {
                          if (pixel_y >=
                              (0.5 *
                               BB_GRAPHICS_HEIGHT)
                              + (k -
                                 2) * 2 *
                              FONT_HEIGHT
                              -
                              0.5 *
                              FONT_HEIGHT
                              && pixel_y
                              <=
                              (0.5 *
                               BB_GRAPHICS_HEIGHT)
                              + (k - 2) * 2 * FONT_HEIGHT + 0.5 * FONT_HEIGHT)
                            {

                              for (i = 0; i < 4; i++)
                                {
                                  if (i == k)
                                    {
                                      OPTIONS[i] = 1;
                                    }
                                  else
                                    {
                                      OPTIONS[i] = 0;
                                    }
                                }
                            }
                        }
                    }

                }

              }

          }

      exit_:

        switch (GAME_STATE)
          {
          case GS_QUIT:
            {
              TTF_Quit ();
              IMG_Quit ();
              Mix_Quit ();
              SDL_Quit ();
              return;
            }
          case GS_FRONT_END:
            {
              bb_cls ();
              DrawOptions ();
              bb_flip (WINDOW);
              break;
            }
          case GS_PLAYING:
          case GS_ABOUT:
          case GS_SETTINGS:
            {
              return;
            }
          }
      }
  }

  void
  DoAbout ()
  {

    int
      i,
      j,
      k,
      frames;
    int
      done = 0;
    int
      OK = 0;
    int
      width;
    SDL_Event
      event;

    while (1)
      {
        while (SDL_PollEvent (&event))
          {

            switch (event.type)
              {
              case SDL_WINDOWEVENT:
                {
                  if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    {
                      SDL_GL_MakeCurrent (WINDOW, GL_CONTEXT);
                      SDL_GL_GetDrawableSize
                        (WINDOW, &BB_GRAPHICS_WIDTH, &BB_GRAPHICS_HEIGHT);
                      glViewport (0, 0,
                                  BB_GRAPHICS_WIDTH, BB_GRAPHICS_HEIGHT);

                    }

                  break;
                }
              case SDL_FINGERDOWN:
                {

                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  width = bb_stringwidth ("[OK]");
                  if (pixel_y >=
                      (BB_GRAPHICS_HEIGHT -
                       2 * FONT_HEIGHT)
                      && pixel_y <= BB_GRAPHICS_HEIGHT - FONT_HEIGHT)
                    {
                      if (pixel_y >=
                          (BB_GRAPHICS_HEIGHT
                           - FONT_HEIGHT) && pixel_y <= BB_GRAPHICS_HEIGHT)
                        {
                          OK = 1;
                        }
                    }

                  break;
                }
              case SDL_FINGERUP:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  width = bb_stringwidth ("[OK]");
                  if (pixel_x >=
                      (0.5 * BB_GRAPHICS_WIDTH -
                       0.5 * width)
                      && pixel_x <= (0.5 * BB_GRAPHICS_WIDTH + 0.5 * width))
                    {
                      if (pixel_y >=
                          (BB_GRAPHICS_HEIGHT
                           - 2 * FONT_HEIGHT)
                          && pixel_y <= BB_GRAPHICS_HEIGHT - FONT_HEIGHT)
                        {
                          GAME_STATE = GS_FRONT_END;
                          goto exit_;
                        }
                    }

                  break;
                }

              case SDL_FINGERMOTION:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  width = bb_stringwidth ("[OK]");
                  if (pixel_x >=
                      (0.5 * BB_GRAPHICS_WIDTH -
                       0.5 * width)
                      && pixel_x <= (0.5 * BB_GRAPHICS_WIDTH + 0.5 * width))
                    {
                      if (pixel_y >=
                          (BB_GRAPHICS_HEIGHT
                           - 2 * FONT_HEIGHT)
                          && pixel_y <= BB_GRAPHICS_HEIGHT - FONT_HEIGHT)
                        {
                          OK = 1;
                        }
                    }

                  break;

                }

              }
          }

      exit_:

        if (GAME_STATE == GS_FRONT_END)
          {
            return;
          }
        else
          {
            bb_cls ();
            DrawAbout ();
            bb_flip (WINDOW);
            break;
          }
      }
  }

  void
  DoSettings ()
  {
    int
      j,
      frames,
      width;
    int
      done = 0;
    SDL_Event
      event;

    while (1)
      {

        while (SDL_PollEvent (&event))
          {

            switch (event.type)
              {
              case SDL_WINDOWEVENT:
                {
                  if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                    {
                      SDL_GL_MakeCurrent (WINDOW, GL_CONTEXT);
                      SDL_GL_GetDrawableSize
                        (WINDOW, &BB_GRAPHICS_WIDTH, &BB_GRAPHICS_HEIGHT);
                      glViewport (0, 0,
                                  BB_GRAPHICS_WIDTH, BB_GRAPHICS_HEIGHT);

                    }

                  break;
                }
              case SDL_FINGERDOWN:
                {

                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  if (sqrt
                      ((pixel_x -
                        CONTROLS_X) * (pixel_x -
                                       CONTROLS_X)
                       + (pixel_y -
                          CONTROLS_Y) * (pixel_y -
                                         CONTROLS_Y))
                      < CONTROLS_RADIUS + ScaleItemToDevice (84))
                    {
                      CONTROLS_ACTIVE = 1;
                    }
                  else
                    if (sqrt
                        ((pixel_x -
                          FIRE_BUTTON_X) *
                         (pixel_x -
                          FIRE_BUTTON_X) +
                         (pixel_y -
                          FIRE_BUTTON_Y) *
                         (pixel_y - FIRE_BUTTON_Y)) < FIRE_BUTTON_SIZE)
                    {
                      FIRE_BUTTON_ACTIVE = 1;
                    }

                  break;
                }
              case SDL_FINGERUP:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  width = bb_stringwidth ("[OK]");
                  if (pixel_x >=
                      (0.5 * BB_GRAPHICS_WIDTH -
                       0.5 * width)
                      && pixel_x <= (0.5 * BB_GRAPHICS_WIDTH + 0.5 * width))
                    {
                      if (pixel_y >=
                          (BB_GRAPHICS_HEIGHT
                           - 2 * FONT_HEIGHT)
                          && pixel_y <= BB_GRAPHICS_HEIGHT - FONT_HEIGHT)
                        {
                          GAME_STATE = GS_FRONT_END;
                          goto exit_;
                        }
                    }

                  if (sqrt
                      ((pixel_x -
                        CONTROLS_X) * (pixel_x -
                                       CONTROLS_X)
                       + (pixel_y -
                          CONTROLS_Y) * (pixel_y -
                                         CONTROLS_Y))
                      < CONTROLS_RADIUS + ScaleItemToDevice (84))
                    {
                      CONTROLS_ACTIVE = 0;
                    }
                  else
                    if (sqrt
                        ((pixel_x -
                          FIRE_BUTTON_X) *
                         (pixel_x -
                          FIRE_BUTTON_X) +
                         (pixel_y -
                          FIRE_BUTTON_Y) *
                         (pixel_y - FIRE_BUTTON_Y)) < FIRE_BUTTON_SIZE)
                    {
                      FIRE_BUTTON_ACTIVE = 0;
                    }

                  break;
                }
              case SDL_FINGERMOTION:
                {
                  int
                    pixel_x = event.tfinger.x * BB_GRAPHICS_WIDTH;
                  int
                    pixel_y = event.tfinger.y * BB_GRAPHICS_HEIGHT;

                  if (CONTROLS_ACTIVE)
                    {
                      CONTROLS_X = pixel_x;
                      CONTROLS_Y = pixel_y;
                    }
                  else if (FIRE_BUTTON_ACTIVE)
                    {
                      FIRE_BUTTON_X = pixel_x;
                      FIRE_BUTTON_Y = pixel_y;
                    }
                }

              }

          }

      exit_:

        if (GAME_STATE == GS_FRONT_END)
          {
            return;
          }
        else
          {
            bb_cls ();
            DrawSettings ();
            bb_flip (WINDOW);
          }

      }

  }

  double
  ScaleItemToDevice (double item_size)
  {
    return ((BB_GRAPHICS_WIDTH +
             BB_GRAPHICS_HEIGHT) / 2) / item_size * DENSITY;
  }

  void
  DrawOptions ()
  {
    int
      i;
    int
      unit = FONT_HEIGHT * 2;

    double
      center_x = 0.5 * BB_GRAPHICS_WIDTH;
    double
      center_y = 0.5 * BB_GRAPHICS_HEIGHT;

    for (i = 0; i < 4; i++)
      {
        if (OPTIONS[i])
          {
            GradientText (center_x,
                          center_y + (unit * (i - 2)),
                          SELECTED_OPTIONS[i], 1, 1);
          }
        else
          {
            GradientText (center_x,
                          center_y + (unit * (i - 2)),
                          UNSELECTED_OPTIONS[i], 1, 1);
          }
      }
  }

  int
  IsAlpha (char c)
  {
    return (c >= 65 && c <= 90) || (c >= 97 && c <= 122);
  }

  void
  DrawTextWrapped (const char *text)
  {
    int
      length = strlen (text);
    int
      width = bb_stringwidth ("A");
    int
      interval = BB_GRAPHICS_WIDTH / width;
    int
      offset = 0;
    char *
      buffer = (char *) malloc (interval + 1);
    int
      y = 0;
    int
      i;

    buffer[interval] = '\0';

    while (offset <= length)
      {
        while (text[offset] == 32)
          {
            offset++;
          }

        if (offset + interval < length)
          {

            if (IsAlpha (text[offset + interval - 1])
                && IsAlpha (text[offset + interval]))
              {

                i = interval - 1;
                while (text[offset + i] != 32)
                  {
                    i--;
                  }

                if (i <= 0)
                  {
                    memcpy (buffer, text + offset, interval);
                    GradientText (0, y, buffer, 0, 0);
                    y += FONT_HEIGHT;
                    offset += interval;
                  }
                else
                  {
                    memcpy (buffer, text + offset, interval - (interval - i));
                    buffer[interval - (interval - i)] = '\0';
                    GradientText (0, y, buffer, 0, 0);
                    y += FONT_HEIGHT;
                    offset += interval - (interval - i) + 1;
                  }

              }
            else
              {
                memcpy (buffer, text + offset, interval);
                GradientText (0, y, buffer, 0, 0);
                y += FONT_HEIGHT;
                offset += interval;
              }

          }
        else
          {
            memcpy (buffer, text + offset, interval);
            GradientText (0, y, buffer, 0, 0);
            y += FONT_HEIGHT;
            offset += interval;
          }

      }

    free (buffer);
  }

  void
  DrawAbout ()
  {

    DrawTextWrapped
      ("Skyghost Touch 1.0.3 "
       "Copyright (C) Clockwork Development Studio 2016-2017. Distributed under the FreeBSD license."
       " Any comments, bug reports or feature requests? Email them to clockworkdevstudio@gmail.com.");
    GradientText (0.5 * BB_GRAPHICS_WIDTH,
                  BB_GRAPHICS_HEIGHT - 2 * FONT_HEIGHT, "[OK]", 1, 0);
  }

  void
  DrawSettings ()
  {
    DrawControls (CONTROLS_X, CONTROLS_Y);
    DrawFireButton (FIRE_BUTTON_X, FIRE_BUTTON_Y);
    DrawTextWrapped
      ("Drag and drop the controls where you would like them to appear during gameplay.");
    GradientText (0.5 * BB_GRAPHICS_WIDTH,
                  BB_GRAPHICS_HEIGHT - 2 * FONT_HEIGHT, "[OK]", 1, 0);
  }

  void
  CreateUniverse (int sector)
  {
    int
      i;
    ASTEROID_COUNT = 0;
    
    for (i = 0; i < sector; i++)
      {
        CreateAsteroid (bb_rand (0, BB_GRAPHICS_WIDTH),
                        bb_rand (0, BB_GRAPHICS_HEIGHT),
                        bb_rand (ASTEROID_MIN_SIZE,
                                 ASTEROID_MAX_SIZE), bb_rnd (0.0, 360.0));
      }
    
  }

  void
  DestroyUniverse ()
  {
    std::list < Asteroid * >::iterator a;
    std::list < Asteroid * >::iterator ta;
    std::list < Entity2D * >::iterator e;
    std::list < Entity2D * >::iterator te;

    DestroyPolygon2D (TRANSFORMATION_POLY1);
    DestroyPolygon2D (TRANSFORMATION_POLY2);
    TRANSFORMATION_POLY1 = CreatePolygon2D (20);
    TRANSFORMATION_POLY2 = CreatePolygon2D (20);

    for (a = ASTEROID_LIST.begin (); a != ASTEROID_LIST.end (); a++)
      {
        ta = a;
        a++;
        DestroyAsteroid (*ta);
      }

    ASTEROID_LIST.clear ();

    for (e = ENTITY_LIST.begin (); e != ENTITY_LIST.end (); e++)
      {
        te = e;
        e++;
        delete (*te)->position;
        delete (*te)->velocity;
        DestroyPolygon2D ((*te)->polygon);
        delete (*te);
        ENTITY_LIST.remove (*te);
      }

    ENTITY_LIST.clear ();

  }

  void
  CreateCraft ()
  {
    std::list < Vector2D * >::iterator v;

    CRAFT = new Craft;

    CRAFT->entity = new Entity2D;
    CRAFT->entity->position = new Vector2D;
    CRAFT->entity->velocity = new Vector2D;
    ENTITY_LIST.push_back (CRAFT->entity);
    CRAFT->entity->polygon = CreatePolygon2D (4);
    CRAFT->entity->class_ = ENTITY_CLASS_CRAFT;
    CRAFT->entity->position->x = 0.5 * BB_GRAPHICS_WIDTH;
    CRAFT->entity->position->y = 0.5 * BB_GRAPHICS_HEIGHT;
    CRAFT->entity->direction = 270.0;
    CRAFT->entity->orientation = 270.0;
    CRAFT->entity->velocity->x = 0.0;
    CRAFT->entity->velocity->y = 0.0;
    CRAFT->entity->angular_velocity = 0.0;

    CRAFT->alive = 1;
    CRAFT->invincible = 1;
    CRAFT->invincible_time = CURRENT_TIME;

    v = CRAFT->entity->polygon->vertices.begin ();

    (*v)->x = 1.0 * CRAFT_SIZE;

    (*v)->y = 0.0000000;
    v++;

    (*v)->x = -0.56 * CRAFT_SIZE;
    (*v)->y = -0.56 * CRAFT_SIZE;
    v++;

    (*v)->x = -0.56 * CRAFT_SIZE;
    (*v)->y = 0.56 * CRAFT_SIZE;
    v++;

    (*v)->x = 1.0 * CRAFT_SIZE;
    (*v)->y = 0.0000000;

    CRAFT->entity->polygon->max_radius =
      CalcPolygon2DMaxRadius (CRAFT->entity->polygon);
    CRAFT->entity->mass = Polygon2DArea (CRAFT->entity->polygon);

  }

  void
  UpdateCraft ()
  {
    double
      angle;
    int
      k;

    if (!(CRAFT->alive))
      {
        return;
      }

    if (CRAFT_SHIELDS <= 0.0)
      {
        if (PLAYER_LIVES >= 1)
          {
            CreateExplosion (CRAFT->entity->position->x,
                             CRAFT->entity->position->y);
            for (k = 0; k < 64; k++)
              {
                angle = bb_rnd (0.0, 360.0);
                CreateParticle (CRAFT->entity->position->x +
                                20 * bb_cos (angle),
                                CRAFT->entity->position->y +
                                20 * bb_sin (angle), angle, 0, 255, 0);
              }
            GAME_STATE = GS_PLAYING;
            MSG_FRAME = 0;
            PLAYER_LIVES--;
            DestroyCraft ();
            CRAFT_SHIELDS = 1.0;
            CreateCraft ();
            return;

          }
        else
          {
            CreateExplosion (CRAFT->entity->position->x,
                             CRAFT->entity->position->y);
            for (k = 0; k < 64; k++)
              {
                angle = bb_rnd (0.0, 360.0);
                CreateParticle (CRAFT->entity->position->x +
                                20 * bb_cos (angle),
                                CRAFT->entity->position->y +
                                20 * bb_sin (angle), angle, 0, 255, 0);
              }
            GAME_STATE = GS_GAME_OVER;
            CRAFT->alive = 0;
            return;
          }
      }

    CRAFT->entity->orientation += ORIENTATION_CHANGE;
    CRAFT->entity->velocity->x -= VELOCITY_CHANGE_X;
    CRAFT->entity->velocity->y -= VELOCITY_CHANGE_Y;

    VELOCITY_CHANGE_X = 0.0;
    VELOCITY_CHANGE_Y = 0.0;
    ORIENTATION_CHANGE = 0.0;

    CRAFT->entity->position->x += CRAFT->entity->velocity->x * TIMER_PROGRESS * DENSITY;
    CRAFT->entity->position->y += CRAFT->entity->velocity->y * TIMER_PROGRESS * DENSITY;

    if (CRAFT->entity->position->x > BB_GRAPHICS_WIDTH)
      CRAFT->entity->position->x = 0;
    if (CRAFT->entity->position->y > BB_GRAPHICS_HEIGHT)
      CRAFT->entity->position->y = 0;
    if (CRAFT->entity->position->x < 0)
      CRAFT->entity->position->x = BB_GRAPHICS_WIDTH;
    if (CRAFT->entity->position->y < 0)
      CRAFT->entity->position->y = BB_GRAPHICS_HEIGHT;

    float
      speed =
      sqrt (CRAFT->entity->velocity->x *
            CRAFT->entity->velocity->x +
            CRAFT->entity->velocity->y * CRAFT->entity->velocity->y);

    if (CRAFT->invincible)
      {
        if (CURRENT_TIME - CRAFT->invincible_time > CRAFT_INVINCIBILITY_TIME)
          {
            CRAFT->invincible = 0;
          }
      }

    if (speed > CRAFT_MAX_SPEED)
      {
        CRAFT->entity->direction =
          bb_atan2 (CRAFT->entity->velocity->y, CRAFT->entity->velocity->x);
        CRAFT->entity->velocity->x =
          CRAFT_MAX_SPEED * bb_cos (CRAFT->entity->direction);
        CRAFT->entity->velocity->y =
          CRAFT_MAX_SPEED * bb_sin (CRAFT->entity->direction);
      }

    if (FIRE_BUTTON_ACTIVE
        && CURRENT_TIME - LAST_BULLET_TIME >= BULLET_DELAY)
      {
        bb_playsound (SND_SHOOT);
        CreateBullet (CRAFT->entity->position->x +
                      CRAFT_SIZE *
                      bb_cos (CRAFT->entity->orientation),
                      CRAFT->entity->position->y +
                      CRAFT_SIZE *
                      bb_sin (CRAFT->entity->orientation),
                      CRAFT->entity->orientation);
        LAST_BULLET_TIME = CURRENT_TIME;
      }

    if (CRAFT_THRUSTING)
      {
        CreateParticle (CRAFT->entity->position->x -
                        CRAFT_SIZE *
                        bb_cos (CRAFT->entity->orientation),
                        CRAFT->entity->position->y -
                        CRAFT_SIZE *
                        bb_sin (CRAFT->entity->orientation),
                        CRAFT->entity->orientation + 180.0,
                        bb_rand (200, 255), bb_rand (64, 255), 0);
        if (CURRENT_TIME - THRUST_START_TIME >= 530)
          {
            THRUST_START_TIME = CURRENT_TIME;
            CHN_THRUST = bb_playsound (SND_THRUST);
          }
      }

  }

  void
  DrawCraft ()
  {

    if (!(CRAFT->alive))
      {
        return;
      }

    bb_color (0, 255, 0, 255);

    RotatePolygon2D (CRAFT->entity->polygon, TRANSFORMATION_POLY1,
                     CRAFT->entity->orientation);
    DrawPolygon2D (TRANSFORMATION_POLY1, CRAFT->entity->position->x,
                   CRAFT->entity->position->y, 0.0);

    if (CRAFT->invincible)
      {
        bb_color (0, bb_rand (64, 255), bb_rand (64, 255), 255);
        bb_oval (CRAFT->entity->position->x - (1.2 * CRAFT_SIZE),
                 CRAFT->entity->position->y - 1.2 * CRAFT_SIZE,
                 2.4 * CRAFT_SIZE, 2.4 * CRAFT_SIZE, 0);
      }

  }

  void
  DestroyCraft ()
  {
    delete
      CRAFT->
      entity->
      position;
    delete
      CRAFT->
      entity->
      velocity;
    DestroyPolygon2D (CRAFT->entity->polygon);
    delete
      CRAFT->
      entity;
    ENTITY_LIST.remove (CRAFT->entity);
    delete
      CRAFT;
    CRAFT = 0;
  }

  void
  CreateAsteroid (double x, double y, double r, double direction)
  {
    Asteroid *
      a;
    int
      i;
    std::list < Vector2D * >::iterator v;
    int
      num_vertices;
    double
      speed;
    double
      angle;

    a = new Asteroid;

    a->entity = new Entity2D;
    a->entity->position = new Vector2D;
    a->entity->velocity = new Vector2D;

    num_vertices = bb_rand (6, 9);

    a->entity->class_ = ENTITY_CLASS_ASTEROID;

    a->entity->polygon = CreatePolygon2D (num_vertices);

    a->entity->position->x = x;
    a->entity->position->y = y;

    speed = bb_rnd (1.0, ASTEROID_MAX_SPEED);
    a->entity->velocity->x = speed * bb_cos (direction);
    a->entity->velocity->y = speed * bb_sin (direction);

    a->entity->direction = direction;
    a->entity->orientation = 0.0;

    a->entity->angular_velocity =
      bb_rnd (-ASTEROID_MAX_ANGULAR_VELOCITY, ASTEROID_MAX_ANGULAR_VELOCITY);

    v = a->entity->polygon->vertices.begin ();
    for (i = 0; i < a->entity->polygon->num_vertices - 1; i++)
      {
        angle =
          bb_rnd (i * (360.0 / num_vertices),
                  (i + 1) * (360.0 / num_vertices));

        (*v)->x = r * bb_cos (angle);
        (*v)->y = r * bb_sin (angle);

        v++;
      }

    (*v)->x = (*(a->entity->polygon->vertices.begin ()))->x;
    (*v)->y = (*(a->entity->polygon->vertices.begin ()))->y;

    a->entity->mass = Polygon2DArea (a->entity->polygon);

    a->entity->polygon->max_radius =
      CalcPolygon2DMaxRadius (a->entity->polygon);

    ASTEROID_COUNT++;

    ASTEROID_LIST.push_back (a);
    ENTITY_LIST.push_back (a->entity);
  }

  void
  UpdateAsteroids ()
  {
    std::list < Asteroid * >::iterator a;
    double
      speed;

    for (a = ASTEROID_LIST.begin (); a != ASTEROID_LIST.end (); a++)
      {
        RotatePolygon2D ((*a)->entity->polygon,
                         TRANSFORMATION_POLY1, (*a)->entity->orientation);

        if ((*a)->entity->position->x >= BB_GRAPHICS_WIDTH)
          (*a)->entity->position->x =
            (*a)->entity->position->x - BB_GRAPHICS_WIDTH;
        if ((*a)->entity->position->y >= BB_GRAPHICS_HEIGHT)
          (*a)->entity->position->y =
            (*a)->entity->position->y - BB_GRAPHICS_HEIGHT;
        if ((*a)->entity->position->x < 0.0)
          (*a)->entity->position->x =
            BB_GRAPHICS_WIDTH + (*a)->entity->position->x;
        if ((*a)->entity->position->y < 0.0)
          (*a)->entity->position->y =
            BB_GRAPHICS_HEIGHT + (*a)->entity->position->y;

        speed =
          sqrt ((*a)->entity->velocity->x *
                (*a)->entity->velocity->x +
                (*a)->entity->velocity->y * (*a)->entity->velocity->y);

        if (speed > ASTEROID_MAX_SPEED)
          {
            (*a)->entity->velocity->x =
              ASTEROID_MAX_SPEED * bb_cos ((*a)->entity->direction);
            (*a)->entity->velocity->y =
              ASTEROID_MAX_SPEED * bb_sin ((*a)->entity->direction);
          }

        (*a)->entity->position->x += (*a)->entity->velocity->x * TIMER_PROGRESS * DENSITY;
        (*a)->entity->position->y += (*a)->entity->velocity->y * TIMER_PROGRESS * DENSITY;

        (*a)->entity->orientation =
	    AWrap ((*a)->entity->orientation + TIMER_PROGRESS * (*a)->entity->angular_velocity);

        if ((*a)->entity->angular_velocity > ASTEROID_MAX_ANGULAR_VELOCITY)
          (*a)->entity->angular_velocity = ASTEROID_MAX_ANGULAR_VELOCITY;

        if ((*a)->entity->angular_velocity < -ASTEROID_MAX_ANGULAR_VELOCITY)
          (*a)->entity->angular_velocity = -ASTEROID_MAX_ANGULAR_VELOCITY;

      }

  }

  void
  DrawAsteroids ()
  {
    std::list < Asteroid * >::iterator a;
    int
      color;
    int
      color_factor;

    for (a = ASTEROID_LIST.begin (); a != ASTEROID_LIST.end (); a++)
      {
        bb_color (255, 255, 255, 255);
        RotatePolygon2D ((*a)->entity->polygon,
                         TRANSFORMATION_POLY1, (*a)->entity->orientation);
        DrawPolygon2D (TRANSFORMATION_POLY1,
                       (*a)->entity->position->x,
                       (*a)->entity->position->y, 0.0);
      }

  }

  void
  SplitAsteroid (Asteroid * a, double x, double y, double angle)
  {
    int
      i;
    Asteroid *
    ar1, *
      ar2;
    std::list < Vector2D * >::iterator v1, v2;  // vertex iterators
    int
      c;                        // counter
    double
      xi1 = 0, yi1 = 0;         // first intercept
    double
      xi2 = 0, yi2 = 0;         // second intercept
    std::list < Vector2D * >::iterator vi1, vi2;        // vertices immediately before intercepts
    int
      voi1 = 0;                 //numerical offset of first intercept
    int
      voi2 = 0;                 //numerical offset of second intercept
    double
      xt = 0, yt = 0;
    double
      ox = 0, oy = 0;

    RotatePolygon2D (a->entity->polygon, TRANSFORMATION_POLY1,
                     a->entity->orientation);
    c = 0;
    v1 = TRANSFORMATION_POLY1->vertices.begin ();
    for (i = 0; i < TRANSFORMATION_POLY1->num_vertices - 1; i++)
      {

        v2 = v1;
        v2++;

        LineSegmentIntersection ((*v1)->x, (*v1)->y, (*v2)->x,
                                 (*v2)->y, x, y,
                                 x + bb_cos (angle), y + bb_sin (angle));

        if (LINE_INTERSECTION_S >= 0.0 && LINE_INTERSECTION_S <= 1.0)
          {
            c++;

            if (c == 1)
              {
                xi1 = LINE_INTERSECTION_X;
                yi1 = LINE_INTERSECTION_Y;
                vi1 = v1;
                voi1 = i;
              }

            else if (c == 2)
              {
                // we've found both intercepts, so store the info and exit the loop
                xi2 = LINE_INTERSECTION_X;
                yi2 = LINE_INTERSECTION_Y;
                vi2 = v1;
                voi2 = i;
                goto escape;
              }
          }
        v1 = v2;
      }

  escape:
    if (c != 2)
      {
        exit (1);
      }

    if (c == 2)
      {

        // create first new asteroid

        ar1 = new Asteroid;
        ASTEROID_LIST.push_back (ar1);
        ar1->entity = new Entity2D;
        ENTITY_LIST.push_back (ar1->entity);
        ar1->entity->class_ = ENTITY_CLASS_ASTEROID;
        ar1->entity->position = new Vector2D;
        ar1->entity->velocity = new Vector2D;
        ar1->entity->polygon = CreatePolygon2D (voi2 - voi1 + 3);

        // first vertex is the first intersection

        v1 = ar1->entity->polygon->vertices.begin ();
        (*v1)->x = xi1;
        (*v1)->y = yi1;
        // create vertex list for asteroid 1

        v1++;
        v2 = vi1;
        v2++;

        for (i = 0; i < voi2 - voi1; i++)
          {

            (*v1)->x = (*v2)->x;
            (*v1)->y = (*v2)->y;

            v1++;
            v2++;
          }

        // last but one vertex is the second intersection

        (*v1)->x = xi2;
        (*v1)->y = yi2;

        // last vertex is the first intersection

        v1++;

        (*v1)->x = xi1;
        (*v1)->y = yi1;

        // establish centre of mass for asteroid 1

        xt = 0.0;
        yt = 0.0;

        v1 = ar1->entity->polygon->vertices.begin ();

        for (i = 0; i < ar1->entity->polygon->num_vertices; i++)
          {
            xt += (*v1)->x;
            yt += (*v1)->y;
            v1++;
          }

        ox = xt / ar1->entity->polygon->num_vertices;

        oy = yt / ar1->entity->polygon->num_vertices;

        v1 = ar1->entity->polygon->vertices.begin ();
        for (i = 0; i < ar1->entity->polygon->num_vertices; i++)
          {

            (*v1)->x -= ox;
            (*v1)->y -= oy;
            v1++;
          }

        // establish position for asteroid 1

        ar1->entity->position->x = a->entity->position->x + ox;
        ar1->entity->position->y = a->entity->position->y + oy;

        // initialise other fields in asteroid 1

        ar1->entity->velocity->x = a->entity->velocity->x;
        ar1->entity->class_ = ENTITY_CLASS_ASTEROID;
        ar1->entity->velocity->y = a->entity->velocity->y;
        ar1->entity->direction = a->entity->direction;
        ar1->entity->orientation = 0.0;
        ar1->entity->mass = Polygon2DArea (ar1->entity->polygon);
        ar1->entity->angular_velocity =
          bb_rnd (-ASTEROID_MAX_ANGULAR_VELOCITY,
                  ASTEROID_MAX_ANGULAR_VELOCITY);

        // create second new asteroid

        ar2 = new Asteroid;
        ASTEROID_LIST.push_back (ar2);
        ar2->entity = new Entity2D;
        ENTITY_LIST.push_back (ar2->entity);
        ar2->entity->class_ = ENTITY_CLASS_ASTEROID;
        ar2->entity->position = new Vector2D;
        ar2->entity->velocity = new Vector2D;
        ar2->entity->polygon =
          CreatePolygon2D (voi1 +
                           (TRANSFORMATION_POLY1->num_vertices - voi2) + 2);

        v1 = ar2->entity->polygon->vertices.begin ();
        (*v1)->x = xi2;
        (*v1)->y = yi2;

        v1++;
        v2 = vi2;
        v2++;

        for (i = 1; i < TRANSFORMATION_POLY1->num_vertices - voi2; i++)
          {
            (*v1)->x = (*v2)->x;
            (*v1)->y = (*v2)->y;

            v1++;
            v2++;
          }

        v2 = TRANSFORMATION_POLY1->vertices.begin ();
        v2++;

        for (i = 1; i <= voi1; i++)
          {
            (*v1)->x = (*v2)->x;
            (*v1)->y = (*v2)->y;
            v1++;
            v2++;
          }

        (*v1)->x = xi1;
        (*v1)->y = yi1;

        v1++;

        (*v1)->x = xi2;
        (*v1)->y = yi2;

        // establish centre of mass for asteroid 2

        xt = 0.0;
        yt = 0.0;

        v1 = ar2->entity->polygon->vertices.begin ();

        for (i = 0; i < ar2->entity->polygon->num_vertices - 1; i++)
          {
            xt += (*v1)->x;
            yt += (*v1)->y;
            v1++;
          }

        ox = xt / ar2->entity->polygon->num_vertices;
        oy = yt / ar2->entity->polygon->num_vertices;

        v1 = ar2->entity->polygon->vertices.begin ();
        for (i = 0; i < ar2->entity->polygon->num_vertices; i++)
          {
            (*v1)->x -= ox;
            (*v1)->y -= oy;
            v1++;
          }

        // establish position for asteroid 2

        ar2->entity->position->x = a->entity->position->x + ox;
        ar2->entity->position->y = a->entity->position->y + oy;

        // initialise other fields in asteroid 2

        ar2->entity->class_ = ENTITY_CLASS_ASTEROID;
        ar2->entity->velocity->x = a->entity->velocity->x;
        ar2->entity->velocity->y = a->entity->velocity->y;
        ar2->entity->direction = a->entity->direction;
        ar2->entity->orientation = 0.0;

        ar2->entity->mass = Polygon2DArea (ar2->entity->polygon);
        ar2->entity->angular_velocity =
          bb_rnd (-ASTEROID_MAX_ANGULAR_VELOCITY,
                  ASTEROID_MAX_ANGULAR_VELOCITY);

        // calculate poly max radii to use in collision detection
        ar1->entity->polygon->max_radius =
          CalcPolygon2DMaxRadius (ar1->entity->polygon);
        ar2->entity->polygon->max_radius =
          CalcPolygon2DMaxRadius (ar2->entity->polygon);

        DestroyAsteroid (a);

        ASTEROID_COUNT += 2;

        if (ar1->entity->mass < ASTEROID_MIN_MASS)
          {
            PLAYER_SCORE += (5 * (int (ar1->entity->mass) / 100));
            DestroyAsteroid (ar1);
          }

        if (ar2->entity->mass < ASTEROID_MIN_MASS)
          {
            PLAYER_SCORE += (5 * (int (ar2->entity->mass) / 100));
            DestroyAsteroid (ar2);
          }

      }

  }

  void
  DestroyAsteroid (Asteroid * a)
  {
    DestroyPolygon2D (a->entity->polygon);
    delete
      a->
      entity->
      position;
    delete
      a->
      entity->
      velocity;
    delete
      a->
      entity;
    ENTITY_LIST.remove (a->entity);
    delete
      a;
    ASTEROID_LIST.remove (a);
    ASTEROID_COUNT--;
  }

  void
  CreateParticle (double x, double y, double a, double r, double g, double b)
  {
    Particle *
      p;

    p = new Particle;
    p->x = x;
    p->y = y;
    p->a = a;
    p->r = r;
    p->g = g;
    p->b = b;
    p->creation_time = CURRENT_TIME;
    p->v = bb_rnd (0.0, PARTICLE_MAX_VELOCITY);
    PARTICLE_LIST.push_back (p);
  }

  void
  UpdateParticles ()
  {
    std::list < Particle * >::iterator p;
    std::list < Particle * >::iterator t;

    for (p = PARTICLE_LIST.begin (); p != PARTICLE_LIST.end (); p++)
      {

	(*p)->x = (*p)->x + (*p)->v * TIMER_PROGRESS * bb_cos ((*p)->a);
        (*p)->y = (*p)->y + (*p)->v * TIMER_PROGRESS * bb_sin ((*p)->a);

        if (CURRENT_TIME - (*p)->creation_time >= PARTICLE_LIFESPAN)
          {
	      delete *p;
	      p = PARTICLE_LIST.erase(p);
          }

      }
  }

  void
  DrawParticles ()
  {
    std::list < Particle * >::iterator p;
    double
      color_factor;

    for (p = PARTICLE_LIST.begin (); p != PARTICLE_LIST.end (); p++)
      {
        color_factor =
          1.0 -
          ((CURRENT_TIME -
            ((*p)->creation_time)) / float (PARTICLE_LIFESPAN));
        bb_color ((*p)->r * color_factor, (*p)->g * color_factor,
                  (*p)->b * color_factor, 255);
        bb_oval ((*p)->x - 4, (*p)->y - 4, 8, 8, 1);
      }

  }

  void
  CreateBullet (double x, double y, double a)
  {
    
    Bullet *
      b;

    b = new Bullet;

    b->x = x;
    b->y = y;
    b->a = a;

    b->creation_time = CURRENT_TIME;

    BULLET_LIST.push_back (b);
  }

  void
  UpdateBullets ()
  {
    std::list < Bullet * >::iterator b;
    std::list < Bullet * >::iterator t;

    for (b = BULLET_LIST.begin (); b != BULLET_LIST.end (); b++)
      {

        if ((*b)->x >= BB_GRAPHICS_WIDTH)
          (*b)->x = (*b)->x - BB_GRAPHICS_WIDTH;
        if ((*b)->y >= BB_GRAPHICS_HEIGHT)
          (*b)->y = (*b)->y - BB_GRAPHICS_HEIGHT;
        if ((*b)->x < 0.0)
          (*b)->x = BB_GRAPHICS_WIDTH + (*b)->x;
        if ((*b)->y < 0.0)
          (*b)->y = BB_GRAPHICS_HEIGHT + (*b)->y;

        (*b)->x = (*b)->x + TIMER_PROGRESS * BULLET_VELOCITY * bb_cos ((*b)->a) * DENSITY;
        (*b)->y = (*b)->y + TIMER_PROGRESS * BULLET_VELOCITY * bb_sin ((*b)->a) * DENSITY;

        if (CURRENT_TIME - (*b)->creation_time >= BULLET_LIFETIME)
          {
	      delete *b;
	      b = BULLET_LIST.erase(b);
          }

      }

  }

  void
  DrawBullets ()
  {
    std::list < Bullet * >::iterator b;

    for (b = BULLET_LIST.begin (); b != BULLET_LIST.end (); b++)
      {
        bb_color (255, 255, 0, 255);
        bb_oval ((*b)->x - 0.5 * BULLET_SIZE,
                 (*b)->y - 0.5 * BULLET_SIZE, BULLET_SIZE, BULLET_SIZE, 0);
      }

  }

  void
  BulletCollisions ()
  {
    std::list < Asteroid * >::iterator a, temp;
    std::list < Bullet * >::iterator b;
    double
      angle;
    int
      k;

    for (a = ASTEROID_LIST.begin (); a != ASTEROID_LIST.end (); a++)
      {

        RotatePolygon2D ((*a)->entity->polygon,
                         TRANSFORMATION_POLY1, (*a)->entity->orientation);

        for (b = BULLET_LIST.begin (); b != BULLET_LIST.end (); b++)
          {
            if (PointInPolygon
                (TRANSFORMATION_POLY1,
                 (*b)->x - (*a)->entity->position->x,
                 (*b)->y - (*a)->entity->position->y))
              {
                CreateExplosion ((*b)->x, (*b)->y);
                for (k = 0; k < 10; k++)
                  {
                    angle = bb_rnd (0.0, 360.0);
                    CreateParticle ((*b)->x +
                                    20 *
                                    bb_cos (angle),
                                    (*b)->y +
                                    20 *
                                    bb_sin (angle),
                                    bb_rnd (0.0, 360.0), 255, 255, 255);
                  }
                temp = a;
                temp++;
                SplitAsteroid ((*a),
                               (*b)->x -
                               (*a)->entity->position->x,
                               (*b)->y - (*a)->entity->position->y, (*b)->a);
                a = temp;		
		/*
		a = temp;
                delete (*b);
                BULLET_LIST.erase (b);
                goto escape;
		*/
		delete(*b);
		b = BULLET_LIST.erase(b);
                goto escape;
              }
          }
      escape:;
      }
  }

  void
  CreateExplosion (double x, double y)
  {
    Explosion *
      e;

    bb_playsound (SND_EXPLODE);

    e = new Explosion;
    e->x = x;
    e->y = y;
    e->creation_time = CURRENT_TIME;

    EXPLOSION_LIST.push_back (e);
  }

  void
  UpdateExplosions ()
  {
    std::list < Explosion * >::iterator e;
    std::list < Explosion * >::iterator t;

    for (e = EXPLOSION_LIST.begin (); e != EXPLOSION_LIST.end (); e++)
      {

        if (CURRENT_TIME - (*e)->creation_time >= EXPLOSION_LIFESPAN)
          {
	      delete *e;
	      e = EXPLOSION_LIST.erase(e);

          }

      }
  }

  void
  DrawExplosions ()
  {
    std::list < Explosion * >::iterator e;

    for (e = EXPLOSION_LIST.begin (); e != EXPLOSION_LIST.end (); e++)
      {
        bb_color (255, 0, 255, 255);
        bb_oval ((*e)->x - 0.5 * EXPLOSION_SIZE,
                 (*e)->y - 0.5 * EXPLOSION_SIZE, EXPLOSION_SIZE,
                 EXPLOSION_SIZE, 0);
	bb_oval ((*e)->x,
                 (*e)->y, 50,
                 50, 1);
      }
  }

  void
  DrawPolygon2D (Polygon2D * p, float x, float y, float a)
  {
    int
      i;
    std::list < Vector2D * >::iterator v1;
    std::list < Vector2D * >::iterator v2;

    v1 = p->vertices.begin ();

    for (i = 0; i < p->num_vertices - 1; i++)
      {
        v2 = v1;
        v2++;
        bb_line (x + (*v1)->x, y + (*v1)->y, x + (*v2)->x, y + (*v2)->y);
        v1 = v2;
      }
  }

  void
  DrawTriangle (int x, int y, float height, float angle)
  {
    float
      v1x,
      v1y,
      v2x,
      v2y,
      v3x,
      v3y;
    float
      v1xr,
      v1yr,
      v2xr,
      v2yr,
      v3xr,
      v3yr;
    float
      half_height = height / 2.0;

    v1x = bb_cos (120 + 150) * half_height;
    v1y = bb_sin (120 + 150) * half_height;
    v2x = bb_cos (240 + 150) * half_height;
    v2y = bb_sin (240 + 150) * half_height;
    v3x = bb_cos (360 + 150) * half_height;
    v3y = bb_sin (360 + 150) * half_height;

    v1xr = v1x * bb_cos (angle) - v1y * bb_sin (angle);
    v1yr = v1x * bb_sin (angle) + v1y * bb_cos (angle);

    v2xr = v2x * bb_cos (angle) - v2y * bb_sin (angle);
    v2yr = v2x * bb_sin (angle) + v2y * bb_cos (angle);

    v3xr = v3x * bb_cos (angle) - v3y * bb_sin (angle);
    v3yr = v3x * bb_sin (angle) + v3y * bb_cos (angle);

    bb_line (x + v1xr, y + v1yr, x + v2xr, y + v2yr);
    bb_line (x + v2xr, y + v2yr, x + v3xr, y + v3yr);
    bb_line (x + v3xr, y + v3yr, x + v1xr, y + v1yr);
  }

  void
  ZeroPaddedNumber (int number, char *dest, int size)
  {
    int
      i;
    int
      num_digits;
    int
      digit;
    for (i = size - 2; i >= 0; i--)
      {
        digit = number % 10;
        if (number)
          {
            dest[i] = char (48) + char (digit);
          }
        else
          {
            dest[i] = '0';
          }
        number /= 10;
      }
    dest[size - 1] = '\0';
  }

  void
  DrawDisplay ()
  {
    double
      scale_x,
      scale_y,
      k;
    char
    score[strlen ("SCORE ") + 7];
    char
    lives[strlen ("LIVES ") + 7];
    char
    sector[strlen ("SECTOR ") + 3];
    char
    sector_cleared[strlen ("SECTOR ") + 2 + strlen (" CLEARED") + 1];
    char
    you_scored[strlen ("YOU SCORED ") + 7];

    strcpy (score, "SCORE ");
    ZeroPaddedNumber (PLAYER_SCORE, score + 6, 7);
    strcpy (lives, "LIVES ");
    ZeroPaddedNumber (PLAYER_LIVES, lives + 6, 7);

    strcpy (sector, "SECTOR ");
    if (SECTOR >= 10)
      {
        sprintf (sector + strlen ("SECTOR "), "%d", SECTOR);
      }
    else
      {
        sector[strlen ("SECTOR ")] = '0';
        sprintf (sector + strlen ("SECTOR ") + 1, "%d", SECTOR);
      }

    strcpy (sector_cleared, "SECTOR ");
    if (SECTOR >= 10)
      {
        sprintf (sector_cleared + strlen ("SECTOR "), "%d", SECTOR);
      }
    else
      {
        sector_cleared[strlen ("SECTOR ")] = '0';
        sprintf (sector_cleared + strlen ("SECTOR ") + 1, "%d", SECTOR);
      }
    strcpy (sector_cleared + strlen ("SECTOR ") + 2, " CLEARED");

    strcpy (you_scored, "YOU SCORED ");
    ZeroPaddedNumber (PLAYER_SCORE, you_scored + strlen ("YOU SCORED "), 7);

    GradientText (0, 0, "SKYGHOST [P] [Q]");
    GradientText (BB_GRAPHICS_WIDTH - bb_stringwidth (score), 0, score);
    GradientText (BB_GRAPHICS_WIDTH - bb_stringwidth (lives),
                  FONT_HEIGHT, lives);

    scale_x = bb_getscalex ();
    scale_y = bb_getscaley ();
    k = 6 * FONT_HEIGHT;
    bb_setscale (k / 128.0, FONT_HEIGHT / 32.0);
    if (CRAFT)
      {
        bb_drawimagerect (HEALTH_BAR,
                          BB_GRAPHICS_WIDTH -
                          bb_stringwidth (score), FONT_HEIGHT * 2,
                          0, 0,
                          bb_imagewidth (HEALTH_BAR) *
                          CRAFT_SHIELDS, bb_imageheight (HEALTH_BAR), 0);
      }
    bb_setscale (scale_x, scale_y);
    switch (GAME_STATE)
      {
      case GS_GET_READY:
        {
          GradientText (0.5 * BB_GRAPHICS_WIDTH,
                        0.5 * BB_GRAPHICS_HEIGHT -
                        3 * FONT_HEIGHT, sector, 1, 1);
          GradientText (0.5 * BB_GRAPHICS_WIDTH,
                        0.5 * BB_GRAPHICS_HEIGHT -
                        2 * FONT_HEIGHT, "GET READY!", 1, 1);
          break;
        }
      case GS_SECTOR_CLEARED:
        {
          GradientText (0.5 * BB_GRAPHICS_WIDTH,
                        0.5 * BB_GRAPHICS_HEIGHT -
                        3 * FONT_HEIGHT, sector_cleared, 1, 1);
          GradientText (0.5 * BB_GRAPHICS_WIDTH,
                        0.5 * BB_GRAPHICS_HEIGHT -
                        2 * FONT_HEIGHT, "TELEPORTING...", 1, 1);
          break;
        }
      case GS_GAME_OVER:
        {
          GradientText (0.5 * BB_GRAPHICS_WIDTH,
                        0.5 * BB_GRAPHICS_HEIGHT -
                        3 * FONT_HEIGHT, "GAME OVER!", 1, 1);
          GradientText (0.5 * BB_GRAPHICS_WIDTH,
                        0.5 * BB_GRAPHICS_HEIGHT -
                        2 * FONT_HEIGHT, you_scored, 1, 1);
          break;
        }
      }

    bb_color (255, 255, 255, 255);
  }

  void
  DrawControls (int x, int y)
  {
    int
      i;

    bb_color (255, 0, 0, 255);

    for (i = 0; i < 4; i++)
      {
        DrawTriangle (x +
                      CONTROLS_RADIUS * bb_cos ((360.0 / 4.0) *
                                                i),
                      y +
                      CONTROLS_RADIUS * bb_sin ((360.0 / 4.0) *
                                                i),
                      ScaleItemToDevice (42), (360.0 / 4.0) * (i - 3));
      }

  }

  void
  DrawFireButton (int x, int y)
  {

    bb_color (255, 0, 0, 255);

    bb_oval (FIRE_BUTTON_X - (0.5 * FIRE_BUTTON_SIZE),
             FIRE_BUTTON_Y - (0.5 * FIRE_BUTTON_SIZE),
             FIRE_BUTTON_SIZE, FIRE_BUTTON_SIZE, 0);
    bb_oval (FIRE_BUTTON_X - (0.5 * (FIRE_BUTTON_SIZE - 8)),
             FIRE_BUTTON_Y - (0.5 * (FIRE_BUTTON_SIZE - 8)),
             FIRE_BUTTON_SIZE - 8, FIRE_BUTTON_SIZE - 8, 1);
  }

  double
  AWrap (double a)
  {
    if (a >= 360.0)
      a = fmod (a, 360.0);
    else if (a < 0.0)
      a = 360.0 + a;
    return a;
  }

}
