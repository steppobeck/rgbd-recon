
#if !defined(SENSOR_SENSOR_H)

#define SENSOR_SENSOR_H

// includes, system

#include <Matrix.h>

// includes, project

#include <device.h>

// types, exported (class, enum, struct, union, typedef)


namespace sensor {

class sensor{
public:

  sensor(const device* /*device*/, size_t /*station_id*/);
  ~sensor();

  size_t                 getFrame() const;
  const timevalue& getTimestamp() const;
  const gloost::Matrix&     getMatrix();
  bool                   getButton(size_t /*which*/) const;

  void                   setTransmitterOffset(const gloost::Matrix& /*transmitterOffset*/);
  void                   setReceiverOffset(const gloost::Matrix& /*receiverOffset*/);

  const gloost::Matrix&     getTransmitterOffset() const;
  const gloost::Matrix&     getReceiverOffset() const;
  

private:
  
  const device::station& _station;
  gloost::Matrix      _devicemat;
  gloost::Matrix      _transmitterOffset;
  gloost::Matrix      _receiverOffset;
  

};

} // end namespace sensor {

// variables, exported (extern)

// functions, inlined (inline)
  
// functions, exported (extern)
  
#endif // #if !defined(SENSOR_SENSOR_H)

