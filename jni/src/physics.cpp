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

#include <cmath>
#include <list>
#include <cstdlib>
#include "main.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_opengles2.h"
#include "multimedia.h"
#include "physics.h"
#include "android/log.h"

std::list < Entity2D * >ENTITY_LIST;
extern
  std::list <
Explosion * >
  EXPLOSION_LIST;
float
  LINE_INTERSECTION_S,
  LINE_INTERSECTION_T;
float
  LINE_INTERSECTION_X,
  LINE_INTERSECTION_Y;
extern int
  POLY_COUNT;
extern int
  EXPLOSION_SIZE;
extern unsigned int
  SND_CRASH;
extern int
  CRAFT_COLLISION;
extern double
  CRAFT_SHIELDS;

extern int
  EVENT_COLLISION;

extern
  "C"
{

  extern Polygon2D *
    TRANSFORMATION_POLY1;
  extern Polygon2D *
    TRANSFORMATION_POLY2;
  extern Craft *
    CRAFT;
  extern double
    BB_PI;

  double
  bb_sin (double angle)
  {
    return sin (angle / 360.0f * 2.0f * BB_PI);
  }
  double
  bb_cos (double angle)
  {
    return cos (angle / 360.0f * 2.0f * BB_PI);
  }

  double
  bb_atan2 (double y, double x)
  {
    return (atan2 (y, x) / (2.0f * BB_PI)) * 360.0;
  }

  double
  bb_abs (double arg)
  {
    return fabs (arg);
  }

  double
  bb_rnd (double lower_bound, double upper_bound)
  {
    double
      difference = upper_bound - lower_bound;
    return (((double) rand ()) / ((double) RAND_MAX)) * difference +
      lower_bound;
  }

  int
  bb_rand (int lower_bound, int upper_bound)
  {
    int
      result;
    int
      quotient;

    if ((upper_bound - 1) == RAND_MAX)
      return rand ();

    quotient = RAND_MAX / upper_bound;

    quotient *= upper_bound;

    while ((result = rand ()) >= quotient);

    return lower_bound + result % upper_bound;
  }

  void
  EntityCollisions ()
  {
    Vector2D
      velocity1,
      velocity2,
      velocity_translated1,
      velocity_translated2;
    double
      angular_velocity1,
      angular_velocity2;
    Vector2D
      position1,
      position2;
    Vector2D
      collisionL,
      collisionR;
    Vector2D
      collision_point,
      normal, normal_scaled1, normal_scaled2, relative_velocity;
    Vector2D
      displacement1,
      displacement2, displacement_scaled1, displacement_scaled2;
    Vector2D
      sum,
      orthogonal_vector1,
      orthogonal_vector2;
    double
      cross1,
      cross2;
    double
      distance,
      penetration;
    double
      velocity_dot_normal;
    double
      modified_velocity;
    double
      j1,
      j2;
    double
      normal_length;
    double
      angular_momentum1,
      angular_momentum2;
    double
      denominator;
    double
      inertia1,
      inertia2;
    double
      angle;
    std::list < Explosion * >::iterator ex;

    std::list < Entity2D * >::iterator e1, e2;

    for (e1 = ENTITY_LIST.begin (); e1 != ENTITY_LIST.end (); e1++)
      {

        RotatePolygon2D ((*e1)->polygon, TRANSFORMATION_POLY1,
                         (*e1)->orientation);

        e2 = e1;
        e2++;

        while (e2 != ENTITY_LIST.end ())
          {
            RotatePolygon2D ((*e2)->polygon,
                             TRANSFORMATION_POLY2, (*e2)->orientation);

            if (PolygonsCollide
                (TRANSFORMATION_POLY1, (*e1)->position->x,
                 (*e1)->position->y, TRANSFORMATION_POLY2,
                 (*e2)->position->x, (*e2)->position->y))
              {
                bb_playsound (SND_CRASH);

                if ((*e1)->class_ == ENTITY_CLASS_CRAFT
                    || (*e2)->class_ == ENTITY_CLASS_CRAFT)
                  {
                    CRAFT_COLLISION = 1;
                    if (CRAFT->invincible == 0 && CRAFT_SHIELDS > 0.0)
                      CRAFT_SHIELDS -= 0.25;
                  }
                // find angle between entities (from the point of view of entity 1)             

                angle =
                  bb_atan2 ((*e2)->position->y -
                            (*e1)->position->y,
                            (*e2)->position->x - (*e1)->position->x);

                // move entities apart

                do
                  {
                    (*e1)->position->x =
                      (*e1)->position->x + 0.5 * bb_cos (angle + 180);
                    (*e1)->position->y =
                      (*e1)->position->y + 0.5 * bb_sin (angle + 180);

                    (*e2)->position->x =
                      (*e2)->position->x + 0.5 * bb_cos (angle);
                    (*e2)->position->y =
                      (*e2)->position->y + 0.5 * bb_sin (angle);
                  }
                while (PolygonsCollide
                       (TRANSFORMATION_POLY1,
                        (*e1)->position->x,
                        (*e1)->position->y,
                        TRANSFORMATION_POLY2,
                        (*e2)->position->x, (*e2)->position->y));

                velocity1.x = (*e1)->velocity->x;
                velocity1.y = (*e1)->velocity->y;

                velocity2.x = (*e2)->velocity->x;
                velocity2.y = (*e2)->velocity->y;

                inertia1 =
                  (*e1)->polygon->max_radius *
                  (*e1)->polygon->max_radius *
                  (*e1)->mass * (2.0 / 5.0) / 3000.0;
                inertia2 =
                  (*e2)->polygon->max_radius *
                  (*e2)->polygon->max_radius *
                  (*e2)->mass * (2.0 / 5.0) / 3000.0;

                angular_momentum1 =
                  ToRadians ((*e1)->angular_velocity) * inertia1;
                angular_momentum2 =
                  ToRadians ((*e2)->angular_velocity) * inertia2;

                normal.x = (*e2)->position->x - (*e1)->position->x;
                normal.y = (*e2)->position->y - (*e1)->position->y;

                position1.x = (*e1)->position->x;
                position1.y = (*e1)->position->y;

                position2.x = (*e2)->position->x;
                position2.y = (*e2)->position->y;

                distance = sqrt (normal.x * normal.x + normal.y * normal.y);

                penetration =
                  (*e1)->polygon->max_radius +
                  (*e2)->polygon->max_radius - distance;

                normal_length = Vector2DLength (&normal);

                if (bb_abs (normal_length) < 0.0001)
                  goto skip;

                NormaliseVector2D (&normal);

                CopyVector2D (&collisionL, &position1);
                ScaleVector2D_ (&normal_scaled1, &normal,
                                (*e1)->polygon->max_radius);
                AddVector2D (&collisionL, &normal_scaled1);
                ScaleVector2D (&collisionL, 0.5);

                CopyVector2D (&collisionR, &position2);
                ScaleVector2D_ (&normal_scaled1, &normal,
                                (*e2)->polygon->max_radius);
                SubtractVector2D (&collisionR, &normal_scaled1);
                ScaleVector2D (&collisionR, 0.5);

                CopyVector2D (&collision_point, &collisionL);
                AddVector2D (&collision_point, &collisionR);

                SubtractVector2D_ (&displacement1,
                                   &collision_point, &position1);
                SubtractVector2D_ (&displacement2,
                                   &collision_point, &position2);

                angular_velocity1 = ToRadians ((*e1)->angular_velocity);
                angular_velocity2 = ToRadians ((*e2)->angular_velocity);

                OrthogonalVector2D_ (&orthogonal_vector1, &displacement1);
                OrthogonalVector2D_ (&orthogonal_vector2, &displacement2);

                ScaleVector2D (&orthogonal_vector1, angular_velocity1);
                ScaleVector2D (&orthogonal_vector2, angular_velocity2);

                AddVector2D_ (&velocity_translated1,
                              &velocity1, &orthogonal_vector1);
                AddVector2D_ (&velocity_translated2,
                              &velocity2, &orthogonal_vector2);

                SubtractVector2D_ (&relative_velocity,
                                   &velocity_translated1,
                                   &velocity_translated2);

                velocity_dot_normal =
                  DotProduct (&relative_velocity, &normal);

                if (velocity_dot_normal < 0.0)
                  goto skip;

                denominator =
                  (1.0 / (*e1)->mass +
                   1.0 / (*e2)->mass) * DotProduct (&normal, &normal);

                cross1 = CrossProduct_ (&displacement1, &normal);
                cross2 = CrossProduct_ (&displacement2, &normal);

                cross1 *= (1.0 / inertia1);
                cross2 *= (1.0 / inertia2);

                ScaleVector2D_ (&displacement_scaled1,
                                &displacement1, cross1);
                ScaleVector2D_ (&displacement_scaled2,
                                &displacement2, cross2);

                AddVector2D_ (&sum,
                              &displacement_scaled1, &displacement_scaled2);

                denominator += DotProduct (&sum, &normal);

                modified_velocity = velocity_dot_normal / denominator;

                j1 = -(1.0 + 1.0) * modified_velocity;
                j2 = -(1.0 + 1.0) * modified_velocity;

                ScaleVector2D_ (&normal_scaled1, &normal, j1 / (*e1)->mass);
                AddVector2D (&velocity1, &normal_scaled1);

                ScaleVector2D_ (&normal_scaled2, &normal, j2 / (*e2)->mass);
                SubtractVector2D (&velocity2, &normal_scaled2);

                (*e1)->velocity->x = velocity1.x;
                (*e1)->velocity->y = velocity1.y;

                (*e2)->velocity->x = velocity2.x;
                (*e2)->velocity->y = velocity2.y;

                (*e1)->direction = bb_atan2 (velocity1.y, velocity1.x);
                (*e2)->direction = bb_atan2 (velocity2.y, velocity2.x);

                ScaleVector2D_ (&normal_scaled1, &normal, j1);
                cross1 = CrossProduct_ (&displacement1, &normal_scaled1);
                angular_momentum1 = angular_momentum1 + cross1;
                angular_velocity1 = angular_momentum1 / inertia1;

                ScaleVector2D_ (&normal_scaled2, &normal, j2);
                cross2 = CrossProduct_ (&displacement2, &normal_scaled2);
                angular_momentum2 = angular_momentum2 + cross2;
                angular_velocity2 = angular_momentum2 / inertia2;

                (*e1)->angular_velocity = ToDegrees (angular_velocity1);
                (*e2)->angular_velocity = ToDegrees (angular_velocity2);

              }
          skip:
            e2++;
          }

        for (ex = EXPLOSION_LIST.begin (); ex != EXPLOSION_LIST.end (); ex++)
          {

            distance =
              sqrt (pow ((*e1)->position->x - (*ex)->x, 2) +
                    pow ((*e1)->position->y - (*ex)->y, 2));
            if (distance < EXPLOSION_SIZE)
              {
                angle =
                  bb_atan2 ((*e1)->position->y -
                            (*ex)->y, (*e1)->position->x - (*ex)->x);

                (*e1)->velocity->x +=
                  (250 *
                   (1.0 -
                    distance / EXPLOSION_SIZE) *
                   bb_cos (angle)) / (*e1)->mass;
                (*e1)->velocity->y +=
                  (250 *
                   (1.0 -
                    distance / EXPLOSION_SIZE) *
                   bb_sin (angle)) / (*e1)->mass;

              }
          }
      }
  }

  void
  CopyVector2D (Vector2D * dest, Vector2D * source)
  {
    dest->x = source->x;
    dest->y = source->y;
  }

  void
  ScaleVector2D_ (Vector2D * dest, Vector2D * source, double factor)
  {
    dest->x = source->x * factor;
    dest->y = source->y * factor;
  }

  void
  ScaleVector2D (Vector2D * dest, double factor)
  {
    dest->x = dest->x * factor;
    dest->y = dest->y * factor;
  }

  void
  AddVector2D_ (Vector2D * dest, Vector2D * left, Vector2D * right)
  {
    dest->x = left->x + right->x;
    dest->y = left->y + right->y;
  }

  void
  AddVector2D (Vector2D * dest, Vector2D * source)
  {
    dest->x = dest->x + source->x;
    dest->y = dest->y + source->y;
  }

  void
  SubtractVector2D_ (Vector2D * dest, Vector2D * left, Vector2D * right)
  {
    dest->x = left->x - right->x;
    dest->y = left->y - right->y;
  }

  void
  SubtractVector2D (Vector2D * dest, Vector2D * source)
  {
    dest->x = dest->x - source->x;
    dest->y = dest->y - source->y;
  }

  double
  Vector2DLength (Vector2D * v)
  {
    return sqrt (v->x * v->x + v->y * v->y);
  }

  void
  NormaliseVector2D (Vector2D * v)
  {
    double
      length = Vector2DLength (v);
    v->x = v->x / length;
    v->y = v->y / length;
  }

  double
  DotProduct (Vector2D * left, Vector2D * right)
  {
    return left->x * right->x + left->y * right->y;
  }

  double
  CrossProduct_ (Vector2D * left, Vector2D * right)
  {
    Vector2D
      temp;
    double
      result;
    temp.x = left->y;
    temp.y = -left->x;
    result = DotProduct (&temp, right);
    return result;
  }

  void
  OrthogonalVector2D_ (Vector2D * dest, Vector2D * source)
  {
    dest->x = source->y;
    dest->y = -source->x;
  }

  double
  ToRadians (double value)
  {
    return (value / 360.0) * 2.0 * BB_PI;
  }

  double
  ToDegrees (double value)
  {
    return (value / (2.0 * BB_PI)) * 360.0;
  }

  int
  LineSegmentIntersection (float segment1x1, float segment1y1,
                           float segment1x2, float segment1y2,
                           float segment2x1, float segment2y1,
                           float segment2x2, float segment2y2)
  {
    float
      endPoint1x,
      endPoint1y,
      endPoint2x,
      endPoint2y;
    float
      sub_s,
      sub_i;
    float
      cs,
      ci;

    // calculate end points

    endPoint1x = segment1x2 - segment1x1;
    endPoint1y = segment1y2 - segment1y1;

    endPoint2x = segment2x2 - segment2x1;
    endPoint2y = segment2y2 - segment2y1;

    // adjust to prevent division by zero
    if (endPoint2x == 0.0f)
      endPoint2x = 0.0001f;
    if (endPoint2y == 0.0f)
      endPoint2y = 0.0001f;

    // the equations we are solving

    // (1) segment1x1 + s * endPoint1x = segment2x1 + t * endPoint2x
    // (2) segment1y1 + s * endPoint1y = segment2y1 + t * endPoint2y

    // find t in terms of s

    sub_i = segment1x1 / endPoint2x - segment2x1 / endPoint2x;
    sub_s = endPoint1x / endPoint2x;

    // collect like terms

    ci = segment2y1 - segment1y1 + sub_i * endPoint2y;
    cs = endPoint1y - sub_s * endPoint2y;

    // adjust to prevent division by zero
    if (cs == 0.0f)
      cs = 0.0001f;

    // calculate s, the point along segment 1 where the lines meet
    LINE_INTERSECTION_S = ci / cs;

    // caculate t, the point along segment 2 where the lines meet
    LINE_INTERSECTION_T =
      (segment1y1 - segment2y1 +
       LINE_INTERSECTION_S * endPoint1y) / endPoint2y;

    LINE_INTERSECTION_X = segment1x1 + LINE_INTERSECTION_S * endPoint1x;
    LINE_INTERSECTION_Y = segment1y1 + LINE_INTERSECTION_S * endPoint1y;

    if (LINE_INTERSECTION_T >= 0.0f && LINE_INTERSECTION_T <= 1.0f
        && LINE_INTERSECTION_S >= 0.0f && LINE_INTERSECTION_S <= 1.0f)
      return 1;
    else
      return 0;

  }

  int
  PointInPolygon (Polygon2D * polygon, float x, float y)
  {
    int
      i;
    int
      c = 0;
    std::list < Vector2D * >::iterator v1, v2;

    v1 = polygon->vertices.begin ();
    v2 = v1;
    v2++;

    for (i = 0; i < polygon->num_vertices - 1; i++)
      {
        LineSegmentIntersection (x, y, x + 10000.0f, y, (*v1)->x,
                                 (*v1)->y, (*v2)->x, (*v2)->y);
        if (LINE_INTERSECTION_T >= 0.0f
            && LINE_INTERSECTION_T < 1.0f && LINE_INTERSECTION_S >= 0.0f)
          c++;
        v1++;
        v2++;
      }

    return (c & 1) == 1;
  }

  int
  PolygonsCollide (Polygon2D * p1, double x1, double y1, Polygon2D * p2,
                   double x2, double y2)
  {
    int
      i;
    std::list < Vector2D * >::iterator v;

    v = p1->vertices.begin ();

    for (i = 0; i < p1->num_vertices; i++)
      {

        if (PointInPolygon (p2, x1 + (*v)->x - x2, y1 + (*v)->y - y2))
          return 1;

        v++;
      }

    v = p2->vertices.begin ();

    for (i = 0; i < p2->num_vertices; i++)
      {

        if (PointInPolygon (p1, x2 + (*v)->x - x1, y2 + (*v)->y - y1))
          return 1;

        v++;
      }

    return 0;

  }

  Polygon2D *
  CreatePolygon2D (int num_vertices)
  {
    Polygon2D *
      p;
    Vector2D *
      v;
    int
      i;

    p = new Polygon2D;

    for (i = 0; i < num_vertices; i++)
      {
        v = new Vector2D;
        v->x = 0;
        v->y = 0;
        p->vertices.push_back (v);
      }

    p->num_vertices = num_vertices;

    POLY_COUNT++;

    return p;

  }

  int
  Polygon2DArea (Polygon2D * p)
  {
    int
      i;
    float
      x1,
      y1;
    float
      x2,
      y2;
    float
      a,
      area = 0;

    std::list < Vector2D * >::iterator v1 = p->vertices.begin ();
    std::list < Vector2D * >::iterator v2 = p->vertices.begin ();

    for (i = 0; i < p->num_vertices - 2; i++)
      {
        v2++ = v1;
        x1 = (*v1)->x;
        y1 = (*v1)->y;
        x2 = (*v2)->x;
        y2 = (*v2)->y;
        area += x1 * y2 - x2 * y1;
        v1++;
      }

    return area / 2;
  }

  void
  RotatePolygon2D (Polygon2D * source, Polygon2D * dest, float a)
  {
    int
      i;

    std::list < Vector2D * >::iterator v1;
    std::list < Vector2D * >::iterator v2;

    dest->num_vertices = source->num_vertices;
    dest->max_radius = source->max_radius;

    v1 = source->vertices.begin ();
    v2 = dest->vertices.begin ();
    float
      xx,
      yy;

    for (i = 0; i < source->num_vertices; i++)
      {

        xx = (*v2)->x = (*v1)->x * bb_cos (a) - (*v1)->y * bb_sin (a);
        yy = (*v2)->y = (*v1)->x * bb_sin (a) + (*v1)->y * bb_cos (a);

        v1++;
        v2++;
      }
  }

  double
  CalcPolygon2DMaxRadius (Polygon2D * p)
  {
    std::list < Vector2D * >::iterator v;
    int
      i;
    double
      r;
    double
      max_radius = 0.0;

    v = p->vertices.begin ();

    for (i = 0; i < p->num_vertices; i++)
      {
        r = sqrt ((*v)->x * (*v)->x + (*v)->y * (*v)->y);
        if (r > max_radius)
          max_radius = r;
        v++;
      }

    return max_radius;

  }

  void
  DestroyPolygon2D (Polygon2D * p)
  {
    std::list < Vector2D * >::iterator v1, v2;
    int
      i;

    v1 = p->vertices.begin ();

    for (i = 0; i < p->num_vertices; i++)
      {
        v2 = v1;
        v2++;
        delete (*v1);
        v1 = v2;
      }
    delete
      p;
    POLY_COUNT--;
  }

}
