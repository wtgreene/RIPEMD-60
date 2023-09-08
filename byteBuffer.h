/** 
    @filename byteBuffer.h
    @author Will Greene (wgreene)
    
    Header file for byteBuffer.c
*/

#include <stdlib.h>
#include <stdio.h>


#define INITIAL_BUFFER_CAPACITY 5

#ifndef _BYTE_BUFFER_H_
#define _BYTE_BUFFER_H_

/** Number of bits in a byte */
#define BBITS 8

/** Type used as a byte. */
typedef unsigned char byte;

/** Representation for a file copied to memory, with some padding
    at the end. */
typedef struct {
  /** Array of bytes from the file (no null termination, so it's not a
      string). */
  byte *data;

  /** Number of currently used bytes in the data array. */
  unsigned int len;

  /** Capacity of the data array (it's typically over-allocated. */
  unsigned int cap;
} ByteBuffer;

#endif

/**
    Creates an instance of ByteBuffer and initializes its fields.
    
    @return ByteBuffer
  */
ByteBuffer *createBuffer();

/**
    Adds a byte to the end of the buffer.
    
    @param buffer ByteBuffer
    @param byte byte to add to buffer
  */
void addByte( ByteBuffer *buffer, byte b );

/**
    Frees all buffer memory.
  */
void freeBuffer( ByteBuffer *buffer );

/**
    Creates a ByteBuffer and reads the contents of the given file into the buffer.
    
    @param filename name of file to read from
    @return Bytebuffer ( with buffer data )
  */
ByteBuffer *readFile( const char *filename );
