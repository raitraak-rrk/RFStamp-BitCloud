/*********************************************************************************************//**
\file

\brief

\author
    Atmel Corporation: http://www.atmel.com \n
    Support email: avr@atmel.com

  Copyright (c) 2008-2013, Atmel Corporation. All rights reserved.
  Licensed under Atmel's Limited License Agreement (BitCloudTM).

\internal

$Id: N_Util.h 27960 2015-06-15 15:10:37Z agasthian.s $

***************************************************************************************************/

#ifndef N_UTIL_H
#define N_UTIL_H

/***************************************************************************************************
* INCLUDE FILES
***************************************************************************************************/

#include "N_Types.h"

#if defined(__ICCAVR__)
#include <pgmspace.h>
#include <ioavr.h>
#include <intrinsics.h>
#endif
#if defined(__ICCARM__)
#if defined(ATSAMD20J18)
#include <atsamd20.h>
#endif
#if defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
#include <atsamr21.h>
#endif
#include <core_cm0plus.h>
#endif
#if defined(__GNUC__)
#if defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
#include <avr/io.h>
#include <avr/interrupt.h>
#elif defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
#include <atsamr21.h>
#include <core_cm0plus.h>
#endif
#endif

#include <string.h>

/***************************************************************************************************
* C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
* EXPORTED MACROS AND CONSTANTS
***************************************************************************************************/
/** CPU-architecture dependent storage modifier to indicate the constant should be in ROM
*/
#if defined(__ICCAVR__)
#define N_UTIL_ROM __farflash
#else
#define N_UTIL_ROM
#endif

#define N_UTIL_STACK_CHECK_DISABLED (0xFFFFu)

/** Macro to get (at compile-time) the number of elements in an array.
    \param ar The array to get the size of (not a pointer)
*/
#define N_UTIL_ARRAY_SIZE(ar) (sizeof(ar)/sizeof(*(ar)))

/** Macro to check a condition during compile time.
    \param condition The condition that needs to be valid at compile time.
    \note If the condition is false at compile time, the compiler will generate an error that a pointer is initialized with -1.
*/
#ifdef _lint
#define N_UTIL_COMPILE_ASSERT(condition)
#else
#define N_UTIL_COMPILE_ASSERT(condition)  static char* dummy##__LINE__ = ((!!(condition)) - 1); (void)dummy##__LINE__
#endif

/** Convert a boolean expression (which has type int) to a bool.

    This is to keep lint happy if the bool type is a 8 bit unsigned integer
    instead of a native int.
*/
#define N_UTIL_BOOL(x) ((x) ? TRUE : FALSE)

/** Wrapper to swallow semicolon */
#define N_UTIL_SWALLOW_SEMICOLON(x) do { x } while(0)

/** Macro to create a mask of a single bit
    \param bit The bit number (0..15) to set in the mask
*/
#define N_UTIL_MASK(bit)                          ( 1u<<(bit) )

/** Macro to clear a mask, typically of an I/O port
    \param port The port to clear the mask in
    \param mask A mask. The bits set in the mask will be cleared in the port
*/
#define N_UTIL_PORT_CLEAR(port,mask)              N_UTIL_SWALLOW_SEMICOLON( (port) &= ~(mask); )

/** Macro to set a mask, typically of an I/O port
    \param port The port to set bits in
    \param mask A mask. The bits set in the mask will be cleared in the port
*/
#define N_UTIL_PORT_SET(port,mask)                N_UTIL_SWALLOW_SEMICOLON( (port) |= (mask); )

/** Macro to set the bits from value that are set in mask
    \param port The port to write to
    \param mask A mask that indicates what bits in the port to change
    \param value The bits to write to the port. Only the bits of value that are also in mask have any effect.
*/
#define N_UTIL_PORT_WRITEBITS(port,mask,value)    N_UTIL_SWALLOW_SEMICOLON( (port) = ((port) & (~(mask))) | ((value) & (mask)); )

/** Refers to a pin of a port. E.g. 0x14 means port 1 pin 4
 */
typedef uint8_t N_Util_Pin_t;
#define N_UTIL_PIN(port,pin)  ((((uint8_t)(port))<<4u)|((uint8_t)(pin)))

/** Compares two strings for equality
    \param s The first string
    \param t The second string
    \returns TRUE if the strings are equal, else FALSE
*/
#define N_UTIL_STR_EQUAL(s,t) ( strcmp((s), (t)) == 0 )

#if defined(__ICCAVR__)
#define IF_STR_EQUAL_TOKEN1(x, y) x ## y
#define IF_STR_EQUAL_TOKEN2(x, y) IF_STR_EQUAL_TOKEN1(x, y)

