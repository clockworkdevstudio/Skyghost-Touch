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
  struct Image
  {
    int width, height;
    int width_frames, height_frames;
    int handle_x, handle_y;
    GLuint *textures;
    SDL_Surface *surface;
    GLuint mask_color;
    unsigned int **masks;
    unsigned int mask_width;
    unsigned int mask_height;
  };

  struct Sound
  {
    unsigned int pitch;
    float volume;
    float pan;
    int loop;
    Mix_Chunk *sample;
  };

  struct Timer
  {
    long int time;
    long int frequency;
  };

  int init_multimedia ();
  unsigned int bb_loadimage (char const *file_name);
  unsigned int bb_imagewidth (unsigned int image_handle);
  unsigned int bb_imageheight (unsigned int image_handle);
  unsigned int bb_drawimage (unsigned int image_handle, int x, int y,
                             unsigned int frame);
  unsigned int bb_drawimagerect (unsigned int image_handle, int x, int y,
                                 int rect_x1, int rect_y1, int rect_width,
                                 int rect_height, unsigned int frame);

  void bb_setscale (double x, double y);
  double bb_getscalex ();
  double bb_getscaley ();
  void bb_setalpha (double a);
  double bb_getalpha ();

  unsigned int bb_flip (SDL_Window * WINDOW);

  unsigned int bb_loadsound (char const *name);
  unsigned int bb_loopsound (unsigned int sound_handle);
  int bb_playsound (unsigned int sound_handle);
  unsigned int bb_stopchannel (int channel);
  unsigned int bb_channelplaying (int channel);

  unsigned int bb_line (int x1, int y1, int x2, int y2);
  unsigned int bb_oval (int x, int y, int width, int height,
                        unsigned int filled);
  unsigned int bb_oval_hollow (int x, int y, int width, int height);
  unsigned int bb_oval_filled (int x, int y, int width, int height);

  unsigned int bb_color (unsigned int red, unsigned int green,
                         unsigned int blue, unsigned int alpha);
  unsigned int bb_clscolor (unsigned int red, unsigned int green,
                            unsigned int blue, unsigned int alpha);
  unsigned int bb_cls ();
  unsigned int bb_loadfont (char const *font_name, unsigned int height,
                            unsigned int bold, unsigned int italic,
                            unsigned int underlined);
  unsigned int bb_setfont (unsigned int font_handle);
  unsigned int bb_text (int x, int y, char *text, unsigned int center_x,
                        unsigned int center_y);
  unsigned int bb_stringwidth (char const *text);
  unsigned int bb_stringheight (char const *text);

  unsigned int bb_createtimer (unsigned int frequency);
  unsigned int bb_waittimer (unsigned int timer_handle);
  unsigned int bb_timerpings (unsigned int timer_handle);
  void bb_resettimer (unsigned int timer_handle);
  void bb_resumetimer (unsigned int timer_handle);
  double bb_timerdelta (unsigned int timer_handle);

}
