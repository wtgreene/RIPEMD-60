/** 
    @file testdriver.c

    This is a test driver for code in the byteBuffer and ripeMD components.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "byteBuffer.h"
#include "ripeMD.h"

/** Total number or tests we tried. */
static int totalTests = 0;

/** Number of test cases passed. */
static int passedTests = 0;

/** Number of tests we should have, if they're all turned on. */
#define EXPECTED_TOTAL 97

/** Macro to check the condition on a test case, keep counts of
    passed/failed tests and report a message if the test fails. */
#define TestCase( conditional ) {\
  totalTests += 1; \
  if ( conditional ) { \
    passedTests += 1; \
  } else { \
    printf( "**** Failed unit test on line %d of %s\n", __LINE__, __FILE__ );    \
  } \
}

int main()
{
  // As you finish parts of your implementation, move this directive
  // down past the blocks of code below.  That will enable tests of
  // various functions you're expected to implement.

  
  ////////////////////////////////////////////////////////////////////////
  // Tests for byteBuffer component.
  ////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // Test createBuffer()
  
  {
    ByteBuffer *buffer = createBuffer();
    
    TestCase( buffer != NULL );
    TestCase( buffer->len == 0 );
    TestCase( buffer->cap == 5 );
  
    freeBuffer( buffer );
  }
    
  ////////////////////////////////////////////////////////////////////////
  // Test addByte()
  
  {
    ByteBuffer *buffer = createBuffer();

    // Insert some characters and make sure the buffer gets updated.
    addByte( buffer, 'v' );
    TestCase( buffer->len == 1 );
    TestCase( buffer->data[ 0 ] == 'v' );
    
    addByte( buffer, 'w' );
    TestCase( buffer->len == 2 );
    TestCase( buffer->data[ 1 ] == 'w' );
    
    addByte( buffer, 'x' );
    TestCase( buffer->len == 3 );
    TestCase( buffer->data[ 2 ] == 'x' );

    addByte( buffer, 'y' );
    TestCase( buffer->len == 4 );
    TestCase( buffer->data[ 3 ] == 'y' );

    addByte( buffer, 'z' );
    TestCase( buffer->len == 5 );
    TestCase( buffer->data[ 4 ] == 'z' );

    // Shouldn't have needed to grow yet.
    TestCase( buffer->cap == 5 );

    // One more character should force the buffer to double
    // its capacity.
    addByte( buffer, '!' );
    
    TestCase( buffer->len == 6 );
    TestCase( buffer->cap >= 10 );
    TestCase( buffer->data[ 5 ] == '!' );
    
    // Try adding a bunch of characters.
    for ( int i = 0; i < 74; i++ )
      addByte( buffer, '.' );
    TestCase( buffer->len == 80 );
    TestCase( buffer->data[ 79 ] == '.' );
    TestCase( buffer->cap >= 80 );

    freeBuffer( buffer );
  }

  ////////////////////////////////////////////////////////////////////////
  // Test readFile()
  
  {
    ByteBuffer *buffer = readFile( "input-01.txt" );
    TestCase( buffer != NULL );
    
    TestCase( buffer->len == 28 );
    // Make sure the buffer contains the right sequence of bytes.
    TestCase( memcmp( buffer->data,
                      "This is a short input file.\n",
                      28 ) == 0 );
    
    freeBuffer( buffer );
  }

  {
    ByteBuffer *buffer = readFile( "input-03.txt" );
    
    // Make sure it looks like we got the whole input file.
    TestCase( buffer->len == 909 );
    TestCase( buffer->data[ 907 ] == '.' );

    // Make sure the buffer still doubles capacity if we add more
    // bytes to it.
    int originalLen = buffer->len;
    int originalCap = buffer->cap;
    for ( int i = 0; i < originalLen; i++ )
      addByte( buffer, 'x' );

    // Capacity should have doubled.
    TestCase( buffer->cap == originalCap * 2 );
    
    freeBuffer( buffer );
  }

  {
    // Try the binary input file.
    ByteBuffer *buffer = readFile( "input-05.bin" );
    
    // Make sure it looks like we got the whole input file.
    TestCase( buffer->len == 11328 );
    TestCase( buffer->data[ 11327 ] == 0x1b );
    
    freeBuffer( buffer );
  }
  
  {
    // Try a file that doesn't exist.
    ByteBuffer *buffer = readFile( "no-input-file.txt" );
    TestCase( buffer == NULL );
  }
  
  ////////////////////////////////////////////////////////////////////////
  // Tests for the ripeMD component
  ////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // Test the five versions of the bitwise F function,
  
  {
    // Test with some small values.
    unsigned int b = 0x00000001;
    unsigned int c = 0x00000003;
    unsigned int d = 0x00000007;
    unsigned int result;
    
    result = bitwiseF0Wrapper( b, c, d );
    TestCase( result == 0x00000005 );
    
    result = bitwiseF1Wrapper( b, c, d );
    TestCase( result == 0x00000007 );
    
    result = bitwiseF2Wrapper( b, c, d );
    TestCase( result == 0xFFFFFFFA );
    
    result = bitwiseF3Wrapper( b, c, d );
    TestCase( result == 0x00000001 );

    result = bitwiseF4Wrapper( b, c, d );
    TestCase( result == 0xFFFFFFFA );
  }

  {
    unsigned int b = 0xAB659F92;
    unsigned int c = 0xE4C6A120;
    unsigned int d = 0xC0D29093;
    unsigned int result;

    result = bitwiseF0Wrapper( b, c, d );
    TestCase( result == 0x8F71AE21 );

    result = bitwiseF1Wrapper( b, c, d );
    TestCase( result == 0xE0D68101 );
    
    result = bitwiseF2Wrapper( b, c, d );
    TestCase( result == 0x7BAF4F4C );
    
    result = bitwiseF3Wrapper( b, c, d );
    TestCase( result == 0xA444B1B2 );
    
    result = bitwiseF4Wrapper( b, c, d );
    TestCase( result == 0x548A70FE );
  }

  ////////////////////////////////////////////////////////////////////////
  // Test the rotateLeft() function.
  
  {
    unsigned int result;
    
    result = rotateLeftWrapper( 0x00000001, 1 );
    TestCase( result == 0x00000002 );
    
    result = rotateLeftWrapper( 0x80000000, 1 );
    TestCase( result == 0x00000001 );
    
    result = rotateLeftWrapper( 0x6223818F, 3 );
    TestCase( result == 0x111C0C7B );

    result = rotateLeftWrapper( 0x9C913C27, 11 );
    TestCase( result == 0x89E13CE4 );
  }

  ////////////////////////////////////////////////////////////////////////
  // Test the initState() function.
  
  {
    HashState state;
    
    initState( &state );
    TestCase( state.A == 0x67452301 );
    TestCase( state.B == 0xEFCDAB89 );
    TestCase( state.C == 0x98BADCFE );
    TestCase( state.D == 0x10325476 );
    TestCase( state.E == 0xC3D2E1F0 );
  }

  ////////////////////////////////////////////////////////////////////////
  // Test the padBuffer() function.
  
  {
    // Make a buffer and put some characters into it.
    ByteBuffer *buffer = createBuffer();

    char *str = "testing 1, 2, 3...";
    for ( int i = 0; str[ i ]; i++ )
      addByte( buffer, str[ i ] );
    padBuffer( buffer );
      
    TestCase( buffer->len == 64 );

    // This should be the buffer, padded with a 0x80, then enough zero bytes
    // to make it 8 bytes less than a multiple of 64 bytes in length.
    // Then, the last 8 bytes are the number of bits in the original
    // message, written out in LSB-First order.
    unsigned char expected[ 64 ] =
      { 0x74, 0x65, 0x73, 0x74, 0x69, 0x6E, 0x67, 0x20,
        0x31, 0x2C, 0x20, 0x32, 0x2C, 0x20, 0x33, 0x2E,
        0x2E, 0x2E, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    
    // Check each part of the padded buffer separately.  This might
    // help in finding errors.

    // Check the original 18 bytes
    TestCase( memcmp( buffer->data, expected, 18 ) == 0 );
    
    // Check the original 0x80 added to the end.
    TestCase( buffer->data[ 18 ] == 0x80 );
    
    // Check the zeros added up to 8 bytes from the end.
    TestCase( memcmp( buffer->data + 19, expected + 19, 37 ) == 0 );
    
    // Check the size field at the end.
    TestCase( memcmp( buffer->data + 56, expected + 56, 8 ) == 0 );
    
    freeBuffer( buffer );
  }
    
  {
    ByteBuffer *buffer = createBuffer();

    // Put some characters in the buffer.
    char *str = "This is a longer test string, chosen to be exactly 64 bytes long";
    for ( int i = 0; str[ i ]; i++ )
      addByte( buffer, str[ i ] );
    padBuffer( buffer );
      
    TestCase( buffer->len == 128 );

    // Same as above, but here the message gets padded to fill two 64-byte
    // blocks.
    unsigned char expected[ 128 ] =
      { 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20,
        0x61, 0x20, 0x6C, 0x6F, 0x6E, 0x67, 0x65, 0x72,
        0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x73, 0x74,
        0x72, 0x69, 0x6E, 0x67, 0x2C, 0x20, 0x63, 0x68,
        0x6F, 0x73, 0x65, 0x6E, 0x20, 0x74, 0x6F, 0x20,
        0x62, 0x65, 0x20, 0x65, 0x78, 0x61, 0x63, 0x74,
        0x6C, 0x79, 0x20, 0x36, 0x34, 0x20, 0x62, 0x79,
        0x74, 0x65, 0x73, 0x20, 0x6C, 0x6F, 0x6E, 0x67,
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      
    TestCase( memcmp( buffer->data, expected, 128 ) == 0 );
    freeBuffer( buffer );
  }
 
  {
    ByteBuffer *buffer = createBuffer();

    // Put some characters in the buffer.
    char *str = "This test is even longer.  It want to make it at least two "
      "blocks long, and, with padding, I'd like it to take three. "
      "Am I going to have to think of something else to say?  Guess not.";
    for ( int i = 0; str[ i ]; i++ )
      addByte( buffer, str[ i ] );
    padBuffer( buffer );
      
    TestCase( buffer->len == 192 );

    // Same as before, but now the message is a little bit longer.
    unsigned char expected[ 192 ] =
      { 0x54, 0x68, 0x69, 0x73, 0x20, 0x74, 0x65, 0x73,
        0x74, 0x20, 0x69, 0x73, 0x20, 0x65, 0x76, 0x65,
        0x6E, 0x20, 0x6C, 0x6F, 0x6E, 0x67, 0x65, 0x72,
        0x2E, 0x20, 0x20, 0x49, 0x74, 0x20, 0x77, 0x61,
        0x6E, 0x74, 0x20, 0x74, 0x6F, 0x20, 0x6D, 0x61,
        0x6B, 0x65, 0x20, 0x69, 0x74, 0x20, 0x61, 0x74,
        0x20, 0x6C, 0x65, 0x61, 0x73, 0x74, 0x20, 0x74,
        0x77, 0x6F, 0x20, 0x62, 0x6C, 0x6F, 0x63, 0x6B,
        0x73, 0x20, 0x6C, 0x6F, 0x6E, 0x67, 0x2C, 0x20,
        0x61, 0x6E, 0x64, 0x2C, 0x20, 0x77, 0x69, 0x74,
        0x68, 0x20, 0x70, 0x61, 0x64, 0x64, 0x69, 0x6E,
        0x67, 0x2C, 0x20, 0x49, 0x27, 0x64, 0x20, 0x6C,
        0x69, 0x6B, 0x65, 0x20, 0x69, 0x74, 0x20, 0x74,
        0x6F, 0x20, 0x74, 0x61, 0x6B, 0x65, 0x20, 0x74,
        0x68, 0x72, 0x65, 0x65, 0x2E, 0x20, 0x41, 0x6D,
        0x20, 0x49, 0x20, 0x67, 0x6F, 0x69, 0x6E, 0x67,
        0x20, 0x74, 0x6F, 0x20, 0x68, 0x61, 0x76, 0x65,
        0x20, 0x74, 0x6F, 0x20, 0x74, 0x68, 0x69, 0x6E,
        0x6B, 0x20, 0x6F, 0x66, 0x20, 0x73, 0x6F, 0x6D,
        0x65, 0x74, 0x68, 0x69, 0x6E, 0x67, 0x20, 0x65,
        0x6C, 0x73, 0x65, 0x20, 0x74, 0x6F, 0x20, 0x73,
        0x61, 0x79, 0x3F, 0x20, 0x20, 0x47, 0x75, 0x65,
        0x73, 0x73, 0x20, 0x6E, 0x6F, 0x74, 0x2E, 0x80,
        0xB8, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
      
    TestCase( memcmp( buffer->data, expected, 192 ) == 0 );
    freeBuffer( buffer );
  }

  ////////////////////////////////////////////////////////////////////////
  // Test the hashIteration() function
  
  {
    // Test for hashIteration(), equivalent to left-side round 0,
    // iteration 1 (both counting from zero) for test input-01.txt.
    
    HashState state = { 0xC3D2E1F0, 0x614593FF, 0xEFCDAB89, 0xEB73FA62,
                        0x10325476 };
    
    hashIterationWrapper( &state, 0x20736920, 14,
                          0x00000000, bitwiseF0Wrapper );
    
    TestCase( state.A == 0x10325476 );
    TestCase( state.B == 0x937B6706 );
    TestCase( state.C == 0x614593FF );
    TestCase( state.D == 0x36AE27BF );
    TestCase( state.E == 0xEB73FA62 );
  }
  
  {
    // Test for hashIteration(), equivalent to right-side round 1,
    // iteration 0 (both counting from zero) for test input-01.txt.
    
    HashState state = { 0xFCE2D1B1, 0x2FDAAC8E, 0x7F1EF9E5, 0xD8435C5D,
                        0x8F89651B };
    
    hashIterationWrapper( &state, 0x0A2E656C, 9,
                          0x5C4DD124U, bitwiseF3Wrapper );
    
    TestCase( state.A == 0x8F89651B );
    TestCase( state.B == 0x0AF54040 );
    TestCase( state.C == 0x2FDAAC8E );
    TestCase( state.D == 0x7BE795FC );
    TestCase( state.E == 0xD8435C5D );
  }
  
  {
    // Test for hashIteration(), equivalent to left-side round 2,
    // iteration 7 (both counting from zero) for test input-01.txt.
    
    HashState state = { 0xEA4852C0, 0x110103C7, 0x98B2A6C5, 0x5C0D3BF2,
                        0x60C03D57 };
    
    hashIterationWrapper( &state, 0x20736920, 15,
                          0x6ED9EBA1, bitwiseF2Wrapper );
    
    TestCase( state.A == 0x60C03D57 );
    TestCase( state.B == 0x64878FC2 );
    TestCase( state.C == 0x110103C7 );
    TestCase( state.D == 0xCA9B1662 );
    TestCase( state.E == 0x5C0D3BF2 );
  }
  
  {
    // Test for hashIteration(), equivalent to right-side round 3,
    // iteration 15 (both counting from zero) for test input-01.txt.
    
    HashState state = { 0x9D8CFDC8, 0xE077A148, 0x88AA536C, 0x5E9B703B,
                        0x0AE81E23 };
    
    hashIterationWrapper( &state, 0x000000E0, 8,
                          0x7A6D76E9, bitwiseF1Wrapper );
    
    TestCase( state.A == 0x0AE81E23 );
    TestCase( state.B == 0xAFAF2AD9 );
    TestCase( state.C == 0xE077A148 );
    TestCase( state.D == 0xA94DB222 );
    TestCase( state.E == 0x5E9B703B );
  }
  
  ////////////////////////////////////////////////////////////////////////
  // Test the hashRound() function
  
  {
    // A random starting hash state.
    HashState state = { 0x7573FB88, 0xC0EE5744, 0x2F754970, 0x507ED763,
                        0xCD487950 };
    
    // A random block to process.
    unsigned int block[] =
      { 0x080B2495, 0x4FBBA436, 0x6490226E, 0x46C425C6,
        0x3FE1D3BD, 0x98066F3B, 0x243443F1, 0xA5C32841,
        0x17A38BB8, 0x6E4CAB6D, 0xCD91EB9C, 0x96C4F350,
        0x71BB004B, 0xC5E969FC, 0xBC131BD4, 0x90E526DE };
      
    // Just access the block elements in order.
    int perm[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    // Always shift by zero for this test.
    int shift[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    // Apply a round with a noise value of zero and using bitwise
    // function 4.
    hashRoundWrapper( &state, block, perm, shift, 0x00000000,
                      bitwiseF4Wrapper );
  
    TestCase( state.A == 0xAF5F8346 );
    TestCase( state.B == 0x18CE87F6 );
    TestCase( state.C == 0x869559AF );
    TestCase( state.D == 0x9622FC56 );
    TestCase( state.E == 0x1CB7E121 );
  }

  {
    // A random starting hash state.
    HashState state = { 0x1AA0D28C, 0x81F943FD, 0x8926C3E7, 0xF545EAAC,
                        0x6BF74A2B };
    
    // A random block to process.
    longword block[] =
      { 0x8D188A93, 0xD26DFD20, 0xBEC5D377, 0x0FD7ABB4,
        0x5070D31A, 0x4172969C, 0x180C1ED0, 0x17653891,
        0x7434CB9D, 0xBF4C4744, 0x8B338358, 0x514D9B93,
        0x0FCC16BB, 0x34D90C53, 0x184535D6, 0x7E0A6C74 };
      
    // Random order for accessing block elements.
    int perm[] = { 4, 10, 11, 15, 14, 5, 13, 1, 6, 9, 3, 7, 8, 2, 0, 12 };

    // Random shift values
    int shift[] = { 9, 14, 15, 7, 5, 15, 13, 10, 5, 9, 11, 5, 15, 8, 15, 15 };

    // Apply a round with a random noise value and using bitwise
    // function 0.
    hashRoundWrapper( &state, block, perm, shift, 0xD52E4431,
                      bitwiseF0Wrapper );
  
    TestCase( state.A == 0x3C27EB3F );
    TestCase( state.B == 0x45B0E223 );
    TestCase( state.C == 0x5F477F1E );
    TestCase( state.D == 0xA2BA24D6 );
    TestCase( state.E == 0x28752D9C );
  }

  ////////////////////////////////////////////////////////////////////////
  // Test the hashBlock() function
  
  {
    // A random starting hash state.
    HashState state = { 0x61975820, 0x20DF29DA, 0x1BA7D460, 0x304626E9,
                        0x0372D2E9 };

    // Random-looking values for a data block.
    byte data[ BLOCK_BYTES ] =
      { 0x23, 0x40, 0x32, 0xF9, 0xF4, 0x0B, 0xF3, 0x71,
        0x60, 0x4E, 0x65, 0xDB, 0x31, 0x16, 0x89, 0x1E,
        0x4C, 0x33, 0xC4, 0x8E, 0x6D, 0xC4, 0x6D, 0x57,
        0xA9, 0x9B, 0xD5, 0x59, 0x08, 0xC9, 0x4A, 0xE3,
        0xFD, 0x35, 0x1D, 0x66, 0xDB, 0x9C, 0x52, 0x51,
        0xF1, 0x1C, 0x04, 0xEC, 0x1D, 0x6A, 0x86, 0x95,
        0xB6, 0x05, 0x8C, 0xAB, 0x88, 0x6B, 0x57, 0x4F,
        0x8F, 0x67, 0x90, 0x56, 0xC8, 0x81, 0x95, 0x1B };

    hashBlock( &state, data );
      
    TestCase( state.A == 0xF56C82A5 );
    TestCase( state.B == 0x1E4B4FCF );
    TestCase( state.C == 0x1B509AE4 );
    TestCase( state.D == 0xFBA0BB5D );
    TestCase( state.E == 0xB9D73744 );
  }

  {
    // A random starting hash state.
    HashState state = { 0xDEAFE6DE, 0xF6A49937, 0xCD7E623B, 0x323E682D,
                        0x851D6872 };

    // Random-looking values for a data block.
    byte data[ BLOCK_BYTES ] =
      { 0xF2, 0x08, 0x50, 0x64, 0xB1, 0xD0, 0x58, 0x9E,
        0xDA, 0x71, 0x6D, 0x74, 0xAE, 0xDE, 0x97, 0xD9,
        0x89, 0x60, 0xFA, 0xDD, 0xBB, 0xAC, 0xFE, 0x38,
        0x39, 0x9E, 0x91, 0xED, 0x2B, 0x7F, 0x03, 0x3E,
        0xBF, 0x1E, 0x90, 0xA3, 0x2F, 0x8E, 0xE2, 0xDB,
        0x4E, 0xDE, 0x4E, 0xB9, 0x91, 0x13, 0x30, 0xED,
        0x86, 0xF0, 0x9E, 0x76, 0x84, 0x40, 0x20, 0x95,
        0x27, 0x8B, 0x2D, 0x6B, 0x6E, 0x68, 0x55, 0x14, };

    hashBlock( &state, data );
      
    TestCase( state.A == 0xEDC96DF2 );
    TestCase( state.B == 0x3BEAC4E1 );
    TestCase( state.C == 0x1DF7F50F );
    TestCase( state.D == 0x5135AFAC );
    TestCase( state.E == 0x639BEE89 );
  }

  printf( "You passed %d / %d unit tests\n", passedTests, totalTests );

  if ( totalTests != EXPECTED_TOTAL )
    printf( "The full test driver should have %d tests\n", EXPECTED_TOTAL );

  if ( passedTests != EXPECTED_TOTAL )
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}
