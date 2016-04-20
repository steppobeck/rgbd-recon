#ifndef SMOOTHER_H
#define SMOOTHER_H


#include <vector>


namespace sensor {
class Smoother{
 public:
  Smoother(int size = 20);
  ~Smoother();
  int operator () (int value);

  void setSize(unsigned int size);

 private:
  int _index;
  std::vector<double> _VecValues;
};
} // namespace sensor {

#endif // #ifndef SMOOTHER_H
