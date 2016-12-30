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
#include "SDL_opengles2.h"
#include "SDL_mixer.h"
#include "multimedia.h"
#include <string>
#include "android/log.h"
#include "math.h"

double GT_SPEED = 1.0;          // number of cycles per second, must be > 0.0
int GT_UPDATE_TIME = 1000 / GT_SPEED;   // time taken for one complete cycle
int GT_PREV_TIME;               // records the last time the text was at phase 0.0
int GT_DIR = 0;                 // scroll direction - 0 means left to right, 1 means right to left

// rgb values of first color in gradient

int GT_R1 = 30;
int GT_G1 = 30;
int GT_B1 = 238;

// rgb values of second color in gradient

int GT_R2 = 48;
int GT_G2 = 223;
int GT_B2 = 233;

// differences in rgb values of the two colors (used to calculate intermediate colors)

int GT_RDIFF = GT_R2 - GT_R1;
int GT_GDIFF = GT_G2 - GT_G1;
int GT_BDIFF = GT_B2 - GT_B1;

// Draw gradient text (animated)

extern "C"
{

  void GradientText (int x, int y, char const *lit, unsigned int cx =
                     0, unsigned int cy = 0)
  {
    int i, j;
    int offset = 0;
    int length;
    float half_length;
    int time;
    int elapsed;
    float phase, factor;
    char substring[2];

      time = SDL_GetTicks ();
      elapsed = time - GT_PREV_TIME;

      phase = (float (elapsed % GT_UPDATE_TIME) / GT_UPDATE_TIME);

    if (elapsed >= GT_UPDATE_TIME)
      {
        GT_PREV_TIME = time - (elapsed % GT_UPDATE_TIME);
      }

    length = strlen (lit);

    if (cx)
      {
        x = x - bb_stringwidth (lit) / 2;
      }

    if (cy)
      {
        y = y - bb_stringheight (lit) / 2;
      }

    half_length = float (length) / 2;

    for (i = 0; i < length; i++)
      {

        if (GT_DIR == 0)
          {
            if ((i - (length * phase)) <= 0.0)
              {
                j = length + (i - (length * phase));
              }
            else
              {
                j = (i - (length * phase));
              }
          }
        else
          {
            j = (i + fmod ((length * phase), length));
          }

        if (float (j) / length >= 0.5)
          {
            factor = 1.0 - (j - half_length) / half_length;
          }
        else
          {
            factor = j / half_length;
          }

        bb_color (GT_R1 + factor * GT_RDIFF,
                  GT_G1 + factor * GT_GDIFF, GT_B1 + factor * GT_BDIFF, 255);

        substring[0] = lit[i];
        substring[1] = '\0';

        bb_text (x + offset, y, &substring[0], 0, 0);
        offset += bb_stringwidth (&substring[0]);

      }
    return;
  }

// Set gradient colors. The first three params make up the first color (red, green blue in that order)
// the last three make up the last color.

  void SetGradientTextColor (int R1, int G1, int B1, int R2, int G2, int B2)
  {
    GT_R1 = R1;
    GT_G1 = G1;
    GT_B1 = B1;

    GT_R2 = R2;
    GT_G2 = G2;
    GT_B2 = B2;

    GT_RDIFF = GT_R2 - GT_R1;
    GT_GDIFF = GT_G2 - GT_G1;
    GT_BDIFF = GT_B2 - GT_B1;
  }

// Set speed of gradient text (cycles per second, must be > 0.0)

  void SetGradientTextSpeed (double speed)
  {
    GT_SPEED = speed;
    GT_UPDATE_TIME = 1000 / GT_SPEED;
  }

// Set anim direction - 0 means left to right (default), 1 means right to left

  void SetGradientTextDirection (int dir)
  {
    GT_DIR = (dir != 0);
  }

// Synchronises (resets) gradient text to current time

  void SyncGradientText ()
  {
    GT_PREV_TIME = SDL_GetTicks ();
  }
}
