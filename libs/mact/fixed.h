#ifndef _fixed_public
#define _fixed_public
#ifdef __cplusplus
extern "C" {
#endif

#define FPSHIFT 16
#define FIXED1 (1l << FPSHIFT)

//***************************************************************************/
//
//  Fixed Point Macros
//
//***************************************************************************/

// Get rid of fraction and round up at the same time

#define RoundFixed( fixedval, bits )  \
   ( ( (fixedval) + ( 1 << ( (bits) - 1 ) ) ) >> (bits) )
//int32 RoundFixed ( fixed fixedval, int32 bits );


#define RoundFixed16( fixedval ) RoundFixed( fixedval, 16 )
//sdword RoundFixed16 ( fixed fixedval );

#define MakeFixed( fixedval, bits ) ( (fixedval) << (bits) )
//fixed MakeFixed ( int32 val, int32 bits );

#define MakeFixed16( fixedval ) MakeFixed ( fixedval, 16 )
//fixed MakeFixed16 ( int32 val );


//***************************************************************************
//
//  Fixed Point Functions
//
//***************************************************************************

static inline fixed FixedMul(fixed a, fixed b)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (fixed)((mul + 0x8000) >> 16);
}

static inline fixed FixedMulShift(fixed a, fixed b, fixed shift)
{
    int64_t mul = (int64_t)a * (int64_t)b;
    return (fixed)(mul >> shift);
}

static inline fixed FixedDiv2(fixed a, fixed b)
{
    return (fixed)(((int64_t)a << 16) / b);
}

static inline fixed FixedDiv(fixed a, fixed b)
{
    return (fixed)(((int64_t)a << 16) / b);
}

static inline fixed FixedDiv24(fixed a, fixed b)
{
    return (fixed)(((int64_t)a << 24) / b);
}

fixed fixedmul24(fixed a, fixed b);
static inline fixed FixedScale(fixed orig, fixed factor, fixed divisor)
{
    int64_t mul = (int64_t)orig * (int64_t)factor;
    return (fixed)(mul / divisor);
}

static inline fixed FixedSqrtLP(fixed n)
{
    int32_t a = 0;
    int32_t b = 0x40000000;

    do
    {
        if (n >= b && n - b >= a)
        {
            n -= a + b;
            a >>= 1;
            a |= b;
            b >>= 2;
        }
        else
        {
            a >>= 1;
            b >>= 1;
        }
    } while (b);
    return a << 8;
}

static inline fixed FixedSqrtHP(fixed n)
{
    int32_t a = 0;
    int32_t b = 0x40000000;

    do
    {
        if (n >= b && n - b >= a)
        {
            n -= a + b;
            a >>= 1;
            a |= b;
            b >>= 2;
        }
        else
        {
            a >>= 1;
            b >>= 1;
        }
    } while (b);

    b = 0x4000;

    a <<= 16;
    n <<= 16;

    do
    {
        if (n >= b && n - b >= a)
        {
            n -= a + b;
            a >>= 1;
            a |= b;
            b >>= 2;
        }
        else
        {
            a >>= 1;
            b >>= 1;
        }
    } while (b);

    return a;
}

static inline fixed FixedDivShift(fixed a, fixed b, fixed c)
{
    return (fixed)(((int64_t)a << c) / b);
}

#ifdef __cplusplus
};
#endif
#endif  /* _fixed_public */
