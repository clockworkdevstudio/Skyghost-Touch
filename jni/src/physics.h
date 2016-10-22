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

extern "C"
{
  double bb_sin (double angle);
  double bb_cos (double angle);
  double bb_atan2 (double y, double x);
  double bb_abs (double arg);
  double bb_rnd (double lower_bound, double upper_bound);
  int bb_rand (int lower_bound, int upper_bound);

  void EntityCollisions ();
  void CopyVector2D (Vector2D * dest, Vector2D * source);
  void ScaleVector2D_ (Vector2D * dest, Vector2D * source, double factor);
  void ScaleVector2D (Vector2D * dest, double factor);
  void AddVector2D_ (Vector2D * dest, Vector2D * left, Vector2D * right);
  void AddVector2D (Vector2D * dest, Vector2D * source);
  void SubtractVector2D_ (Vector2D * dest, Vector2D * left, Vector2D * right);
  void SubtractVector2D (Vector2D * dest, Vector2D * source);
  double Vector2DLength (Vector2D * v);
  void NormaliseVector2D (Vector2D * v);
  double DotProduct (Vector2D * left, Vector2D * right);
  double CrossProduct_ (Vector2D * left, Vector2D * right);
  void OrthogonalVector2D_ (Vector2D * dest, Vector2D * source);
  double ToRadians (double value);
  double ToDegrees (double value);
  int PointInPolygon (Polygon2D * polygon, float x, float y);
  int PolygonsCollide (Polygon2D * p1, double x1, double y1,
                       Polygon2D * p2, double x2, double y2);
  Polygon2D *CreatePolygon2D (int num_vertices);
  double CalcPolygon2DMaxRadius (Polygon2D * p);
  int Polygon2DArea (Polygon2D * p);
  void RotatePolygon2D (Polygon2D * source, Polygon2D * dest, float a);
  void DestroyPolygon2D (Polygon2D * p);
  int LineSegmentIntersection (float segment1x1, float segment1y1,
                               float segment1x2, float segment1y2,
                               float segment2x1, float segment2y1,
                               float segment2x2, float segment2y2);
}
