/** 
    @filename ripeMD.c
    @author Will Greene (wgreene)
    
    Contains functions for computing a RIPEMD-160 hash.
*/
#include "ripeMD.h"
#include "byteBuffer.h"

/**
    Initializes the fields of a given HashState instance.
    
    @param state HashState address
  */
void initState( HashState *state )
{
    state->A = 0x67452301;
    state->B = 0xEFCDAB89;
    state->C = 0x98BADCFE;
    state->D = 0x10325476;
    state->E = 0xC3D2E1F0;
}

/**
    Pads the given buffer by bringing its length up to a a multiple of 64 bytes.
    Adds byte values as described in the RIPEMD algorithm.
    
    @param buffer ByteBuffer address
  */
void padBuffer( ByteBuffer *buffer )
{
    longword numBits = buffer->len * BBITS;
    
    int length = buffer->len;
    
    addByte( buffer, LAST_BYTE_IN_LAST_BLOCK );
        
    for ( int i = length % BLOCK_BYTES + 1; i < ( BLOCK_BYTES - BBITS ); i++ )
        addByte( buffer, 0 );
        
    addByte( buffer, ( numBits & 0x000000FF ) >> BBITS * 0 );
    addByte( buffer, ( numBits & 0x0000FF00 ) >> BBITS * 1 );
    addByte( buffer, ( numBits & 0x00FF0000 ) >> BBITS * 2 );
    addByte( buffer, ( numBits & 0xFF000000 ) >> BBITS * 3 );
    
    for ( int i = 0; i < 4; i++ )
        addByte( buffer, 0 );
}

/**
    Prints the final hash value stored in the given state.
    This is a 160 bit number in hexadecimal.
    
    @param state HashState address
  */
void printHash( HashState *state )
{
    PRINT_STATE_REVERSED( state->A );
    PRINT_STATE_REVERSED( state->B );
    PRINT_STATE_REVERSED( state->C );
    PRINT_STATE_REVERSED( state->D );
    PRINT_STATE_REVERSED( state->E );
    
    printf( "\n" );
}

/**
    Function (F0) for combining the values of 3 longwords.
    
    @param a 1st longword ( this is technically value B )
    @param a 2nd longword ( this is technically value C )
    @param a 3rd longword ( this is technically value D )
    @return longword resulting from the bitwise operation
  */
static longword bitwiseF0( longword a, longword b, longword c )
{
    return a ^ b ^ c;
}

/**
    Function (F1) for combining the values of 3 longwords.
    
    @param a 1st longword ( this is technically value B )
    @param a 2nd longword ( this is technically value C )
    @param a 3rd longword ( this is technically value D )
    @return longword resulting from the bitwise operation
  */
static longword bitwiseF1( longword a, longword b, longword c )
{
    return ( a & b ) | ( ~a & c );
}

/**
    Function (F2) for combining the values of 3 longwords.
    
    @param a 1st longword ( this is technically value B )
    @param a 2nd longword ( this is technically value C )
    @param a 3rd longword ( this is technically value D )
    @return longword resulting from the bitwise operation
  */
static longword bitwiseF2( longword a, longword b, longword c )
{
    return ( a | ~b ) ^ c;
}

/**
    Function (F3) for combining the values of 3 longwords.
    
    @param a 1st longword ( this is technically value B )
    @param a 2nd longword ( this is technically value C )
    @param a 3rd longword ( this is technically value D )
    @return longword resulting from the bitwise operation
  */
static longword bitwiseF3( longword a, longword b, longword c )
{
    return ( a & c ) | ( b & ~c );
}

/**
    Function (F4) for combining the values of 3 longwords.
    
    @param a 1st longword ( this is technically value B )
    @param a 2nd longword ( this is technically value C )
    @param a 3rd longword ( this is technically value D )
    @return longword resulting from the bitwise operation
  */
static longword bitwiseF4( longword a, longword b, longword c )
{
    return a ^ ( b | ~c );
}

/**
    Shifts the given value by s bits, with wraparound.
    
    @param value longword to be shifted
    @param s number of bits to shift
    @return new longword value
  */
