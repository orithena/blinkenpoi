#ifndef __INC_LIB8TION_H
#define __INC_LIB8TION_H


/*
 * THIS IS ONLY AN EXCERPT FROM THE ORIGINAL FastLED lib8tion.h!

 Fast, efficient 8-bit math functions specifically
 designed for high-performance LED programming.

 Because of the AVR(Arduino) and ARM assembly language
 implementations provided, using these functions often
 results in smaller and faster code than the equivalent
 program using plain "C" arithmetic and logic.


 Included are:


 - Saturating unsigned 8-bit add and subtract.
   Instead of wrapping around if an overflow occurs,
   these routines just 'clamp' the output at a maxumum
   of 255, or a minimum of 0.  Useful for adding pixel
   values.  E.g., qadd8( 200, 100) = 255.

     qadd8( i, j) == MIN( (i + j), 0xFF )
     qsub8( i, j) == MAX( (i - j), 0 )

 - Saturating signed 8-bit ("7-bit") add.
     qadd7( i, j) == MIN( (i + j), 0x7F)


 - Scaling (down) of unsigned 8- and 16- bit values.
   Scaledown value is specified in 1/256ths.
     scale8( i, sc) == (i * sc) / 256
     scale16by8( i, sc) == (i * sc) / 256

   Example: scaling a 0-255 value down into a
   range from 0-99:
     downscaled = scale8( originalnumber, 100);

   A special version of scale8 is provided for scaling
   LED brightness values, to make sure that they don't
   accidentally scale down to total black at low
   dimming levels, since that would look wrong:
     scale8_video( i, sc) = ((i * sc) / 256) +? 1

   Example: reducing an LED brightness by a
   dimming factor:
     new_bright = scale8_video( orig_bright, dimming);


 - Fast 8- and 16- bit unsigned random numbers.
   Significantly faster than Arduino random(), but
   also somewhat less random.  You can add entropy.
     random8()       == random from 0..255
     random8( n)     == random from 0..(N-1)
     random8( n, m)  == random from N..(M-1)

     random16()      == random from 0..65535
     random16( n)    == random from 0..(N-1)
     random16( n, m) == random from N..(M-1)

     random16_set_seed( k)    ==  seed = k
     random16_add_entropy( k) ==  seed += k


 - Absolute value of a signed 8-bit value.
     abs8( i)     == abs( i)


 - 8-bit math operations which return 8-bit values.
   These are provided mostly for completeness,
   not particularly for performance.
     mul8( i, j)  == (i * j) & 0xFF
     add8( i, j)  == (i + j) & 0xFF
     sub8( i, j)  == (i - j) & 0xFF


 - Fast 16-bit approximations of sin and cos.
   Input angle is a uint16_t from 0-65535.
   Output is a signed int16_t from -32767 to 32767.
      sin16( x)  == sin( (x/32768.0) * pi) * 32767
      cos16( x)  == cos( (x/32768.0) * pi) * 32767
   Accurate to more than 99% in all cases.

 - Fast 8-bit approximations of sin and cos.
   Input angle is a uint8_t from 0-255.
   Output is an UNsigned uint8_t from 0 to 255.
       sin8( x)  == (sin( (x/128.0) * pi) * 128) + 128
       cos8( x)  == (cos( (x/128.0) * pi) * 128) + 128
   Accurate to within about 2%.


 - Fast 8-bit "easing in/out" function.
     ease8InOutCubic(x) == 3(x^i) - 2(x^3)
     ease8InOutApprox(x) ==
       faster, rougher, approximation of cubic easing
     ease8InOutQuad(x) == quadratic (vs cubic) easing

 - Cubic, Quadratic, and Triangle wave functions.
   Input is a uint8_t representing phase withing the wave,
     similar to how sin8 takes an angle 'theta'.
   Output is a uint8_t representing the amplitude of
     the wave at that point.
       cubicwave8( x)
       quadwave8( x)
       triwave8( x)

 - Square root for 16-bit integers.  About three times
   faster and five times smaller than Arduino's built-in
   generic 32-bit sqrt routine.
     sqrt16( uint16_t x ) == sqrt( x)

 - Dimming and brightening functions for 8-bit
   light values.
     dim8_video( x)  == scale8_video( x, x)
     dim8_raw( x)    == scale8( x, x)
     dim8_lin( x)    == (x<128) ? ((x+1)/2) : scale8(x,x)
     brighten8_video( x) == 255 - dim8_video( 255 - x)
     brighten8_raw( x) == 255 - dim8_raw( 255 - x)
     brighten8_lin( x) == 255 - dim8_lin( 255 - x)
   The dimming functions in particular are suitable
   for making LED light output appear more 'linear'.


 - Linear interpolation between two values, with the
   fraction between them expressed as an 8- or 16-bit
   fixed point fraction (fract8 or fract16).
     lerp8by8(   fromU8, toU8, fract8 )
     lerp16by8(  fromU16, toU16, fract8 )
     lerp15by8(  fromS16, toS16, fract8 )
       == from + (( to - from ) * fract8) / 256)
     lerp16by16( fromU16, toU16, fract16 )
       == from + (( to - from ) * fract16) / 65536)
     map8( in, rangeStart, rangeEnd)
       == map( in, 0, 255, rangeStart, rangeEnd);

 - Optimized memmove, memcpy, and memset, that are
   faster than standard avr-libc 1.8.
      memmove8( dest, src,  bytecount)
      memcpy8(  dest, src,  bytecount)
      memset8(  buf, value, bytecount)

 - Beat generators which return sine or sawtooth
   waves in a specified number of Beats Per Minute.
   Sine wave beat generators can specify a low and
   high range for the output.  Sawtooth wave beat
   generators always range 0-255 or 0-65535.
     beatsin8( BPM, low8, high8)
         = (sine(beatphase) * (high8-low8)) + low8
     beatsin16( BPM, low16, high16)
         = (sine(beatphase) * (high16-low16)) + low16
     beatsin88( BPM88, low16, high16)
         = (sine(beatphase) * (high16-low16)) + low16
     beat8( BPM)  = 8-bit repeating sawtooth wave
     beat16( BPM) = 16-bit repeating sawtooth wave
     beat88( BPM88) = 16-bit repeating sawtooth wave
   BPM is beats per minute in either simple form
   e.g. 120, or Q8.8 fixed-point form.
   BPM88 is beats per minute in ONLY Q8.8 fixed-point
   form.

Lib8tion is pronounced like 'libation': lie-BAY-shun

*/



