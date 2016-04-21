
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
#include <ObjMatFile.h>



/// cpp includes
#include <string>
#include <iostream>



namespace gloost
{

////////////////////////////////////////////////////////////////////////////////


  /// class constructor

ObjMatFile::ObjMatFile():
    _filePath("0"),
    _materialMap()
{
	// insert your code here
}


////////////////////////////////////////////////////////////////////////////////


  /// class destructor

ObjMatFile::~ObjMatFile()
{
  std::map<std::string, gloost::Material*>::iterator matIt    = _materialMap.begin();
  std::map<std::string, gloost::Material*>::iterator matEndIt = _materialMap.end();


	for (; matIt!=matEndIt; ++matIt)
	{
	  delete (*matIt).second;
	}

	_materialMap.clear();

}


////////////////////////////////////////////////////////////////////////////////


  /// ...

bool
ObjMatFile::load(const std::string& filePath)
{

  _filePath = filePath;


  std::ifstream infile;
  infile.open(_filePath.c_str());

  if (!infile)
  {

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_WARNINGS
    std::cout << std::endl;
    std::cout << std::endl << "Warning from ObjMatFile::load:";
    std::cout << std::endl << "             Cant open \"" << _filePath << "\"!";
    std::cout << std::endl;
    std::flush(std::cout);
#endif

    return 0;
  }


#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout << std::endl;
  std::cout << std::endl << "Message from ObjMatFile::load:";
  std::cout << std::endl << "             Loading \"" << _filePath << "\".";
  std::cout << std::endl;
  std::flush(std::cout);
#endif

  std::string token;

  gloost::Material* currentMaterial = 0;

  while(infile >> token)
  {

    if(token == "newmtl")
    {
      std::string newMatName;

      infile >> newMatName;

      currentMaterial = new gloost::Material();
      currentMaterial->setAmbient(0.0, 0.0, 0.0,  1.0);
      currentMaterial->setDiffuse(0.0, 0.0, 0.0,  1.0);
      currentMaterial->setSpecular(0.0, 0.0, 0.0, 1.0);
      currentMaterial->setNs(64);
      currentMaterial->setIllumination(0.0);

      _materialMap[newMatName] = currentMaterial;

    }
    else if(token == "Ka")
    {
      Point3 component(infile);
      currentMaterial->setAmbient(component[0],component[1],component[2], 1.0);
    }
    else if(token == "Kd")
    {
      Point3 component(infile);
      currentMaterial->setDiffuse(component[0],component[1],component[2], 1.0);
    }
    else if(token == "Ks")
    {
      Point3 component(infile);
      currentMaterial->setSpecular(component[0],component[1],component[2], 1.0);
    }
    else if(token == "illum")
    {
      float component = 0.0;
      infile >> component;
      currentMaterial->setIllumination(component);
    }
    else if(token == "Ns")
    {
      float component = 0.0;
      infile >> component;
      currentMaterial->setNs(component);
    }
  }

  infile.close();

#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_MESSAGES
  std::cout << std::endl;
  std::cout << std::endl << "Message from ObjMatFile::load:";
  std::cout << std::endl << "             \"" << _filePath << "\" succesfully loaded";
  std::cout << std::endl << "              " << _materialMap.size() << " materials:";

  std::map<std::string, gloost::Material*>::iterator matIt    = _materialMap.begin();
  std::map<std::string, gloost::Material*>::iterator matItEnd = _materialMap.end();


  for (; matIt!=matItEnd; ++matIt)
  {
    std::cout << std::endl << "                - \"" << (*matIt).first << "\"";
  }

  std::cout << std::endl;
  std::flush(std::cout);
#endif

  return true;
}


////////////////////////////////////////////////////////////////////////////////


  /// returns a gloost::Material Pointer for a name or 0 if the material was not found

gloost::Material*
ObjMatFile::getMaterial(const std::string& materialName)
{

  std::map<std::string, gloost::Material*>::iterator pos = _materialMap.find(materialName);

  if (pos != _materialMap.end())
  {
    return (*pos).second;
  }


#ifndef GLOOST_SYSTEM_DISABLE_OUTPUT_WARNINGS
    std::cout << std::endl;
    std::cout << std::endl << "WARNING from ObjMatFile::getMaterial:";
    std::cout << std::endl << "             Material with name \"" << materialName << "\" was not";
    std::cout << std::endl << "             found in this ObjMatFile instance.";

    if (_filePath != std::string("0"))
    {
      std::cout << std::endl << "             loaded from defined in  \"" << _filePath << "\".";
    }

    std::cout << std::endl;
    std::flush(std::cout);
#endif


  return 0;

}


////////////////////////////////////////////////////////////////////////////////


  /// add a Material to this mat file

void
ObjMatFile::addMaterial(const std::string& name, Material* material)
{
  _materialMap[name] = material;
}


////////////////////////////////////////////////////////////////////////////////





} // namespace gloost

