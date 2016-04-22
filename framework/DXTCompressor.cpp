#include "DXTCompressor.h"


#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <iostream>

using fastdxt::byte;

namespace mvt{

  DXTCompressor::DXTCompressor()
    :
    _fc(0),
    _timer(),
    _width(0),
    _height(0),
    _type(0),
    _storage(0),
    _compressed_buff_sub_size(0),
    _rgba_buff(0),
    _rgba_buff_sub(),
    _compressed_buff(0),
    _compressed_buff_sub()
  {}

  DXTCompressor::~DXTCompressor(){
    free(_rgba_buff);
    free(_compressed_buff);

    for(unsigned tid = 0; tid != DXTCOMPRESSOR_NUMTHREADS; ++tid){
      free(_rgba_buff_sub[tid]);
      free(_compressed_buff_sub[tid]);


    }

  }

  unsigned
  DXTCompressor::init(unsigned width, unsigned height, unsigned type){
    // type is not used until now
    _width = width;
    _height = height;
    _type = type;


    const unsigned rgba_size = _width*_height*4;
    const unsigned compressed_size = _width*_height*4/8;
    _rgba_buff =       (byte*) memalign(16, rgba_size);
    _compressed_buff = (byte*) memalign(16, compressed_size);
    _storage = fastdxt::CompressDXT(_rgba_buff, _compressed_buff, _width, _height, _type, /*numthreads*/1);

    std::cerr << "DXTCompressor::init _storage size: "  <<  _storage << std::endl;




    _height_sub = _height / DXTCOMPRESSOR_NUMTHREADS;
    _compressed_buff_sub_size = _width*_height_sub*4/8;
    for(unsigned tid = 0; tid != DXTCOMPRESSOR_NUMTHREADS; ++tid){
      _rgba_buff_sub[tid]       = (byte*) memalign(16, _width*_height_sub*4);
      _compressed_buff_sub[tid] = (byte*) memalign(16, _width*_height_sub*4/8);

      std::cerr << "DXTCompressor::init _rgba_buff_sub[tid]: " << _rgba_buff_sub[tid] << " : " << _width*_height_sub*4 << std::endl;
      std::cerr << "DXTCompressor::init _compressed_buff_sub[tid]: " << _compressed_buff_sub[tid] << " : " << 1.0 * _width*_height_sub*4/8 << std::endl;
      std::cerr << "DXTCompressor::init testsize: " << fastdxt::CompressDXT(_rgba_buff_sub[tid], _compressed_buff_sub[tid], _width, _height_sub, _type, /*numthreads*/1) << std::endl;


    }




    return _storage;
  }

  unsigned
  DXTCompressor::getStorageSize(){
    return _storage;
  }


  unsigned char*
  DXTCompressor::compress(unsigned char* buff, bool resetbg){

#if 0    
    _timer.start();
#endif

#if 1
    boost::thread_group threadGroup;
    for (unsigned tid = 0; tid != DXTCOMPRESSOR_NUMTHREADS; ++tid){

      unsigned char* buff_tmp = &buff[(tid * (_width*_height*3/DXTCOMPRESSOR_NUMTHREADS))];
      threadGroup.create_thread(boost::bind(&DXTCompressor::docompress,this,tid, buff_tmp, resetbg));
    }
    threadGroup.join_all();


#else

    unsigned i = 0;
    unsigned o = 0;
    for(unsigned y = 0; y < _height; ++y){
      for(unsigned x = 0; x < _width; ++x){
	_rgba_buff[o] = buff[i];
	++o;++i;
	_rgba_buff[o] = buff[i];
	++o;++i;
	_rgba_buff[o] = buff[i];
	++o;++i;
	_rgba_buff[o] = 255;
	++o;
      }
    }
    _storage = CompressDXT(_rgba_buff, _compressed_buff, _width, _height, _type, /*numthreads*/4);
#endif

    



#if 0
    _timer.stop();

    if(_fc++ % 100 == 0)
    std::cerr << "compression took " << _timer.get().msec() << " msec" << " compressed " << _width * _height * 3 << " bytes to " << (100.0 * _storage) / (_width * _height * 3) << "%" << std::endl;
#endif


    return _compressed_buff;
  }

  unsigned
  DXTCompressor::getType(){
    return _type;
  }


  void
  DXTCompressor::docompress(unsigned tid, unsigned char* buff, bool resetbg){

#if 0
    if(resetbg){

      std::cerr << this << " DXTCompressor::docompress resetting background from thread " << (unsigned) tid << std::endl;

      unsigned i = 0;
      unsigned o = 0;
      for(unsigned y = 0; y < _height_sub; ++y){
	for(unsigned x = 0; x < _width; ++x){

	  unsigned char r = buff[i];++i;
	  unsigned char g = buff[i];++i;
	  unsigned char b = buff[i];++i;
	  const unsigned index = y * _width + x;
	  _bgd[tid]->setBackground(index, r, g, b);

	  _rgba_buff_sub[tid][o] = r; ++o;
	  _rgba_buff_sub[tid][o] = g; ++o;
	  _rgba_buff_sub[tid][o] = b; ++o;
	  _rgba_buff_sub[tid][o] = 255; ++o;

	}
      }
    }
    else{
      unsigned i = 0;
      unsigned o = 0;
      for(unsigned y = 0; y < _height_sub; ++y){
	for(unsigned x = 0; x < _width; ++x){

	  unsigned char r = buff[i];
	  ++i;
	  unsigned char g = buff[i];
	  ++i;
	  unsigned char b = buff[i];
	  ++i;
	  const unsigned index = y * _width + x;
	  const bool bg = _bgd[tid]->detectBackground(index, r, g, b);
	  if(bg){
	    _rgba_buff_sub[tid][o] = 0;
	    ++o;
	    _rgba_buff_sub[tid][o] = 0;
	    ++o;
	    _rgba_buff_sub[tid][o] = 0;
	    ++o;
	    _rgba_buff_sub[tid][o] = 255;
	    ++o;
	  }
	  else{
	    _rgba_buff_sub[tid][o] = r;
	    ++o;
	    _rgba_buff_sub[tid][o] = g;
	    ++o;
	    _rgba_buff_sub[tid][o] = b;
	    ++o;
	    _rgba_buff_sub[tid][o] = 255;
	    ++o;
	  }

	}
      }
    }

#else
    // original
    unsigned i = 0;
    unsigned o = 0;
    for(unsigned y = 0; y < _height_sub; ++y){
      for(unsigned x = 0; x < _width; ++x){
	_rgba_buff_sub[tid][o] = buff[i];
	++o;++i;
	_rgba_buff_sub[tid][o] = buff[i];
	++o;++i;
	_rgba_buff_sub[tid][o] = buff[i];
	++o;++i;
	_rgba_buff_sub[tid][o] = 255;
	++o;
      }
    }
#endif

    unsigned s = fastdxt::CompressDXT(_rgba_buff_sub[tid], _compressed_buff_sub[tid], _width, _height_sub, _type, /*numthreads*/1);


    memcpy( (_compressed_buff + (tid * _compressed_buff_sub_size)), _compressed_buff_sub[tid], s/*_compressed_buff_sub_size*/);

  }
}