#include <stdint.h>

#define LIB8STATIC __attribute__ ((unused)) static inline
#define LIB8STATIC_ALWAYS_INLINE __attribute__ ((always_inline)) static inline

#if (defined(ARDUINO) || defined(SPARK) || defined(FASTLED_HAS_MILLIS)) && !defined(USE_GET_MILLISECOND_TIMER)
// Forward declaration of Arduino function 'millis'.
//uint32_t millis();
#define GET_MILLIS millis
#else
uint32_t get_millisecond_timer();
#define GET_MILLIS get_millisecond_timer
#endif

/// Return the current seconds since boot in a 16-bit value.  Used as part of the
/// "every N time-periods" mechanism
LIB8STATIC uint16_t seconds16()
{
    uint32_t ms = GET_MILLIS();
    uint16_t s16;
    s16 = ms / 1000;
    return s16;
}

/// Return the current minutes since boot in a 16-bit value.  Used as part of the
/// "every N time-periods" mechanism
LIB8STATIC uint16_t minutes16()
{
    uint32_t ms = GET_MILLIS();
    uint16_t m16;
    m16 = (ms / (60000L)) & 0xFFFF;
    return m16;
}

/// Return the current hours since boot in an 8-bit value.  Used as part of the
/// "every N time-periods" mechanism
LIB8STATIC uint8_t hours8()
{
    uint32_t ms = GET_MILLIS();
    uint8_t h8;
    h8 = (ms / (3600000L)) & 0xFF;
    return h8;
}


/// Helper routine to divide a 32-bit value by 1024, returning
/// only the low 16 bits. You'd think this would be just
///   result = (in32 >> 10) & 0xFFFF;
/// and on ARM, that's what you want and all is well.
/// But on AVR that code turns into a loop that executes
/// a four-byte shift ten times: 40 shifts in all, plus loop
/// overhead. This routine gets exactly the same result with
/// just six shifts (vs 40), and no loop overhead.
/// Used to convert millis to 'binary seconds' aka bseconds:
/// one bsecond == 1024 millis.
LIB8STATIC uint16_t div1024_32_16( uint32_t in32)
{
    uint16_t out16;
#if defined(__AVR__)
    asm volatile (
                  "  lsr %D[in]  \n\t"
                  "  ror %C[in]  \n\t"
                  "  ror %B[in]  \n\t"
                  "  lsr %D[in]  \n\t"
                  "  ror %C[in]  \n\t"
                  "  ror %B[in]  \n\t"
                  "  mov %B[out],%C[in] \n\t"
                  "  mov %A[out],%B[in] \n\t"
                  : [in] "+r" (in32),
                  [out] "=r" (out16)
                  );
#else
    out16 = (in32 >> 10) & 0xFFFF;
#endif
    return out16;
}

/// bseconds16 returns the current time-since-boot in
/// "binary seconds", which are actually 1024/1000 of a
/// second long.
LIB8STATIC uint16_t bseconds16()
{
    uint32_t ms = GET_MILLIS();
    uint16_t s16;
    s16 = div1024_32_16( ms);
    return s16;
}


