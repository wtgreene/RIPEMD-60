/** 
    @filename byteBuffer.c
    @author Will Greene (wgreene)
    
    Contains functions that read into, create, add bytes to, and free the buffer.
*/
#include "byteBuffer.h"


/**
    Creates an instance of ByteBuffer and initializes its fields.
    
    @return ByteBuffer
  */
ByteBuffer *createBuffer()
{
    ByteBuffer *buffer = (ByteBuffer *) malloc( sizeof( ByteBuffer ) );
    
    buffer->data = (byte *) malloc( sizeof( byte ) * INITIAL_BUFFER_CAPACITY );
    buffer->len = 0;
    buffer->cap = INITIAL_BUFFER_CAPACITY;
    
    return buffer;
}

/**
    Adds a byte to the end of the buffer.
    
    @param buffer ByteBuffer
    @param byte byte to add to buffer
  */
void addByte( ByteBuffer *buffer, byte b )
{
    if ( buffer->len >= buffer->cap ) {
        buffer->cap *= 2;
        buffer->data = realloc( buffer->data, sizeof( byte ) * buffer->cap );
    }
    
    buffer->data[ buffer->len ] = b;
    buffer->len++;
}

/**
    Frees all buffer memory.
  */
void freeBuffer( ByteBuffer *buffer )
{
    free( buffer->data );
    free( buffer );
}

/**
    Creates a ByteBuffer and reads the contents of the given file into the buffer.
    
    @param filename name of file to read from
    @return Bytebuffer ( with buffer data )
  */
ByteBuffer *readFile( const char *filename )
{
    ByteBuffer *buffer = createBuffer();
    
    FILE *fp = fopen( filename, "rb" );
    
    if ( !fp ) {
        return NULL;
    }
    
    int ch = fgetc( fp );
    
    while ( ch != EOF ) {
        byte b = (byte) ch;
        addByte( buffer, b );
        ch = fgetc( fp );
    }
    
    fclose( fp );
    
    return buffer;
}
