#if !defined(SENSOR_TIMEVALUE_H)

#define SENSOR_TIMEVALUE_H

// includes, system

#include <iosfwd> // std::ostream
#include <string> // std::string

// includes, project

// #include <>

// exported namespace

namespace sensor {
  
  // types, exported (class, enum, struct, union, typedef)
  
  class timevalue {
    
  public:
    
    // this must be a _signed_ (integral ??) type !!
    
    typedef signed long long size_type;
    
    // some predefined (and maybe useful) time values
    
    static const timevalue const_zero;
    static const timevalue const_010_ns;
    static const timevalue const_050_ns;
    static const timevalue const_100_ns;
    static const timevalue const_500_ns;
    static const timevalue const_999_ns;
    static const timevalue const_010_us;
    static const timevalue const_050_us;
    static const timevalue const_100_us;
    static const timevalue const_500_us;
    static const timevalue const_999_us;
    static const timevalue const_010_ms;
    static const timevalue const_050_ms;
    static const timevalue const_100_ms;
    static const timevalue const_500_ms;
    static const timevalue const_999_ms;
    static const timevalue const_010_s;
    static const timevalue const_050_s;
    static const timevalue const_100_s;
    static const timevalue const_500_s;
    static const timevalue const_999_s;
    static const timevalue const_001_h;
    static const timevalue const_001_d;
    
    /** a (faked) struct timeval as known from posix.1 */
    struct timevalue_rep {
      
    public:
      
      typedef long sec_type;
      typedef int  nsec_type;
      
    public:
      
      sec_type  tv_sec;
      nsec_type tv_nsec;
      
    };
    
  public:
    
    timevalue(const timevalue&);
    timevalue(const timevalue_rep::sec_type& = 0, const timevalue_rep::nsec_type& = 0);
    timevalue(const double&);
    ~timevalue() throw();
    
    operator timevalue_rep() const;
    
    const timevalue& operator=(const timevalue&);
    const timevalue& operator=(const double&);
    
    timevalue& operator+=(const timevalue&);
    timevalue& operator-=(const timevalue&);
    timevalue& operator*=(const unsigned int&);
    timevalue& operator/=(const unsigned int&);
    
    std::string formatted() const; // return [yy:ddd:hh:mm:ss.usec] formatted
    size_type sec() const;         // return seconds
    size_type msec() const;        // return millseconds
    size_type usec() const;        // return microseconds
    size_type nsec() const;        // return nanoseconds
    
    friend bool operator==(const timevalue&, const timevalue&);
    friend bool operator!=(const timevalue&, const timevalue&);
    friend bool operator<(const timevalue&, const timevalue&);
    friend bool operator<=(const timevalue&, const timevalue&);
    friend bool operator>(const timevalue&, const timevalue&);
    friend bool operator>=(const timevalue&, const timevalue&);
    
    friend void nanosleep(const timevalue& timeout);
    friend void sleep(const timevalue&);
    
  protected:
    
    timevalue_rep _stamp;
    
  };
  
  // variables, exported (extern)
  
  // functions, inlined (inline)
  
  // functions, exported (extern)
  
  extern const timevalue operator+(const timevalue&, const timevalue&);
  extern const timevalue operator-(const timevalue&, const timevalue&);
  extern const timevalue operator*(const timevalue&, const unsigned int&);
  extern const timevalue operator*(const unsigned int&, const timevalue&);
  extern const timevalue operator/(const timevalue&, const unsigned int&);
  
  extern std::ostream& operator<<(std::ostream&, const timevalue&);

} // namespace sensor {

#endif // #if !defined(SENSOR_TIMEVALUE_H)

