
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



#ifndef GLOOST_TEXCOORD_H
#define GLOOST_TEXCOORD_H


/// if you write textcoords to a file, what is your float precision
#define DEFAULT_PRECISION   12
#define MAX_PRECISION     1000


/// cpp includes
#include <fstream>
#include <string>



namespace gloost
{


  /// Texture coordinate

class Texcoord
{

 public:

  /// constructor
  Texcoord();
  Texcoord(const float* data);
  Texcoord(const float& u, const float& v, const float& w);

  /// constructor reads obj file texCoord declaration like "vt 0.0238753 0.0416692"
  Texcoord(std::ifstream& in);

  /// destructor
  ~Texcoord();

  void dump() const;

  void write(std::ofstream& out) const;

  static void setPrecision(unsigned int p);

  static void setFP(std::string fp);

  /// indexing
  float& operator[] (unsigned int);
  const float& operator[] (unsigned int) const;

  /// get pointer to an array
  const float* data() const;
  float* data();


 private:

  float _data[3];
  static unsigned int precision;
  static std::string fp;
};


} // namespace gloost

#endif // #ifndef GLOOST
