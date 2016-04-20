
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
#include <BitMask.h>



/// cpp includes
#include <string>
#include <iostream>



namespace gloost
{

///////////////////////////////////////////////////////////////////////////////


  /// class constructor

BitMask::BitMask():
  _mask(GLOOST_BITMASK_ALL_UNSET)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

BitMask::BitMask(unsigned int value):
  _mask(value)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


  /// class constructor

BitMask::BitMask(const BitMask& bitMask):
  _mask(bitMask._mask)
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////


  /// class destructor

BitMask::~BitMask()
{
	// insert your code here
}


///////////////////////////////////////////////////////////////////////////////

  /// returns true if position of the BitMask is enabled

bool
BitMask::getFlag(unsigned int flagPosition) const
{
	flagPosition = 1 << flagPosition;
	return (_mask & flagPosition);
}


///////////////////////////////////////////////////////////////////////////////


  /// enable flags 0..31 of this TravMask


void
BitMask::setFlag(unsigned int flagPosition, bool value)
{

  flagPosition = 1 << flagPosition;

  if (value)
  {
    _mask = _mask | flagPosition;
  }
  else
  {
    flagPosition = ~flagPosition;
    _mask = _mask & flagPosition;
  }
}


///////////////////////////////////////////////////////////////////////////////


  ///  set flags 0..31 of this BitMask on more than one flag positions

void
BitMask::setFlags(const BitMask& positions, bool value)
{
  if (value)
  {
    _mask = _mask | positions._mask;
  }
  else
  {
    _mask = _mask & positions.inverted()._mask;
  }
}


///////////////////////////////////////////////////////////////////////////////


  ///  ///  set all flags 0..31 of this BitMask on all positions to value

void
BitMask::setFlags(bool value)
{
  if (value)
  {
    _mask = GLOOST_BITMASK_ALL_SET;
  }
  else
  {
    _mask = GLOOST_BITMASK_ALL_UNSET;
  }
}


///////////////////////////////////////////////////////////////////////////////


  /// invert the BitMask

void
BitMask::invert()
{
	_mask = ~_mask;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns a inverted version of this BitMask

BitMask
BitMask::inverted() const
{
	BitMask bm( (*this) );
	bm.invert();
	return bm;
}


///////////////////////////////////////////////////////////////////////////////


  /// returns the value of the mask as unsigned int

unsigned int
BitMask::getValue() const
{
  return _mask;
}


///////////////////////////////////////////////////////////////////////////////


bool
BitMask::operator&(const BitMask& mask) const
{
  return (_mask & mask._mask); /// & mask mask mask          ... maks, aahh mask
}


////////////////////////////////////////////////////////////////////////////////


/// assigns a unsigned int value

unsigned int
BitMask::operator=(unsigned int value)
{
  _mask = value;
  return _mask;
}


////////////////////////////////////////////////////////////////////////////////


  ///

/* extern */
std::ostream&
operator<< (std::ostream& os, const BitMask& bitMask)
{
  os << "BitMask( " << std::fixed << bitMask.getValue() << " )";
  return os;
}


///////////////////////////////////////////////////////////////////////////////





} // namespace gloost



