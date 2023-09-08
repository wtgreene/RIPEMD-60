/**
    @filename hash.c
    @author Will Greene (wgreene)
    
    Computes the RIPEMD-160 hash for a given input file.
  */
#include "byteBuffer.h"
#include "ripeMD.h"

/** number of executable arguments */
#define EXECUTABLE_ARG 1

/** number of required additional arguments */
#define REQUIRED_ADDITIONAL_ARGS 1

/** exit failure */
#define FAIL exit( EXIT_FAILURE );

/**
    Starting point. Reads file data into a buffer, then creates 64-byte blocks of 
    data to run through the RIPEMD algorithm. The end state of each block is used as the 
    beginning state of the next block. The final state is printed.
    
    @param argc number of arguments
    @param *argv[] array of pointers to command line arguments
    @return exit status
  */
int main( int argc, char *argv[] )
{
    // parameter error checking
    if ( argc != EXECUTABLE_ARG + REQUIRED_ADDITIONAL_ARGS ) {
        fprintf( stderr, "usage: hash <input-file>\n" );
        FAIL;
    }
    
    char *filename = argv[ 1 ];
    ByteBuffer *buffer = readFile( filename );
    
    if ( !buffer ) {
        perror( filename );
        FAIL;
    }
    
    padBuffer( buffer );
        
    HashState *hash = ( HashState * ) malloc( sizeof( HashState ) );
    initState( hash );
    
    int numBlocks = buffer->len / BLOCK_BYTES;
    byte block[ BLOCK_BYTES ] = {};
    
    for ( int i = 0; i < numBlocks; i++ ) {
    
        for ( int j = 0; j < BLOCK_BYTES; j++ )
            block[ j ] = buffer->data[ i * BLOCK_BYTES + j ];
            
        hashBlock( hash, block );
    }
        
    printHash( hash );
    
    free( hash );
    freeBuffer( buffer );
    return EXIT_SUCCESS;
}