#define IF_STR_EQUAL(str,conststr) \
    static const __farflash char IF_STR_EQUAL_TOKEN2(const,__LINE__)[] = conststr; if ( strcmp_P(str, IF_STR_EQUAL_TOKEN2(const,__LINE__)) == 0 )
#else
#define IF_STR_EQUAL(s,t) if (N_UTIL_STR_EQUAL(s, t))     
#endif
/** Utility macro for concatenation
*/
#define N_UTIL_CONCAT_HELPER(x,y) x ## y
#define N_UTIL_CONCAT(x,y) N_UTIL_CONCAT_HELPER(x,y)

/** Declare administration for the N_UTIL_CALLBACK macros
    \note The _size variable is optimized away by the compiler.
    \note The subscribersArray cannot be 0-size, but for the 0-subscriber case everything is optimized away anyway.
*/
#define N_UTIL_CALLBACK_DECLARE(callbackType,subscribersArray,numSubscribers) \
    static const callbackType* subscribersArray[/*lint -e(506) */ numSubscribers?numSubscribers:1u] = { NULL }; \
    static const uint8_t N_UTIL_CONCAT(subscribersArray,_size) = numSubscribers

/** Standard implementation of a Subscribe function.
    \param callbackType Type of the callback interface structure
    \param subscribersArray Array with subscribers
    \param pNewCb Pointer to add to subscribersArray
*/
#define N_UTIL_CALLBACK_SUBSCRIBE(callbackType, subscribersArray, pNewCb) \
    N_UTIL_SWALLOW_SEMICOLON( \
        /*lint -e{506} */ \
        N_ERRH_ASSERT_FATAL(N_UTIL_CONCAT(subscribersArray,_size) != 0u); \
        uint8_t i=(uint8_t)N_UTIL_CONCAT(subscribersArray,_size) - 1u; \
        do { \
            if (subscribersArray[i] == NULL) \
            { \
                subscribersArray[i] = pNewCb; \
                break; \
            } \
            N_ERRH_ASSERT_FATAL(i != 0u); \
            i--; \
        } while(/*lint -e(506) */ 1); \
    )

/** Finds the first active subscriber, calls it and executes a statement
    \param callbackType Type of the callback interface structure
    \param subscribersArray Array with subscribers
    \param statement Statement to do for each subscriber, e.g. "i ="
    \param member Callback function to do
    \param arguments Parentised list of arguments suitable for member
*/
#define N_UTIL_CALLBACK_STATEMENT(callbackType, subscribersArray, statement, member, arguments) \
    N_UTIL_SWALLOW_SEMICOLON( \
        /*lint -e{506} */ \
        if (N_UTIL_CONCAT(subscribersArray,_size) != 0u) \
        { \
            uint8_t i_ = (uint8_t)N_UTIL_CONCAT(subscribersArray,_size) - 1u; \
            do { \
                if ((subscribersArray[i_]) != NULL) \
                { \
                    if (subscribersArray[i_]->member != NULL) \
                    { \
                        statement subscribersArray[i_]->member arguments; \
                    } \
                } \
                if (i_ == 0u) \
                { \
                    break; \
                } \
                i_--; \
            } while (/*lint -e(506) */ 1); \
        } \
    )

/** Standard implementation to do a callback to all subscribers.
    \param callbackType Type of the callback interface structure
    \param subscribersArray Array with subscribers
    \param member Callback function to do
    \param arguments Parentised list of arguments suitable for member
*/
#define N_UTIL_CALLBACK(callbackType, subscribersArray, member, arguments) \
    N_UTIL_CALLBACK_STATEMENT(callbackType, subscribersArray, /* no statement */, member, arguments)

/***************************************************************************************************
* TYPEDEFS
***************************************************************************************************/

/***************************************************************************************************
* IN-LINE FUNCTIONS
***************************************************************************************************/

/** Takes a byte out of a uint32_t
    \param var The 32-bit integer to break up
    \param byteIndex The byte (0..3) to return. 0 for the LS byte, 3 for the MS byte.
*/
static inline uint8_t N_Util_BreakUint32(uint32_t var, uint8_t byteIndex)
{
    uint8_t bits = byteIndex * 8u;
    var >>= bits;
    return (uint8_t)var;
}

static inline uint32_t N_Util_BuildUint32(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3)
{
    return (  (uint32_t)byte0
           | (((uint32_t)byte1) << 8u)
           | (((uint32_t)byte2) << 16u)
           | (((uint32_t)byte3) << 24u) );
}

static inline uint16_t N_Util_BuildUint16(uint8_t lowByte, uint8_t highByte)
{
    return ( (uint16_t)lowByte | (((uint16_t)highByte) << 8u) );
}

static inline uint8_t N_Util_HighByteUint16(uint16_t a)
{
    return (uint8_t)((a >> 8u) & 0xFFu);
}