static longword rotateLeft( longword value, int s )
{
    longword shift = 0x00000000;
    
    if ( s == 1 )
        shift = 0x80000000;
    else if ( s == 2 )
        shift = 0xC0000000;
    else if ( s == 3 )
        shift = 0xE0000000;
    else if ( s == 4 )
        shift = 0xF0000000;
    else if ( s == 5 )
        shift = 0xF8000000;
    else if ( s == 6 )
        shift = 0xFC000000;
    else if ( s == 7 )
        shift = 0xFE000000;
    else if ( s == 8 )
        shift = 0xFF000000;
    else if ( s == 9 )
        shift = 0xFF800000;
    else if ( s == 10 )
        shift = 0xFFC00000;
    else if ( s == 11 )
        shift = 0xFFE00000;
    else if ( s == 12 )
        shift = 0xFFF00000;
    else if ( s == 13 )
        shift = 0xFFF80000;
    else if ( s == 14 )
        shift = 0xFFFC0000;
    else if ( s == 15 )
        shift = 0xFFFE0000;
    
    return value << s | ( value & shift ) >> ( sizeof( longword ) * BBITS - s );
}

/**
    Iteration of the RIPEMD algorithm. 
    
    @param state HashState address
    @param datum longword to be manipulated
    @param shift number of bits to shift for the corresponding side & round
    @param noise noise metric add to "randomness" for the corresponding side & round
    @param f BitwiseFunction to use for the corresponding side & round
  */
static void hashIteration( HashState *state, longword datum, int shift, longword noise, BitwiseFunction f )
{
    state->A += f( state->B, state->C, state->D ) + datum + noise;
    
    state->A = rotateLeft( state->A, shift );
    state->C = rotateLeft( state->C, NUM_C_ROTATIONS );

    state->A += state->E;
    
    longword temp = state->A;
    state->A = state->E;
    state->E = state->D;
    state->D = state->C;
    state->C = state->B;
    state->B = temp;
}

/**
    Implements a round of the RIPEMD algorithm. Calls hashIteration().
    
    @param state HashState address
    @param data array of longwords to be manipulated
    @param perm array to be used in tandem with data for further randomization
    @param shift number of bits to shift for the corresponding side
    @param noise noise metric add to "randomness" for the corresponding side
    @param f BitwiseFunction to use for the corresponding side
  */
static void hashRound( HashState *state, longword data[ BLOCK_LONGWORDS ], int perm[ RIPE_ITERATIONS ], int shift[ RIPE_ITERATIONS ], longword noise, BitwiseFunction f )
{
    for ( int i = 0; i < RIPE_ITERATIONS; i++ )
        hashIteration( state, data[ perm[ i ] ], shift[ i ], noise, f );
}

/**
    Processes the given block of 64 bytes. The given state is the input state for 
    processing the block, and it’s used as the output state for returning the resulting 
    A, B, C, D and E values after the block is processed. Calls hashRound().
    
    @param state HashState address
    @param block array of longwords to be manipulated
  */
