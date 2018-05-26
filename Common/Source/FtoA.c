/*****************************************************************************
 *
 * MODULE:             Float to ASCII
 *
 * COMPONENT:          Float to ASCII
 *
 * AUTHOR:             Lee Mitchell
 *
 * DESCRIPTION:        Float to ASCII conversion
 *
 * $HeadURL: $
 *
 * $Revision: $
 *
 * $LastChangedBy: $
 *
 * $LastChangedDate: $
 *
 * $Id: $
 *
 *****************************************************************************
 *
 * This software is owned by Jennic and/or its supplier and is protected
 * under applicable copyright laws. All rights are reserved. We grant You,
 * and any third parties, a license to use this software solely and
 * exclusively on Jennic products. You, and any third parties must reproduce
 * the copyright and warranty notice and any other legend of ownership on each
 * copy or partial copy of the software.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS". JENNIC MAKES NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE,
 * ACCURACY OR LACK OF NEGLIGENCE. JENNIC SHALL NOT, IN ANY CIRCUMSTANCES,
 * BE LIABLE FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON WHATSOEVER.
 *
 * Copyright Jennic Ltd. 2009 All rights reserved
 *
 ****************************************************************************/

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <jendefs.h>
#include <stdlib.h>
#include <string.h>
#include "FtoA.h"
/* ZB-only defines */
#ifdef RTOS
#include "ovly.h"
#endif

#include "dbg.h"

#ifdef DEBUG_FTOA
#define TRACE_FTOA    TRUE
#else
#define TRACE_FTOA    FALSE
#endif

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
/* JIP-only defines */
#ifndef RTOS
/* Get rid of JenOS overlays */
#define OVERLAY(a)
#endif

//#define _FTOA_TOO_LARGE 1;
//#define _FTOA_TOO_SMALL 2;

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef union {
    long L;
    float F;
} LF_t;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

PRIVATE void ltoa(char *acBuffer, uint32 u32Number, uint8 u8Base);

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/


/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
//PUBLIC char *ftoa(float f, int *status)
PUBLIC OVERLAY(COLOUR_CONTROL) char *FtoA(float f)
{
    long mantissa, int_part, frac_part;
    short exp2;
    LF_t x;
    char *p;
    static char outbuf[15];

    memset(outbuf, 0, sizeof(outbuf));

//    *status = 0;
    if (f == 0.0)
    {
        outbuf[0] = '0';
        outbuf[1] = '.';
        outbuf[2] = '0';
        outbuf[3] = 0;
        return outbuf;
    }
    x.F = f;

    exp2 = (unsigned char)(x.L >> 23) - 127;
    mantissa = (x.L & 0xFFFFFF) | 0x800000;
    frac_part = 0;
    int_part = 0;

    if (exp2 >= 31)
    {
//        *status = _FTOA_TOO_LARGE;
        outbuf[0] = '-';
        outbuf[1] = 'E';
        outbuf[2] = '-';
        outbuf[3] = 0;
        return outbuf;
    }
    else if (exp2 < -23)
    {
//        *status = _FTOA_TOO_SMALL;
        outbuf[0] = '0';
        outbuf[1] = '.';
        outbuf[2] = '0';
        outbuf[3] = 0;
        return outbuf;
    }
    else if (exp2 >= 23)
    {
        int_part = mantissa << (exp2 - 23);
    }
    else if (exp2 >= 0)
    {
        int_part = mantissa >> (23 - exp2);
        frac_part = (mantissa << (exp2 + 1)) & 0xFFFFFF;
    }
    else /* if (exp2 < 0) */
    {
        frac_part = (mantissa & 0xFFFFFF) >> -(exp2 + 1);
    }

    p = outbuf;

    if (x.L < 0)
    *p++ = '-';

    if (int_part == 0)
    {
        *p++ = '0';
    }
    else
    {
        ltoa(p, int_part, 10);
        while (*p)
        p++;
    }
    *p++ = '.';

    if (frac_part == 0)
    {
        *p++ = '0';
    }
    else
    {
        char m, max;

        max = sizeof (outbuf) - (p - outbuf) - 1;
        if (max > 7)
        {
            max = 7;
        }

        /* print BCD */
        for (m = 0; m < max; m++)
        {
            /* frac_part *= 10; */
            frac_part = (frac_part << 3) + (frac_part << 1);

            *p++ = (frac_part >> 24) + '0';
            frac_part &= 0xFFFFFF;
        }
        /* delete ending zeroes */
        for (--p; p[0] == '0' && p[-1] != '.'; --p);

        ++p;
    }
    *p = 0;

    return outbuf;
}

/****************************************************************************
 *
 * NAME:       ltoa
 *
 * DESCRIPTION:
 * Converts a number in a specified base to a string representation.
 *
 * PARAMETERS:  Name            RW  Usage
 *
 * RETURNS:
 * void
 *
 ****************************************************************************/
PRIVATE OVERLAY(COLOUR_CONTROL) void ltoa(char *acBuffer, uint32 u32Number, uint8 u8Base)
{

    char acTmpBuffer[36] = {0};
    char *ptr1 = acTmpBuffer + 35;

    /* ptr2 will point to the start of the numbers, not the preceding padding if any */
    char *ptr2 = ptr1;

    uint32 c;
    uint32 u32Length = 0;

    /* terminate the string */
    *--ptr1 = '\0';

    do {

        /* get next digit to display */
        c = u32Number % u8Base;

        /* figure out what ascii character to use */
        if (c < 10) {
            if((u32Number > 0) || (u32Length == 0)){
                *--ptr1 = '0' + c;
                ptr2 = ptr1;
            } else {
                *--ptr1 = ' ';
            }
        } else {
            *--ptr1 = 'a' + (c - 10);
            ptr2 = ptr1;
        }

        /* next digit...*/
        u32Number /= u8Base;

        u32Length++;
    } while (u32Number != 0);

    /* write completed string to output */
    strncpy(acBuffer, ptr1, u32Length);

}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

