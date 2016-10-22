/*

Copyright (c) 2016, Clockwork Dev Studio
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

#include <list>

#define ENTITY_CLASS_ASTEROID 1
#define ENTITY_CLASS_CRAFT 2

extern "C"
{

  struct Vector2D
  {
    float x, y;
  };

  struct Polygon2D
  {
    int num_vertices;
    double max_radius;
      std::list < Vector2D * >vertices;
  };

  struct Entity2D
  {
    int class_;
    Vector2D *position;
    Vector2D *velocity;
    float angular_velocity;
    float orientation, direction;
    float mass;
    Polygon2D *polygon;
  };

  struct Craft
  {
    Entity2D *entity;
    int ID;
    int alive;
    int invincible;
    int invincible_time;
  };

  struct Asteroid
  {
    Entity2D *entity;
  };

  struct Particle
  {
    double x, y, a, v;
    double r, g, b;
    int creation_time;
  };

  struct Bullet
  {
    double x;
    double y;
    double a;
    int creation_time;
  };

  struct Explosion
  {
    double x, y;
    int creation_time;
  };

}
