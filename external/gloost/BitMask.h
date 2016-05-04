
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



#ifndef H_GLOOST_BITMASK
#define H_GLOOST_BITMASK



#define GLOOST_BITMASK_ALL_SET   0xFFFFFFFF
#define GLOOST_BITMASK_ALL_UNSET 0x00000000



/// gloost system includes



/// cpp includes
#include <string>



namespace gloost
{


  ///  BitMask to hold 32 flags (0..31)

class BitMask
{
	public:

    /// class constructor
    BitMask();

    /// class constructor
    BitMask(unsigned int value);

    /// class constructor
    BitMask(const BitMask& bitMask);

    /// class destructor
	  ~BitMask();


//	  void         setMask(const unsigned int mask, bool threadsafe = true);
//    unsigned int getMask() const;



	  /// returns true if position of the Bitask is enabled
	  bool getFlag(unsigned int flagPosition) const;

	  /// set flags 0..31 of this BitMask
	  void setFlag(unsigned int flagPosition, bool value = 1);


	  ///  set flags 0..31 of this BitMask on more than one flag positions
	  void setFlags(const BitMask& positions, bool value = 1);

	  ///  set all flags 0..31 of this BitMask on all positions to value
	  void setFlags(bool value = 1);


	  /// invert the BitMask
	  void invert();

	  /// returns a inverted version of this BitMask
	  BitMask inverted() const;


	  /// returns the value of the mask as unsigned int
	  unsigned int getValue() const;


	  /// returns true if at least one flag in mask is identical to this one
	  bool operator&(const BitMask& mask) const;


	  /// assigns a unsigned int value
	  unsigned int operator=(unsigned int value);


	private:

	  unsigned int _mask;


};



/// ostream operator
extern std::ostream& operator<< (std::ostream&, const BitMask& bitMask);


} // namespace gloost


#endif // GLOOST_TRAVMASK_H



