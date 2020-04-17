#include "anim_helper.h"
// Add generative animation functions here.
// 
// The most simple function that qualifies as an animation function looks like this:
//   int dummy_animation(struct AnimationState *state) {
//     for( int a = 0; a < 25; a++ ) {
//       state->pixels[a] = { r: a*10, g: 0, b: a*10 };
//     }
//   }
//
// The general contract is:
//   state contains the following "input values", not to be altered by the animation function:
//     frame: the frame number count. One frame is 4ms long. The animation function may be called more often, though.
//   state contains the following "output values", to be written by the animation function:
//     pixels: represents a row of LEDs. Animation functions must fill this array.
//   return value:
//     -1   : the controller MUST stop this animation
//     0    : the controller MAY stop this animation or repeat it (usually returned with the last frame of a repeating animation)
//     1    : the controller SHALL continue this animation with the next frame as soon as possible.
//     >1   : the controller SHALL continue this animation with the next frame after the returned amount of milliseconds (not implemented yet)


int dummy_animation(struct AnimationState *state) {
  for( int a = 0; a < 25; a++ ) {
    state->pixels[a] = { r: a*10, g: 0, b: a*10 };
  }
  return 0;
}


#define LISSAJOUS 4
#define NUM_LEDS 25
int lissajou(struct AnimationState *state) {
  Clear(state);
  uint32_t ms = millis();
  for( int i = 0; i < LISSAJOUS; i++ ) {
    byte col = ((ms >> 5) & 0xFF) + ((256/LISSAJOUS)*i); 
    byte pos = (NUM_LEDS/2) + (sin(((ms >> 1) & 0x0000FFFF) / ((5+i)*PI)) * (NUM_LEDS/2));
    pos = _max(0, _min(NUM_LEDS-1, pos));
    state->pixels[pos] = HSV(col, 255, 255);
  }
  return 0;
}

AnimationInfo animations[] = { 
  { "dummy", dummy_animation }, 
  { "Lissajou", lissajou }
};