// Classes to implement "Every N Milliseconds", "Every N Seconds",
// "Every N Minutes", "Every N Hours", and "Every N BSeconds".
#if 1
#define INSTANTIATE_EVERY_N_TIME_PERIODS(NAME,TIMETYPE,TIMEGETTER) \
class NAME {    \
public: \
    TIMETYPE mPrevTrigger;  \
    TIMETYPE mPeriod;   \
    \
    NAME() { reset(); mPeriod = 1; }; \
    NAME(TIMETYPE period) { reset(); setPeriod(period); };    \
    void setPeriod( TIMETYPE period) { mPeriod = period; }; \
    TIMETYPE getTime() { return (TIMETYPE)(TIMEGETTER()); };    \
    TIMETYPE getPeriod() { return mPeriod; };   \
    TIMETYPE getElapsed() { return getTime() - mPrevTrigger; }  \
    TIMETYPE getRemaining() { return mPeriod - getElapsed(); }  \
    TIMETYPE getLastTriggerTime() { return mPrevTrigger; }  \
    bool ready() { \
        bool isReady = (getElapsed() >= mPeriod);   \
        if( isReady ) { reset(); }  \
        return isReady; \
    }   \
    void reset() { mPrevTrigger = getTime(); }; \
    void trigger() { mPrevTrigger = getTime() - mPeriod; }; \
        \
    operator bool() { return ready(); } \
};
INSTANTIATE_EVERY_N_TIME_PERIODS(CEveryNMillis,uint32_t,GET_MILLIS);
INSTANTIATE_EVERY_N_TIME_PERIODS(CEveryNSeconds,uint16_t,seconds16);
INSTANTIATE_EVERY_N_TIME_PERIODS(CEveryNBSeconds,uint16_t,bseconds16);
INSTANTIATE_EVERY_N_TIME_PERIODS(CEveryNMinutes,uint16_t,minutes16);
INSTANTIATE_EVERY_N_TIME_PERIODS(CEveryNHours,uint8_t,hours8);
#else

// Under C++11 rules, we would be allowed to use not-external
// -linkage-type symbols as template arguments,
// e.g., LIB8STATIC seconds16, and we'd be able to use these
// templates as shown below.
// However, under C++03 rules, we cannot do that, and thus we
// have to resort to the preprocessor to 'instantiate' 'templates',
// as handled above.
template<typename timeType,timeType (*timeGetter)()>
class CEveryNTimePeriods {
public:
    timeType mPrevTrigger;
    timeType mPeriod;

    CEveryNTimePeriods() { reset(); mPeriod = 1; };
    CEveryNTimePeriods(timeType period) { reset(); setPeriod(period); };
    void setPeriod( timeType period) { mPeriod = period; };
    timeType getTime() { return (timeType)(timeGetter()); };
    timeType getPeriod() { return mPeriod; };
    timeType getElapsed() { return getTime() - mPrevTrigger; }
    timeType getRemaining() { return mPeriod - getElapsed(); }
    timeType getLastTriggerTime() { return mPrevTrigger; }
    bool ready() {
        bool isReady = (getElapsed() >= mPeriod);
        if( isReady ) { reset(); }
        return isReady;
    }
    void reset() { mPrevTrigger = getTime(); };
    void trigger() { mPrevTrigger = getTime() - mPeriod; };

    operator bool() { return ready(); }
};
typedef CEveryNTimePeriods<uint16_t,seconds16> CEveryNSeconds;
typedef CEveryNTimePeriods<uint16_t,bseconds16> CEveryNBSeconds;
typedef CEveryNTimePeriods<uint32_t,millis> CEveryNMillis;
typedef CEveryNTimePeriods<uint16_t,minutes16> CEveryNMinutes;
typedef CEveryNTimePeriods<uint8_t,hours8> CEveryNHours;
#endif


#define CONCAT_HELPER( x, y ) x##y
#define CONCAT_MACRO( x, y ) CONCAT_HELPER( x, y )
#define EVERY_N_MILLIS(N) EVERY_N_MILLIS_I(CONCAT_MACRO(PER, __COUNTER__ ),N)
#define EVERY_N_MILLIS_I(NAME,N) static CEveryNMillis NAME(N); if( NAME )
#define EVERY_N_SECONDS(N) EVERY_N_SECONDS_I(CONCAT_MACRO(PER, __COUNTER__ ),N)
#define EVERY_N_SECONDS_I(NAME,N) static CEveryNSeconds NAME(N); if( NAME )
#define EVERY_N_BSECONDS(N) EVERY_N_BSECONDS_I(CONCAT_MACRO(PER, __COUNTER__ ),N)
#define EVERY_N_BSECONDS_I(NAME,N) static CEveryNBSeconds NAME(N); if( NAME )
#define EVERY_N_MINUTES(N) EVERY_N_MINUTES_I(CONCAT_MACRO(PER, __COUNTER__ ),N)
#define EVERY_N_MINUTES_I(NAME,N) static CEveryNMinutes NAME(N); if( NAME )
#define EVERY_N_HOURS(N) EVERY_N_HOURS_I(CONCAT_MACRO(PER, __COUNTER__ ),N)
#define EVERY_N_HOURS_I(NAME,N) static CEveryNHours NAME(N); if( NAME )

#define CEveryNMilliseconds CEveryNMillis
#define EVERY_N_MILLISECONDS(N) EVERY_N_MILLIS(N)
#define EVERY_N_MILLISECONDS_I(NAME,N) EVERY_N_MILLIS_I(NAME,N)

#endif