static inline uint8_t N_Util_LowByteUint16(uint16_t a)
{
    return (uint8_t)(a & 0xFFu);
}

/***************************************************************************************************
* EXPORTED FUNCTIONS
***************************************************************************************************/

/** Reset the system
*/
void N_Util_SystemReset(void);

/** Return the highest network update id, taking into account the wrap-around.
    \param id1 One of the update id's to check
    \param id2 One of the update id's to check
    \returns The highest update id

    The algorithm is specified in the ZigBee LightLink specification, section 8.6
*/
uint8_t N_Util_HighestNetworkUpdateId(uint8_t id1, uint8_t id2);

/** Starts a timer. When it fails, it will assert.
    \param taskId The task to send the timeout event to
    \param event The timeout event to trigger
    \param timeoutMs The timeout in milliseconds
*/
void N_Util_StartTimerSafe(uint8_t taskId, uint16_t event, uint16_t timeoutMs);

#if defined(_WIN32)

    // The PC build doesn't use interrupts, so this is a dummy implementation
    typedef bool N_Util_CriticalSection_SaveState_t;

    static inline N_Util_CriticalSection_SaveState_t N_Util_CriticalSection_Enter(void)
    {
        return TRUE;
    }

    static inline void N_Util_CriticalSection_Exit(N_Util_CriticalSection_SaveState_t state)
    {
    }

#elif defined(__ICCAVR__)

    typedef uint8_t N_Util_CriticalSection_SaveState_t;

    static inline N_Util_CriticalSection_SaveState_t N_Util_CriticalSection_Enter(void)
    {
        uint8_t state = SREG;
        __disable_interrupt();
        return state;
    }

    static inline void N_Util_CriticalSection_Exit(N_Util_CriticalSection_SaveState_t state)
    {
        SREG = state;
    }

#elif defined(__GNUC__)
  #if defined(ATMEGA256RFR2) || defined(ATMEGA2564RFR2)
    typedef uint8_t N_Util_CriticalSection_SaveState_t;

    static inline N_Util_CriticalSection_SaveState_t N_Util_CriticalSection_Enter(void)
    {
        uint8_t state = SREG;
        cli();
        return state;
    }

    static inline void N_Util_CriticalSection_Exit(N_Util_CriticalSection_SaveState_t state)
    {
        SREG = state;
    }
  #elif defined(ATSAMR21G18A) || defined(ATSAMR21E18A) || defined(ATSAMR21E19A)
    typedef uint32_t N_Util_CriticalSection_SaveState_t;

    static inline N_Util_CriticalSection_SaveState_t N_Util_CriticalSection_Enter(void)
    {
        uint32_t state = __get_PRIMASK();
        __disable_irq();
        return state;
    }

    static inline void N_Util_CriticalSection_Exit(N_Util_CriticalSection_SaveState_t state)
    {
        __set_PRIMASK(state);
    }
  #endif
#elif defined (__ICCARM__)

    typedef uint32_t N_Util_CriticalSection_SaveState_t;

    static inline N_Util_CriticalSection_SaveState_t N_Util_CriticalSection_Enter(void)
    {
        uint32_t state = __get_PRIMASK();
        __disable_irq();
        return state;
    }

    static inline void N_Util_CriticalSection_Exit(N_Util_CriticalSection_SaveState_t state)
    {
        __set_PRIMASK(state);
    }

#endif

/** Return a random uint16_t. Not suitable for security-related code!!
*/
uint16_t N_Util_Random(void);

/** Start a stopwatch timer. After starting the timer, Util_TimerElapsed can be used to get the elapsed time
    \returns The value that has to be supplied to Util_TimerElapsed to get the elapsed time
    \note There is no stop function.
*/
uint32_t N_Util_TimerStart(void);

/** Get the elapsed time since the stopwatch timer was started
    \param startValue The value returned by Util_TimerStart
    \returns The elapsed time in milliseconds since Util_TimerStart was called
    \note The stopwatch timer uses timer 5 of the ATMega256R(4)FR2. This is a 16 bit timer.
*/
uint32_t N_Util_TimerElapsed(uint32_t startValue);

/** Return the minimum free stack space. Refers to the free space on External Stack.
    \returns Free bytes on Stack or N_UTIL_STACK_CHECK_DISABLED on error.
*/
uint16_t N_Util_GetMinimumStackLeft(void);

/** Returns how many bytes are available on stack. Refers to the free space on External Stack.
    \returns Current free bytes on Stack or N_UTIL_STACK_CHECK_DISABLED on error.
*/
uint16_t N_Util_GetCurrentStackLeft(void);


/***************************************************************************************************
* END OF C++ DECLARATION WRAPPER
***************************************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // N_UTIL_H
