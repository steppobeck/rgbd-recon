#include "timevalue.h"

// includes, system

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

#ifdef WIN32
#	include <windows.h>
#endif // WIN32

// includes, project

#include <clock.h>

// internal unnamed namespace

namespace {
  
  // types, internal (class, enum, struct, union, typedef)

  using sensor::timevalue;
  
  // variables, internal

  const timevalue::size_type one_second_in_nsec = 1000000000;
  const timevalue::size_type one_second_in_usec = 1000000;
  const timevalue::size_type one_second_in_msec = 1000;
  const timevalue::size_type one_second_in_nsec_neg = -one_second_in_nsec;
  
  // functions, internal

  /**
   * this is needed since the following should yield true:<br>
   * <code>timevalue(1, 1000000) == timevalue(2, 0)</code>
   * <p>
   * <b>!stolen from ace 4.3!</b>
   */
  timevalue::timevalue_rep
  normalize(timevalue::size_type a)
  {
    timevalue::timevalue_rep result = {0, 0};

    if ((0 > a) && (one_second_in_nsec_neg >= a)) {
      do {
        --result.tv_sec;
        a += one_second_in_nsec;
      } while (one_second_in_nsec_neg >= a);
    }
    
    else if ((0 < a) && (one_second_in_nsec <= a)) {
      do {
        ++result.tv_sec;
        a -= one_second_in_nsec;
      } while (one_second_in_nsec <= a);
    }

    result.tv_nsec = a;
    
    if ((1 <= result.tv_sec) && (0 > result.tv_nsec)) {
      --result.tv_sec;
      result.tv_nsec += one_second_in_nsec;
    }

    else if ((0 > result.tv_sec) && (0 < result.tv_nsec)) {
      ++result.tv_sec;
      result.tv_nsec -= one_second_in_nsec;
    }
    
    return result;
  }
                                  
} // namespace {

// exported namespace

namespace sensor {

  // variables, exported

  const timevalue timevalue::const_zero  (    0,         0);
  const timevalue timevalue::const_010_ns(    0,        10);
  const timevalue timevalue::const_050_ns(    0,        50);
  const timevalue timevalue::const_100_ns(    0,       100);
  const timevalue timevalue::const_500_ns(    0,       500);
  const timevalue timevalue::const_999_ns(    0,       999);
  const timevalue timevalue::const_010_us(    0,     10000);
  const timevalue timevalue::const_050_us(    0,     50000);
  const timevalue timevalue::const_100_us(    0,    100000);
  const timevalue timevalue::const_500_us(    0,    500000);
  const timevalue timevalue::const_999_us(    0,    999000);
  const timevalue timevalue::const_010_ms(    0,  10000000);
  const timevalue timevalue::const_050_ms(    0,  50000000);
  const timevalue timevalue::const_100_ms(    0, 100000000);
  const timevalue timevalue::const_500_ms(    0, 500000000);
  const timevalue timevalue::const_999_ms(    0, 999000000);
  const timevalue timevalue::const_010_s (   10,         0);
  const timevalue timevalue::const_050_s (   50,         0);
  const timevalue timevalue::const_100_s (  100,         0);
  const timevalue timevalue::const_500_s (  500,         0);
  const timevalue timevalue::const_999_s (  999,         0);
  const timevalue timevalue::const_001_h ( 3600,         0);
  const timevalue timevalue::const_001_d (68400,         0);

  // functions, exported

  timevalue::timevalue(const timevalue& a)
    : _stamp(a._stamp)
  {}

  timevalue::timevalue(const timevalue_rep::sec_type& secs, const timevalue_rep::nsec_type& nsecs)
  {
    _stamp.tv_sec = secs;
    _stamp.tv_nsec = nsecs;

    const timevalue_rep tmp(normalize(nsec()));

    _stamp.tv_sec = tmp.tv_sec;
    _stamp.tv_nsec = tmp.tv_nsec;
  }

  timevalue::timevalue(const double& a)
  {
    typedef timevalue_rep::sec_type sec_type;
    typedef timevalue_rep::nsec_type nsec_type;

    _stamp.tv_sec = static_cast<sec_type>(a);
    _stamp.tv_nsec = static_cast<nsec_type>((a -
                                             static_cast<double>(_stamp.tv_sec)) *
                                             one_second_in_nsec);

    const timevalue_rep tmp(normalize(nsec()));

    _stamp.tv_sec = tmp.tv_sec;
    _stamp.tv_nsec = tmp.tv_nsec;
  }

  timevalue::~timevalue() throw()
  {}

  timevalue::operator
  timevalue::timevalue_rep() const
  {
    return _stamp;
  }

  const timevalue&
  timevalue::operator=(const timevalue& a)
  {
    _stamp = a._stamp;

    return *this;
  }

  const timevalue&
  timevalue::operator=(const double& a)
  {
    typedef timevalue_rep::sec_type sec_type;
    typedef timevalue_rep::nsec_type nsec_type;

    _stamp.tv_sec = static_cast<sec_type>(a);
    _stamp.tv_nsec = static_cast<nsec_type>((a -
                                             static_cast<double>(_stamp.tv_sec)) *
                                             one_second_in_nsec);

    const timevalue_rep tmp(normalize(nsec()));

    _stamp.tv_sec = tmp.tv_sec;
    _stamp.tv_nsec = tmp.tv_nsec;

    return *this;
  }

