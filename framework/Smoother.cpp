#include <Smoother.h>
namespace sensor {
Smoother::Smoother(int size):
  _index(0),
  _VecValues(){
  for(int i = 0; i < size; ++i)
    _VecValues.push_back(1.0);
}


Smoother::~Smoother()
{}


int
Smoother::operator () (int value){
  _VecValues[_index++] = (double) value;
  if(_index == (int) _VecValues.size())
    _index = 0;

  double smooth = 0.0;
  int size = _VecValues.size();
  for(int i = 0; i < size; ++i)
    smooth += _VecValues[i];

  return (int) (smooth/size);
}


void
Smoother::setSize(unsigned int size){
  _VecValues.clear();
  for(unsigned int i = 0; i < size; ++i)
    _VecValues.push_back(1.0);
}

} // namespace sensor {
