
/*
                       ___                            __
                      /\_ \                          /\ \__
                   __ \//\ \     ___     ___     ____\ \  _\
                 /'_ `\ \ \ \   / __`\  / __`\  /  __\\ \ \/
                /\ \ \ \ \_\ \_/\ \ \ \/\ \ \ \/\__   \\ \ \_
                \ \____ \/\____\ \____/\ \____/\/\____/ \ \__\
                 \/___/\ \/____/\/___/  \/___/  \/___/   \/__/
                   /\____/
                   \_/__/

                   OpenGL framework for fast demo programming

                             http://www.gloost.org

    This file is part of the gloost framework. You can use it in parts or as
       whole under the terms of the GPL (http://www.gnu.org/licenses/#GPL).

            gloost is being created by Felix Weiﬂig and Stephan Beck

     Felix Weiﬂig (thesleeper@gmx.net), Stephan Beck (stephan@pixelstars.de)
*/



// gloost system includes
#include <Texcoord.h>
#include <gloostHelper.h>


// cpp includes
#include <stdio.h>
#include <iostream>
#include <stdlib.h>



namespace gloost
{


////////////////////////////////////////////////////////////////////////////////


/*static*/ unsigned int Texcoord::precision = DEFAULT_PRECISION;
/*static*/ std::string  Texcoord::fp = "f";


////////////////////////////////////////////////////////////////////////////////


Texcoord::Texcoord()
  : _data()
{
  _data[0] = 0.0f;
  _data[1] = 0.0f;
  _data[2] = 0.0f;
}


////////////////////////////////////////////////////////////////////////////////


Texcoord::Texcoord(const float* data)
  : _data()
{
  _data[0] = data[0];
  _data[1] = data[1];
  _data[2] = data[2];
}


////////////////////////////////////////////////////////////////////////////////


Texcoord::Texcoord(const float& u, const float& v, const float& w)
  : _data()
{
  _data[0] = u;
  _data[1] = v;
  _data[2] = w;
}


////////////////////////////////////////////////////////////////////////////////


Texcoord::Texcoord(std::ifstream& in)
  : _data()
{
  in >> _data[0];
  in >> _data[1];
}


////////////////////////////////////////////////////////////////////////////////


Texcoord::~Texcoord()
{}


////////////////////////////////////////////////////////////////////////////////


void
Texcoord::dump() const
{
  std::cerr << "texcoord: "
	    << _data[0]
	    << ", "
	    << _data[1]
	    << ", "
	    << _data[2]
	    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////


void
Texcoord::write(std::ofstream& out) const
{

  char buffer[ 3*MAX_PRECISION + 30];
  std::string format = \
    "%." + gloost::toString(precision) + "f" + fp+ ", " +\
    "%." + gloost::toString(precision) + "f" + fp+ ", " +\
    "%." + gloost::toString(precision) + "f" + fp+ ",";


  if(!sprintf(buffer,format.c_str(),_data[0],_data[1],_data[2]))
  {
    std::cerr << "ERROR in" << __FILE__ << __LINE__ << " ...exiting!" << std::endl;
    exit(-1);
  }

  out << "\t" << buffer << std::endl;

}


////////////////////////////////////////////////////////////////////////////////


/*static*/
void
Texcoord::setPrecision(unsigned int a)
{
  precision = a;
}


////////////////////////////////////////////////////////////////////////////////


/*static*/
void
Texcoord::setFP(std::string a)
{
  fp = a;
}


////////////////////////////////////////////////////////////////////////////////


  /// indexing

float&
Texcoord::operator[](unsigned int a)
{
  return (2 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


  /// indexing

const float&
Texcoord::operator[] (unsigned int a) const
{
  return (2 < a) ? _data[0] : _data[a];
}


////////////////////////////////////////////////////////////////////////////////


 /// get pointer to an array

const float*
Texcoord::data() const
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////


 /// get pointer to an array

float*
Texcoord::data()
{
  return _data;
}


////////////////////////////////////////////////////////////////////////////////



} // namespace gloost