  timevalue&
  timevalue::operator+=(const timevalue& a)
  {
    const timevalue_rep tmp(normalize(nsec() + a.nsec()));

    _stamp.tv_sec = tmp.tv_sec;
    _stamp.tv_nsec = tmp.tv_nsec;

    return *this;
  }

  timevalue&
  timevalue::operator-=(const timevalue& a)
  {
    const timevalue_rep tmp(normalize(nsec() - a.nsec()));
    
    _stamp.tv_sec = tmp.tv_sec;
    _stamp.tv_nsec = tmp.tv_nsec;

    return *this;
  }

  timevalue&
  timevalue::operator*=(const unsigned int& a)
  {
    if (0 != a) {
      const timevalue_rep tmp(normalize(nsec() * a));

      _stamp.tv_sec = tmp.tv_sec;
      _stamp.tv_nsec = tmp.tv_nsec;
    } else
      _stamp.tv_sec = _stamp.tv_nsec = 0;

    return *this;
  }

  timevalue&
  timevalue::operator/=(const unsigned int& a)
  {
    if (0 != a) {
      const timevalue_rep tmp(normalize(nsec() / a));
      
      _stamp.tv_sec = tmp.tv_sec;
      _stamp.tv_nsec = tmp.tv_nsec;
    } else {
      _stamp.tv_nsec = one_second_in_nsec - 1;
      _stamp.tv_sec = 0;
      
      std::cerr << "<timevalue::operator/=>: "
                << "applying division by zero provides a time value of {"
                << _stamp.tv_sec << "," << _stamp.tv_nsec << '}'
                << std::endl;
    }

    return *this;
  }

  std::string
  timevalue::formatted() const
  {
    const std::string sign = (0 > nsec()) ? "-" : "";
    
    timevalue_rep::sec_type current_secs(std::abs(_stamp.tv_sec));

    const std::size_t years = current_secs / 31536000;

    current_secs -= years * 31536000;

    const std::size_t days =  current_secs / 86400;

    current_secs -= days * 86400;

    const std::size_t hours = current_secs / 3600;

    current_secs -= hours * 3600;

    const std::size_t minutes = current_secs / 60;

    current_secs -= minutes * 60;

    std::ostringstream result;

    result << sign
           << std::setfill('0')
           << std::setw(2) << years << ':'
           << std::setw(3) << days  << ':'
           << std::setw(2) << hours << ':'
           << std::setw(2) << minutes << ':'
           << std::setw(2) << current_secs << '.'
           << std::setw(9) << std::abs(_stamp.tv_nsec);

    return result.str().c_str();
  }

  timevalue::size_type
  timevalue::sec() const
  {
    return (nsec() / one_second_in_nsec);
  }

  timevalue::size_type
  timevalue::msec() const
  {
    return (nsec() / one_second_in_usec);
  }

  timevalue::size_type
  timevalue::usec() const
  {
    return (nsec() / one_second_in_msec);
  }
  
  timevalue::size_type
  timevalue::nsec() const
  {
    return ((static_cast<size_type>(_stamp.tv_sec) * one_second_in_nsec)
            + static_cast<size_type>(_stamp.tv_nsec));
  }

  bool
  operator==(const timevalue& a, const timevalue& b)
  {
    return ((a._stamp.tv_sec == b._stamp.tv_sec) &&
            (a._stamp.tv_nsec == b._stamp.tv_nsec));
  }

  bool
  operator!=(const timevalue& a, const timevalue& b)
  {
    return !(a == b);
  }

  bool
  operator<(const timevalue& a, const timevalue& b)
  {
    return a.nsec() < b.nsec();
  }

  bool
  operator<=(const timevalue& a, const timevalue& b)
  {
    return a.nsec() <= b.nsec();
  }

  bool
  operator>(const timevalue& a, const timevalue& b)
  {
    return a.nsec() > b.nsec();
  }

  bool
  operator>=(const timevalue& a, const timevalue& b)
  {
    return a.nsec() >= b.nsec();
  }

  void
  nanosleep(const timevalue& timeout)
  {
#if defined(_POSIX_SOURCE)

    struct ::timespec remain = {0, 0};
    struct ::timespec request = {timeout._stamp.tv_sec, timeout._stamp.tv_nsec};

    while (0 != ::nanosleep(&request, &remain))
      request = remain;

#elif defined(WIN32)
	  Sleep(timeout.msec());
#else
# error "unable to determine system time api"
#endif // #if defined(_POSIX_SOURCE)

  }

  void
  sleep(const timevalue& timeout)
  {
    nanosleep(timeout);
  }

  const timevalue
  operator+(const timevalue& a, const timevalue& b)
  {
    return timevalue(a) += b;
  }

  const timevalue
  operator-(const timevalue& a, const timevalue& b)
  {
    return timevalue(a) -= b;
  }

  const timevalue
  operator*(const timevalue& a, const unsigned int& b)
  {
    return timevalue(a) *= b;
  }

  const timevalue
  operator*(const unsigned int& a, const timevalue& b)
  {
    return timevalue(b) *= a;
  }

  const timevalue
  operator/(const timevalue& a, const unsigned int& b)
  {
    return timevalue(a) /= b;
  }

  std::ostream&
  operator<<(std::ostream& os, const timevalue& a)
  {
    std::ostream::sentry cerberus(os);

    if (cerberus)
      os << a.formatted();

    return os;
  }

} // namespace sensor {

