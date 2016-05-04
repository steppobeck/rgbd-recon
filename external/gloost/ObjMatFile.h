
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



#ifndef GLOOST_OBJMATFILE_H
#define GLOOST_OBJMATFILE_H



/// gloost system includes
#include <gloostConfig.h>
#include <gloostMath.h>
#include <Material.h>


/// cpp includes
#include <string>
#include <map>



namespace gloost
{


  ///  reads *.mtl files ( Material definition of Wavefront Obj format )

class ObjMatFile
{
	public:

    /// class constructor
    ObjMatFile();

    /// class destructor
	  ~ObjMatFile();


    /// ~
    bool load(const std::string& filePath);


    /// returns a gloost::Material Pointer for a name or 0 if the material was not found
    gloost::Material* getMaterial(const std::string& materialName);


    /// add a Material to this mat file
    void addMaterial(const std::string& name, Material* material);



	private:

	  std::string                              _filePath;
    std::map<std::string, gloost::Material*> _materialMap;

};


} // namespace gloost


#endif // GLOOST_OBJMATFILE_H

