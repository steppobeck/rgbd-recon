
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



#ifndef GLOOST_HELPER_H
#define GLOOST_HELPER_H



/// cpp includes
#include <string>
#include <sstream>



namespace gloost
{



///////////////////////////////////////////////////////////////////////////////


  /// convert something into a string

template <class T>
inline std::string
toString(T value)
{
  std::ostringstream stream;
  stream << value;
  return stream.str();
}



///////////////////////////////////////////////////////////////////////////////


  /// convert something into a string

inline std::string
floatToString(float value, unsigned int precision = 2)
{
  std::ostringstream stream;

  stream.precision(precision);
  stream.fill('0');

  stream << value;
  return stream.str();
}


///////////////////////////////////////////////////////////////////////////////


  /// return the basename of a file within a path ( "folder/subfolder/someimage.jpg" -> "someimage" )

inline
std::string
pathToBasename(const std::string& filepath)
{
  // find last dot
  int dotPos   = filepath.rfind('.');

  // find last slash
  int slashPos = filepath.rfind('/');

  std::string filename = filepath.substr( slashPos+1, dotPos-slashPos-1);
  return filename;
}


///////////////////////////////////////////////////////////////////////////////


  /// return the basename of a path ( "folder/subfolder/someimage.jpg" -> "someimage.jpg" )

inline
std::string
pathToFilename(const std::string& filepath)
{
  // find last slash
  int slashPos = filepath.rfind('/');

  std::string basename = filepath.substr( slashPos+1, filepath.length()-slashPos-1);
  return basename;
}


///////////////////////////////////////////////////////////////////////////////


  /// return the path of a file without the filename of a path ( "folder/subfolder/someimage.jpg" -> "folder/subfolder" )

inline std::string pathToBasePath(const std::string& filepath)
{
  // find last slash
  int slashPos = filepath.rfind('/');

  return filepath.substr( 0, slashPos+1);
}


///////////////////////////////////////////////////////////////////////////////



}  // namespace gloost



/// + operator for string+int

inline const std::string operator+ (const std::string& s, const int ui)
{
  return s + gloost::toString(ui);
}
/// + operator for int+string
inline const std::string operator+ (const int ui, const std::string& s)
{
  return gloost::toString(ui) + s;
}

/// + operator for string+unsigned int
inline const std::string operator+ (const std::string& s, const unsigned int ui)
{
  return s + gloost::toString(ui);
}
/// + operator for unsigned int+string
inline const std::string operator+ (const unsigned int ui, const std::string& s)
{
  return gloost::toString(ui) + s;
}

/// + operator for string+float
inline const std::string operator+ (const std::string& s, const float f)
{
  return s + gloost::toString(f);
}
/// + operator for float+string
inline const std::string operator+ (const float f, const std::string& s)
{
  return gloost::toString(f) + s;
}



#endif // #ifndef GLOOST_HELPER_H
