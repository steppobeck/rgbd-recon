
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



/// gloost system includes
#include <UniformTypes.h>



/// cpp includes




namespace gloost
{



//////////////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const vec2& v)
{
  os << "vec2" <<  " {" << v.u << ", " << v.v << "}" << std::endl;

  return os;
}


//////////////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const vec3& v)
{
  os << "vec3" << " {" << v.x << ", " << v.y << ", " << v.z << "}" << std::endl;

  return os;
}

//////////////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const vec4& v)
{
  os << "vec4" << " {" << v.r << ", " << v.g << ", " << v.b << ", " << v.a << "}" << std::endl;

  return os;
}

//////////////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const mat3& v)
{
  os << "mat3" << " {" << v.data[0] << ", " << v.data[1] << ", " << v.data[2] << ", " << std::endl;
  os << "    " << "  " << v.data[3] << ", " << v.data[4] << ", " << v.data[5] << ", " << std::endl;
  os << "    " << "  " << v.data[6] << ", " << v.data[7] << ", " << v.data[8] << std::endl;

  os <<  "    }" << std::endl;

  return os;
}


} // namespace gloost

