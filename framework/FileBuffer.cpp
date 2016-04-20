#include "FileBuffer.h"


#include <iostream>
/*

from: http://www.cplusplus.com/reference/clibrary/cstdio/setbuf/

void setbuf ( FILE * stream, char * buffer );

Set stream buffer
Specifies the buffer to be used by the stream for I/O operations, which becomes a fully buffered stream. Or, alternatively, if buffer is a null pointer, buffering is disabled for the stream, which becomes an unbuffered stream.

This function should be called once the stream has been associated with an open file, but before any input or output operation is performed with it.

The buffer is assumed to be at least BUFSIZ bytes in size (see setvbuf to specify a size of the buffer).

A stream buffer is a block of data that acts as intermediary between the i/o operations and the physical file associated to the stream: For output buffers, data is output to the buffer until its maximum capacity is reached, then it is flushed (i.e.: all data is sent to the physical file at once and the buffer cleared). Likewise, input buffers are filled from the physical file, from which data is sent to the operations until exhausted, at which point new data is acquired from the file to fill the buffer again.

Stream buffers can be explicitly flushed by calling fflush. They are also automatically flushed by fclose and freopen, or when the program terminates normally.

A full buffered stream uses the entire size of the buffer as buffer whenever enough data is available (see setvbuf for other buffer modes).

All files are opened with a default allocated buffer (fully buffered) if they are known to not refer to an interactive device. This function can be used to either set a specific memory block to be used as buffer or to disable buffering for the stream.

The default streams stdin and stdout are fully buffered by default if they are known to not refer to an interactive device. Otherwise, they may either be line buffered or unbuffered by default, depending on the system and library implementation. The same is true for stderr, which is always either line buffered or unbuffered by default.

A call to this function is equivalent to calling setvbuf with _IOFBF as mode and BUFSIZ as size (when buffer is not a null pointer), or equivalent to calling it with _IONBF as mode (when it is a null pointer).
*/
namespace sys{
	
  FileBuffer::FileBuffer(const char* path)
    : m_path(path),
      m_file(0),
      m_buffer(0),
      m_bytes_r(0),
      m_bytes_w(0),
      m_fstat(),
      m_looping(false)
  {}


  FileBuffer::~FileBuffer(){
    if(0 != m_file)
      fclose(m_file);
    if(0 != m_buffer)
      delete [] m_buffer;
  }

  bool
  FileBuffer::isOpen(){
    return m_file != 0;
  }
	
  bool
  FileBuffer::open(const char* mode, unsigned buffersize){

    m_file = fopen(m_path.c_str(), mode);
    if(lstat(m_path.c_str(),&m_fstat) < 0)    
      return false;
    if(0 == m_file)
      return false;

    if(0 != buffersize){
      m_buffer = new char [buffersize];
      setvbuf (m_file, m_buffer, _IOFBF, buffersize);
    }
  
    std::cerr << "FileBuffer " << this << " opening " << m_path << std::endl;
    return true;
  }


  unsigned
  FileBuffer::calcNumFrames(unsigned framesize){
    return m_fstat.st_size/framesize;
  }

  void
  FileBuffer::close(){
    if(0 != m_file)
      fclose(m_file);
    m_file = 0;
  }

  void
  FileBuffer::rewindFile(){
    if(0 != m_file){
      rewind(m_file);
      m_bytes_r = 0;
      m_bytes_w = 0;
    }
  }
  

  void
  FileBuffer::setLooping(bool onoff){
    m_looping = onoff;
  }

  bool
  FileBuffer::getLooping(){
    return m_looping;
  }

  
  unsigned
  FileBuffer::read (void* buffer, unsigned numbytes){
    if(0 == m_file)
      return 0;

    if((m_bytes_r + numbytes) > m_fstat.st_size){
      if(m_looping){
	//std::cerr << "FileBuffer " << this << " rewinding " << m_path << " filesize is " << m_fstat.st_size << std::endl;
	rewind(m_file);
	m_bytes_r = 0;
      }
      else{
	return 0;
      }
    }

    unsigned bytes = fread(buffer, sizeof (unsigned char), numbytes, m_file);
    m_bytes_r += bytes;

    return bytes;
  }
  
  unsigned
  FileBuffer::write(void* buffer, unsigned numbytes){
    if(0 == m_file)
      return 0;
    unsigned bytes = fwrite(buffer, sizeof (unsigned char), numbytes, m_file);
    m_bytes_w += bytes;
    return bytes;
  }
  
  unsigned
  FileBuffer::numBytesR() const{
    return m_bytes_r;
  }

  unsigned
  FileBuffer::numBytesW() const{
    return m_bytes_w;
  }
  
}