void hashBlock( HashState *state, byte block[ BLOCK_BYTES ] )
{
    int leftPerm0[ RIPE_ITERATIONS ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    int leftPerm1[ RIPE_ITERATIONS ] = { 7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8 };
    int leftPerm2[ RIPE_ITERATIONS ] = { 3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12 };
    int leftPerm3[ RIPE_ITERATIONS ] = { 1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2 };
    int leftPerm4[ RIPE_ITERATIONS ] = { 4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 };
    
    int rightPerm0[ RIPE_ITERATIONS ] = { 5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12 };
    int rightPerm1[ RIPE_ITERATIONS ] = { 6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2 };
    int rightPerm2[ RIPE_ITERATIONS ] = { 15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13 };
    int rightPerm3[ RIPE_ITERATIONS ] = { 8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14 };
    int rightPerm4[ RIPE_ITERATIONS ] = { 12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 };
    
    int leftShift0[ RIPE_ITERATIONS ] = { 11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8 };
    int leftShift1[ RIPE_ITERATIONS ] = { 7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12 };
    int leftShift2[ RIPE_ITERATIONS ] = { 11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5 };
    int leftShift3[ RIPE_ITERATIONS ] = { 11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12 };
    int leftShift4[ RIPE_ITERATIONS ] = { 9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 };
    
    int rightShift0[ RIPE_ITERATIONS ] = { 8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6 };
    int rightShift1[ RIPE_ITERATIONS ] = { 9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11 };
    int rightShift2[ RIPE_ITERATIONS ] = { 9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5 };
    int rightShift3[ RIPE_ITERATIONS ] = { 15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8 };
    int rightShift4[ RIPE_ITERATIONS ] = { 8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 };
    
    longword leftNoise0 = 0x00000000;
    longword leftNoise1 = 0x5A827999;
    longword leftNoise2 = 0x6ED9EBA1;
    longword leftNoise3 = 0x8F1BBCDC;
    longword leftNoise4 = 0xA953FD4E;
    
    longword rightNoise0 = 0x50A28BE6;
    longword rightNoise1 = 0x5C4DD124;
    longword rightNoise2 = 0x6D703EF3;
    longword rightNoise3 = 0x7A6D76E9;
    longword rightNoise4 = 0x00000000;
    
    BitwiseFunction leftBitwise[ NUM_BITWISE_FUNCTIONS ] = { bitwiseF0, bitwiseF1, bitwiseF2, bitwiseF3, bitwiseF4 };
    BitwiseFunction rightBitwise[ NUM_BITWISE_FUNCTIONS ] = { bitwiseF4, bitwiseF3, bitwiseF2, bitwiseF1, bitwiseF0 };
    
    longword longwordArray[ RIPE_ITERATIONS ] = {};
    
    for ( int i = 0; i < RIPE_ITERATIONS; i++ ) {
        for ( int j = 0; j < sizeof( longword ); j++ )
            longwordArray[ i ] = longwordArray[ i ] | ( block[ sizeof( longword ) * i + j ] << ( j * BBITS ) );
    }
    
    HashState leftSideRound = { state->A, state->B, state->C, state->D, state->E };
    HashState rightSideRound = { state->A, state->B, state->C, state->D, state->E };
            
    hashRound( &leftSideRound, longwordArray, leftPerm0, leftShift0, leftNoise0, leftBitwise[ 0 ] );
    hashRound( &leftSideRound, longwordArray, leftPerm1, leftShift1, leftNoise1, leftBitwise[ 1 ] );
    hashRound( &leftSideRound, longwordArray, leftPerm2, leftShift2, leftNoise2, leftBitwise[ 2 ] );
    hashRound( &leftSideRound, longwordArray, leftPerm3, leftShift3, leftNoise3, leftBitwise[ 3 ] );
    hashRound( &leftSideRound, longwordArray, leftPerm4, leftShift4, leftNoise4, leftBitwise[ 4 ] );
    
    hashRound( &rightSideRound, longwordArray, rightPerm0, rightShift0, rightNoise0, rightBitwise[ 0 ] );
    hashRound( &rightSideRound, longwordArray, rightPerm1, rightShift1, rightNoise1, rightBitwise[ 1 ] );
    hashRound( &rightSideRound, longwordArray, rightPerm2, rightShift2, rightNoise2, rightBitwise[ 2 ] );
    hashRound( &rightSideRound, longwordArray, rightPerm3, rightShift3, rightNoise3, rightBitwise[ 3 ] );
    hashRound( &rightSideRound, longwordArray, rightPerm4, rightShift4, rightNoise4, rightBitwise[ 4 ] );
    
    longword temp = state->A;
    state->A = state->B + leftSideRound.C + rightSideRound.D;
    state->B = state->C + leftSideRound.D + rightSideRound.E;
    state->C = state->D + leftSideRound.E + rightSideRound.A;
    state->D = state->E + leftSideRound.A + rightSideRound.B;
    state->E =   temp   + leftSideRound.B + rightSideRound.C;
}

// Put the following at the end of your implementation file.
// If we're compiling for unit tests, create wrappers for the otherwise
// private functions we'd like to be able to test.

#ifdef TESTABLE

longword bitwiseF0Wrapper( longword b, longword c, longword d )
{
  return bitwiseF0( b, c, d );
}

longword bitwiseF1Wrapper( longword b, longword c, longword d )
{
  return bitwiseF1( b, c, d );
}

longword bitwiseF2Wrapper( longword b, longword c, longword d )
{
  return bitwiseF2( b, c, d );
}

longword bitwiseF3Wrapper( longword b, longword c, longword d )
{
  return bitwiseF3( b, c, d );
}

longword bitwiseF4Wrapper( longword b, longword c, longword d )
{
  return bitwiseF4( b, c, d );
}

longword rotateLeftWrapper( longword value, int s )
{
  return rotateLeft( value, s );
}

void hashIterationWrapper( HashState *state,
                           longword datum,
                           int shift,
                           longword noise,
                           BitwiseFunction f )
{
  hashIteration( state, datum, shift, noise, f );
}

void hashRoundWrapper( HashState *state,
                       longword *block,
                       int perm[ RIPE_ITERATIONS ],
                       int shift[ RIPE_ITERATIONS ],
                       longword noise,
                       BitwiseFunction f )
{
  hashRound( state, block, perm, shift, noise, f );
}

#endif
