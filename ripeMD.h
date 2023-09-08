/** 
    @filename ripeMD.h
    @author Will Greene (wgreene)
    
    Header file for ripeMD.c
*/
#ifndef _RIPEMD_H_
#define _RIPEMD_H_

#include "byteBuffer.h"

/** Name for an unsigned 32-bit integer. */
typedef unsigned int longword;

/** Number of bytes in a block. */
#define BLOCK_BYTES 64

/** Number of longwords in a block. */
#define BLOCK_LONGWORDS ( BLOCK_BYTES / sizeof( longword ) )

/** Number of iterations for each round. */
#define RIPE_ITERATIONS 16

/** prints a state of HashState in reverse order */
#define PRINT_STATE_REVERSED( state ) {                    \
    printf( "%02x", ( state & 0x000000FF ) >> BBITS * 0 ); \
    printf( "%02x", ( state & 0x0000FF00 ) >> BBITS * 1 ); \
    printf( "%02x", ( state & 0x00FF0000 ) >> BBITS * 2 ); \
    printf( "%02x", ( state & 0xFF000000 ) >> BBITS * 3 ); \
}

/** number of bitwise functions to be used */
#define NUM_BITWISE_FUNCTIONS 5

/** number of bits state C is rotated */
#define NUM_C_ROTATIONS 10

/** value of ending byte to buffer read */
#define LAST_BYTE_IN_LAST_BLOCK 0x80

/** Type for a pointer to the bitwise f function used in each round. */
typedef longword (*BitwiseFunction)( longword b, longword c, longword d );

/** Representation for the state of the hash computation.  It's just 4
    unsigned 32-bit integers. Client code can create an instance
    (statically, on the stack or on the heap), but initState() needs
    to initialize it before it can be used. */
typedef struct {
  /** Hash field A used by RipeMD  */
  longword A;
  
  /** Hash field B used by RipeMD  */
  longword B;
  
  /** Hash field C used by RipeMD  */
  longword C;
  
  /** Hash field D used by RipeMD  */
  longword D;
  
  /** Hash field E used by RipeMD  */
  longword E;
  
} HashState;

/**
    Initializes the fields of a given HashState instance.
    
    @param state HashState address
  */
void initState( HashState *state );

/**
    Pads the given buffer by bringing its length up to a a multiple of 64 bytes.
    Adds byte values as described in the RIPEMD algorithm.
    
    @param buffer ByteBuffer address
  */
void padBuffer( ByteBuffer *buffer );

/**
    Prints the final hash value stored in the given state.
    This is a 160 bit number in hexadecimal.
    
    @param state HashState address
  */
void printHash( HashState *state );

/**
    Processes the given block of 64 bytes. The given state is the input state for 
    processing the block, and it’s used as the output state for returning the resulting 
    A, B, C, D and E values after the block is processed. Calls hashRound().
    
    @param state HashState address
    @param block array of longwords to be manipulated
  */
void hashBlock( HashState *state, byte block[ BLOCK_BYTES ] );

// If we're compiling for test, expose a collection of wrapper
// functions that let us (indirectly) call internal (static) functions
// in this component.

// You shouldn't need to change the following code, and you don't need
// to worry about commenting it.

#ifdef TESTABLE

longword bitwiseF0Wrapper( longword b, longword c, longword d );
longword bitwiseF1Wrapper( longword b, longword c, longword d );
longword bitwiseF2Wrapper( longword b, longword c, longword d );
longword bitwiseF3Wrapper( longword b, longword c, longword d );
longword bitwiseF4Wrapper( longword b, longword c, longword d );

longword rotateLeftWrapper( longword value, int s );

void hashIterationWrapper( HashState *state,
                           longword datum,
                           int shift,
                           longword noise,
                           BitwiseFunction f );

void hashRoundWrapper( HashState *state,
                       longword data[ BLOCK_LONGWORDS ],
                       int perm[ RIPE_ITERATIONS ],
                       int shift[ RIPE_ITERATIONS ],
                       longword noise,
                       BitwiseFunction f );

#endif

#endif
