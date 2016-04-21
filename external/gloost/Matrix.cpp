
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



/// cpp includes
#include <cstring>



/// gloost includes
#include <Matrix.h>
#include <Vector3.h>
#include <Point3.h>
#include <Ray.h>



namespace gloost
{

////////////////////////////////////////////////////////////////////////////////


 /// class constructor

Matrix::Matrix():
  _data()
{
  // setIdentity();
}


////////////////////////////////////////////////////////////////////////////////


 /// copy constructor

Matrix::Matrix(const Matrix& m):
  _data()
{
  (*this) = m;
}


////////////////////////////////////////////////////////////////////////////////


 /// copy constructor

Matrix::Matrix(const mathType v[16]):
  _data()
{
  memcpy(_data, v, sizeof(mathType)*16);
}


////////////////////////////////////////////////////////////////////////////////


 /// copy constructor

//Matrix::Matrix(const mathType* v):
//  _data()
//{
//  for(unsigned int i=0; i != 16; ++i)
//  {
//    _data[i] = (mathType)v[i];
//  }
//}


////////////////////////////////////////////////////////////////////////////////


  /// value constructor

Matrix::Matrix(const Vector3& col_1, const Vector3& col_2,
               const Vector3& col_3, const Point3& col_4):
  _data()
{
  _data[0]  = col_1[0];
  _data[1]  = col_1[1];
  _data[2]  = col_1[2];
  _data[3]  = 0;
  _data[4]  = col_2[0];
  _data[5]  = col_2[1];
  _data[6]  = col_2[2];
  _data[7]  = 0;
  _data[8]  = col_3[0];
  _data[9]  = col_3[1];
  _data[10] = col_3[2];
  _data[11] = 0;
  _data[12] = col_4[0];
  _data[13] = col_4[1];
  _data[14] = col_4[2];
  _data[15] = 1;
}


////////////////////////////////////////////////////////////////////////////////


 /// class destructor

/*virtual*/
Matrix::~Matrix()
{


}


////////////////////////////////////////////////////////////////////////////////


 /// class destructor

mathType
Matrix::get( unsigned int i, unsigned int j) const
{
  return _data[4*i+j];
}


////////////////////////////////////////////////////////////////////////////////


 /// class destructor

void
Matrix::set( unsigned int i, unsigned int j, mathType value)
{
  _data[4*i+j] = value;
}


////////////////////////////////////////////////////////////////////////////////


 /// set all values of the matrix

void
Matrix::setData(const mathType v[16])
{
  for(unsigned int i=0; i != 16; ++i)
  {
    _data[i] = (mathType)v[i];
  }
}


////////////////////////////////////////////////////////////////////////////////


 /// get 4x4 matrix as an vector of 16 mathTypes

const mathType*
Matrix::data() const
{
  return (_data);
}


////////////////////////////////////////////////////////////////////////////////


 /// get 4x4 matrix as an vector of 16 mathTypes

mathType*
Matrix::data()
{
  return (_data);
}



////////////////////////////////////////////////////////////////////////////////


  ///

void
Matrix::setIdentity()
{
  _data[0] = 1;
  _data[1] = 0;
  _data[2] = 0;
  _data[3] = 0;

  _data[4] = 0;
  _data[5] = 1;
  _data[6] = 0;
  _data[7] = 0;

  _data[8] = 0;
  _data[9] = 0;
  _data[10] = 1;
  _data[11] = 0;

  _data[12] = 0;
  _data[13] = 0;
  _data[14] = 0;
  _data[15] = 1;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::invert ()
{

  mathType rDet;
  Matrix result;

  mathType a1, a2, a3, a4,
         b1, b2, b3, b4,
         c1, c2, c3, c4,
         d1, d2, d3, d4;

  a1 = _data[0]; // data_[0].values[0];
  b1 = _data[4]; // data_[1].values[0];
  c1 = _data[8]; // data_[2].values[0];
  d1 = _data[12]; // data_[3].values[0];

  a2 = _data[1]; // data_[0].values[1];
  b2 = _data[5]; // data_[1].values[1];
  c2 = _data[9]; // data_[2].values[1];
  d2 = _data[13]; // data_[3].values[1];

  a3 = _data[2]; // data_[0].values[2];
  b3 = _data[6]; // data_[1].values[2];
  c3 = _data[10]; // data_[2].values[2];
  d3 = _data[14]; // data_[3].values[2];

  a4 = _data[3]; // data_[0].values[3];
  b4 = _data[7]; // data_[1].values[3];
  c4 = _data[11]; // data_[2].values[3];
  d4 = _data[15]; // data_[3].values[3];

  rDet = det();

  if(fabs(rDet) < 0.00001)
  {
    //std::cout << std::endl << "fabs(rDet) < EPS: " << fabs(rDet) << std::endl;
    setIdentity();
    return;
  }

  rDet = 1.f / rDet;

  result[0]  =   det3(b2, b3, b4, c2, c3,	c4, d2, d3, d4) * rDet;
  result[1]  = - det3(a2, a3, a4, c2, c3, c4, d2, d3, d4) * rDet;
  result[2]  =   det3(a2, a3, a4, b2, b3, b4, d2, d3, d4) * rDet;
  result[3]  = - det3(a2, a3, a4, b2, b3, b4, c2, c3, c4) * rDet;

  result[4]  = - det3(b1, b3, b4, c1, c3, c4, d1, d3, d4) * rDet;
  result[5]  =   det3(a1, a3, a4, c1, c3, c4, d1, d3, d4) * rDet;
  result[6]  = - det3(a1, a3, a4, b1, b3, b4, d1, d3, d4) * rDet;
  result[7]  =   det3(a1, a3, a4, b1, b3, b4, c1, c3, c4) * rDet;

  result[8]  =   det3(b1, b2, b4, c1, c2, c4, d1, d2, d4) * rDet;
  result[9]  = - det3(a1, a2, a4, c1, c2, c4, d1, d2, d4) * rDet;
  result[10] =   det3(a1, a2, a4, b1, b2, b4, d1, d2, d4) * rDet;
  result[11] = - det3(a1, a2, a4, b1, b2, b4, c1, c2, c4) * rDet;

  result[12] = - det3(b1, b2, b3, c1, c2, c3, d1, d2, d3) * rDet;
  result[13] =   det3(a1, a2, a3, c1, c2, c3, d1, d2, d3) * rDet;
  result[14] = - det3(a1, a2, a3, b1, b2, b3, d1, d2, d3) * rDet;
  result[15] =   det3(a1, a2, a3, b1, b2, b3, c1, c2, c3) * rDet;

  *this = result;
}


////////////////////////////////////////////////////////////////////////////////


 /// returns a inverted version of this matrix

Matrix
Matrix::inverted () const
{

  mathType rDet;
  Matrix result;

  mathType a1, a2, a3, a4,
         b1, b2, b3, b4,
         c1, c2, c3, c4,
         d1, d2, d3, d4;

  a1 = _data[0]; // data_[0].values[0];
  b1 = _data[4]; // data_[1].values[0];
  c1 = _data[8]; // data_[2].values[0];
  d1 = _data[12]; // data_[3].values[0];

  a2 = _data[1]; // data_[0].values[1];
  b2 = _data[5]; // data_[1].values[1];
  c2 = _data[9]; // data_[2].values[1];
  d2 = _data[13]; // data_[3].values[1];

  a3 = _data[2]; // data_[0].values[2];
  b3 = _data[6]; // data_[1].values[2];
  c3 = _data[10]; // data_[2].values[2];
  d3 = _data[14]; // data_[3].values[2];

  a4 = _data[3]; // data_[0].values[3];
  b4 = _data[7]; // data_[1].values[3];
  c4 = _data[11]; // data_[2].values[3];
  d4 = _data[15]; // data_[3].values[3];

  rDet = det();

  if(fabs(rDet) < 0.00001)
  {
    //std::cout << std::endl << "fabs(rDet) < EPS: " << fabs(rDet) << std::endl;
//    setIdentity();
    Matrix i;
    i.setIdentity();

    return i;
  }

  rDet = 1.f / rDet;

  result[0]  =   det3(b2, b3, b4, c2, c3,	c4, d2, d3, d4) * rDet;
  result[1]  = - det3(a2, a3, a4, c2, c3, c4, d2, d3, d4) * rDet;
  result[2]  =   det3(a2, a3, a4, b2, b3, b4, d2, d3, d4) * rDet;
  result[3]  = - det3(a2, a3, a4, b2, b3, b4, c2, c3, c4) * rDet;

  result[4]  = - det3(b1, b3, b4, c1, c3, c4, d1, d3, d4) * rDet;
  result[5]  =   det3(a1, a3, a4, c1, c3, c4, d1, d3, d4) * rDet;
  result[6]  = - det3(a1, a3, a4, b1, b3, b4, d1, d3, d4) * rDet;
  result[7]  =   det3(a1, a3, a4, b1, b3, b4, c1, c3, c4) * rDet;

  result[8]  =   det3(b1, b2, b4, c1, c2, c4, d1, d2, d4) * rDet;
  result[9]  = - det3(a1, a2, a4, c1, c2, c4, d1, d2, d4) * rDet;
  result[10] =   det3(a1, a2, a4, b1, b2, b4, d1, d2, d4) * rDet;
  result[11] = - det3(a1, a2, a4, b1, b2, b4, c1, c2, c4) * rDet;

  result[12] = - det3(b1, b2, b3, c1, c2, c3, d1, d2, d3) * rDet;
  result[13] =   det3(a1, a2, a3, c1, c2, c3, d1, d2, d3) * rDet;
  result[14] = - det3(a1, a2, a3, b1, b2, b3, d1, d2, d3) * rDet;
  result[15] =   det3(a1, a2, a3, b1, b2, b3, c1, c2, c3) * rDet;

  return result;;
}

////////////////////////////////////////////////////////////////////////////////


	/// transpose matrix

void
Matrix::transpose()
{
  Matrix temp;

  temp[0] = _data[0];
  temp[1] = _data[4];
  temp[2] = _data[8];
  temp[3] = _data[12];

  temp[4] = _data[1];
  temp[5] = _data[5];
  temp[6] = _data[9];
  temp[7] = _data[13];

  temp[8] = _data[2];
  temp[9] = _data[6];
  temp[10] = _data[10];
  temp[11] = _data[14];

  temp[12] = _data[3];
  temp[13] = _data[7];
  temp[14] = _data[11];
  temp[15] = _data[15];

  (*this) = Matrix(temp._data);
}

////////////////////////////////////////////////////////////////////////////////


	/// returns transposed matrix

Matrix
Matrix::transposed() const
{
  Matrix temp;

  temp[0] = _data[0];
  temp[1] = _data[4];
  temp[2] = _data[8];
  temp[3] = _data[12];

  temp[4] = _data[1];
  temp[5] = _data[5];
  temp[6] = _data[9];
  temp[7] = _data[13];

  temp[8] = _data[2];
  temp[9] = _data[6];
  temp[10] = _data[10];
  temp[11] = _data[14];

  temp[12] = _data[3];
  temp[13] = _data[7];
  temp[14] = _data[11];
  temp[15] = _data[15];

  return temp;
}

////////////////////////////////////////////////////////////////////////////////


 ///  returns determinate of the rot/scale part of the matrix


mathType
Matrix::det3() const
{
//    return (data_[0].values[0] * data_[1].values[1] * data_[2].values[2] +
//            data_[0].values[1] * data_[1].values[2] * data_[2].values[0] +
//            data_[0].values[2] * data_[1].values[0] * data_[2].values[1] -
//            data_[0].values[2] * data_[1].values[1] * data_[2].values[0] -
//            data_[0].values[1] * data_[1].values[0] * data_[2].values[2] -
//            data_[0].values[0] * data_[1].values[2] * data_[2].values[1]);
 std::cerr << "ATTENTION in Matrix::det3()" << std::endl;
 return 1234567;
}


////////////////////////////////////////////////////////////////////////////////


 ///  returns determinate of the matrix

mathType
Matrix::det() const
{
  mathType a1, a2, a3, a4,
         b1, b2, b3, b4,
         c1, c2, c3, c4,
         d1, d2, d3, d4;

  a1 = _data[0]; //  data_[0].values[0];
  b1 = _data[4]; //  data_[1].values[0];
  c1 = _data[8]; //  data_[2].values[0];
  d1 = _data[12]; //  data_[3].values[0];

  a2 = _data[1]; //  data_[0].values[1];
  b2 = _data[5]; //  data_[1].values[1];
  c2 = _data[9]; //  data_[2].values[1];
  d2 = _data[13]; //  data_[3].values[1];

  a3 = _data[2]; //  data_[0].values[2];
  b3 = _data[6]; //  data_[1].values[2];
  c3 = _data[10]; //  data_[2].values[2];
  d3 = _data[14]; //  data_[3].values[2];

  a4 = _data[3]; //  data_[0].values[3];
  b4 = _data[7]; //  data_[1].values[3];
  c4 = _data[11]; //  data_[2].values[3];
  d4 = _data[15]; //  data_[3].values[3];

  return(   a1 * det3(b2, b3, b4, c2, c3, c4, d2, d3, d4)
          - b1 * det3(a2, a3, a4, c2, c3, c4, d2, d3, d4)
          + c1 * det3(a2, a3, a4, b2, b3, b4, d2, d3, d4)
          - d1 * det3(a2, a3, a4, b2, b3, b4, c2, c3, c4));

}


////////////////////////////////////////////////////////////////////////////////


 ///  returns determinante of an particular matrix

mathType
Matrix::det3(const mathType a1, const mathType a2, const mathType a3,
             const mathType b1, const mathType b2, const mathType b3,
			       const mathType c1, const mathType c2, const mathType c3) const
{
    return (a1 * b2 * c3) + (a2 * b3 * c1) + (a3 * b1 * c2) -
           (a1 * b3 * c2) - (a2 * b1 * c3) - (a3 * b2 * c1);
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::mult (const Matrix& lhs)
{
  Matrix temp;
  Matrix rhs = (*this);

  // M * this

  temp[0] = lhs[0]*rhs[0] + lhs[4]*rhs[1] + lhs[8]*rhs[2]  + lhs[12]*rhs[3];
  temp[1] = lhs[1]*rhs[0] + lhs[5]*rhs[1] + lhs[9]*rhs[2]  + lhs[13]*rhs[3];
  temp[2] = lhs[2]*rhs[0] + lhs[6]*rhs[1] + lhs[10]*rhs[2] + lhs[14]*rhs[3];
  temp[3] = lhs[3]*rhs[0] + lhs[7]*rhs[1] + lhs[11]*rhs[2] + lhs[15]*rhs[3];

  temp[4] = lhs[0]*rhs[4] + lhs[4]*rhs[5] + lhs[8]*rhs[6]  + lhs[12]*rhs[7];
  temp[5] = lhs[1]*rhs[4] + lhs[5]*rhs[5] + lhs[9]*rhs[6]  + lhs[13]*rhs[7];
  temp[6] = lhs[2]*rhs[4] + lhs[6]*rhs[5] + lhs[10]*rhs[6] + lhs[14]*rhs[7];
  temp[7] = lhs[3]*rhs[4] + lhs[7]*rhs[5] + lhs[11]*rhs[6] + lhs[15]*rhs[7];


  temp[8]  = lhs[0]*rhs[8] + lhs[4]*rhs[9] + lhs[8]*rhs[10]  + lhs[12]*rhs[11];
  temp[9]  = lhs[1]*rhs[8] + lhs[5]*rhs[9] + lhs[9]*rhs[10]  + lhs[13]*rhs[11];
  temp[10] = lhs[2]*rhs[8] + lhs[6]*rhs[9] + lhs[10]*rhs[10] + lhs[14]*rhs[11];
  temp[11] = lhs[3]*rhs[8] + lhs[7]*rhs[9] + lhs[11]*rhs[10] + lhs[15]*rhs[11];


  temp[12] = lhs[0]*rhs[12] + lhs[4]*rhs[13] + lhs[8]*rhs[14]  + lhs[12]*rhs[15];
  temp[13] = lhs[1]*rhs[12] + lhs[5]*rhs[13] + lhs[9]*rhs[14]  + lhs[13]*rhs[15];
  temp[14] = lhs[2]*rhs[12] + lhs[6]*rhs[13] + lhs[10]*rhs[14] + lhs[14]*rhs[15];
  temp[15] = lhs[3]*rhs[12] + lhs[7]*rhs[13] + lhs[11]*rhs[14] + lhs[15]*rhs[15];

  *this = temp;
}


////////////////////////////////////////////////////////////////////////////////


 ///

bool
Matrix::equals(const Matrix& rhs, mathType epsylon) const
{
  for (int i=0; i!=16; ++i)
  {
    if( std::abs(_data[i]-rhs[i]) > epsylon )
      return false;
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setTranslate(const mathType x, const mathType y, const mathType z)
{
  _data[12] = x;
  _data[13] = y;
  _data[14] = z;
  _data[15] = 1;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setTranslate(Vector3 const& v)
{
  _data[12] = v[0];
  _data[13] = v[1];
  _data[14] = v[2];
  _data[15] = 1;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setTranslate(Point3 const& p)
{
  _data[12] = p[0];
  _data[13] = p[1];
  _data[14] = p[2];
  _data[15] = 1;
}


////////////////////////////////////////////////////////////////////////////////


 ///

Vector3
Matrix::getTranslate() const
{
  return Vector3(_data[12], _data[13], _data[14]);
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setScale(const mathType x, const mathType y, const mathType z)
{
  _data[0]  = x;
  _data[5]  = y;
  _data[10] = z;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setScale(Vector3 const& v)
{
  _data[0]  = v[0];
  _data[5]  = v[1];
  _data[10] = v[2];
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setScale(Point3 const& p)
{
  _data[0]  = p[0];
  _data[5]  = p[1];
  _data[10] = p[2];
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setScale(mathType s)
{
  _data[0]  = s;
  _data[5]  = s;
  _data[10] = s;
}


////////////////////////////////////////////////////////////////////////////////


 ///

Vector3
Matrix::getScale() const
{
  return Vector3(_data[0], _data[5], _data[10]);
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotate(const mathType x, const mathType y, const mathType z)
{
  Matrix rotX;
  rotX.setRotateX(x);

  Matrix rotY;
  rotY.setRotateY(y);

  Matrix rotZ;
  rotZ.setRotateZ(z);

  (*this) = rotY*rotX*rotZ;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotate(Vector3 const& v)
{
  setRotate(v[0], v[1], v[2] );
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotate(Point3 const& p)
{
  setRotate(p[0], p[1], p[2]);
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotate(Vector3 const& v, mathType angle)
{
  setRotate(v[0]*angle, v[1]*angle, v[2]*angle);
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotate(Point3 const& p, mathType angle)
{
  setRotate(p[0]*angle, p[1]*angle, p[2]*angle);
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotateX(mathType angle)
{
   setIdentity();

   mathType cosine = cos(angle);
   mathType sine   = sin(angle);

   _data[5]  = cosine;
   _data[6]  =   sine;
   _data[9]  =  -sine;
   _data[10] = cosine;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotateY(mathType angle)
{
   setIdentity();

   mathType cosine = cos(angle);
   mathType sine   = sin(angle);

   _data[0] = cosine;
   _data[2] =  sine;
   _data[8] =  -sine;
   _data[10] = cosine;
}


////////////////////////////////////////////////////////////////////////////////


 ///

void
Matrix::setRotateZ(mathType angle)
{
  setIdentity();

  mathType cosine = cos(angle);
  mathType sine   = sin(angle);

  _data[0] = cosine;
  _data[1] =   sine;
  _data[4] =  -sine;
  _data[5] = cosine;
}


////////////////////////////////////////////////////////////////////////////////


  ///

Matrix&
Matrix::operator=(Matrix const& m)
{
  memcpy(_data, m._data, sizeof(mathType)*16);
  return (*this);
}


////////////////////////////////////////////////////////////////////////////////


 ///

mathType&
Matrix::operator[](unsigned int i)
{
  return _data[i];
}


////////////////////////////////////////////////////////////////////////////////


 /// Read

const mathType&
Matrix::operator[](unsigned int i) const
{
  return  _data[i];
}


////////////////////////////////////////////////////////////////////////////////


 /// Matrix + Matrix

Matrix
operator+(const Matrix& lhs,
          const Matrix& rhs)
{
  Matrix tmp;

  tmp[0] = lhs[0] + rhs[0];
  tmp[1] = lhs[1] + rhs[1];
  tmp[2] = lhs[2] + rhs[2];
  tmp[3] = lhs[3] + rhs[3];
  tmp[4] = lhs[4] + rhs[4];
  tmp[5] = lhs[5] + rhs[5];
  tmp[6] = lhs[6] + rhs[6];
  tmp[7] = lhs[7] + rhs[7];
  tmp[8] = lhs[8] + rhs[8];
  tmp[9] = lhs[9] + rhs[9];
  tmp[10] = lhs[10] + rhs[10];
  tmp[11] = lhs[11] + rhs[11];
  tmp[12] = lhs[12] + rhs[12];
  tmp[13] = lhs[13] + rhs[13];
  tmp[14] = lhs[14] + rhs[14];
  tmp[15] = lhs[15] + rhs[15];

  return tmp;
}


////////////////////////////////////////////////////////////////////////////////


 /// Matrix - Matrix

Matrix
operator-(const Matrix& lhs, const Matrix& rhs)
{
  Matrix tmp;

  tmp[0] = lhs[0] - rhs[0];
  tmp[1] = lhs[1] - rhs[1];
  tmp[2] = lhs[2] - rhs[2];
  tmp[3] = lhs[3] - rhs[3];
  tmp[4] = lhs[4] - rhs[4];
  tmp[5] = lhs[5] - rhs[5];
  tmp[6] = lhs[6] - rhs[6];
  tmp[7] = lhs[7] - rhs[7];
  tmp[8] = lhs[8] - rhs[8];
  tmp[9] = lhs[9] - rhs[9];
  tmp[10] = lhs[10] - rhs[10];
  tmp[11] = lhs[11] - rhs[11];
  tmp[12] = lhs[12] - rhs[12];
  tmp[13] = lhs[13] - rhs[13];
  tmp[14] = lhs[14] - rhs[14];
  tmp[15] = lhs[15] - rhs[15];

  return tmp;
}


////////////////////////////////////////////////////////////////////////////////


 /// Scalar * Matrix

Matrix
operator*(mathType scalar,
          Matrix const& m)
{
  Matrix tmp;

  tmp[0] = m[0] * scalar;
  tmp[1] = m[1] * scalar;
  tmp[2] = m[2] * scalar;
  tmp[3] = m[3] * scalar;
  tmp[4] = m[4] * scalar;
  tmp[5] = m[5] * scalar;
  tmp[6] = m[6] * scalar;
  tmp[7] = m[7] * scalar;
  tmp[8] = m[8] * scalar;
  tmp[9] = m[9] * scalar;
  tmp[10] = m[10] * scalar;
  tmp[11] = m[11] * scalar;
  tmp[12] = m[12] * scalar;
  tmp[13] = m[13] * scalar;
  tmp[14] = m[14] * scalar;
  tmp[15] = m[15] * scalar;

  return tmp;
}


////////////////////////////////////////////////////////////////////////////////


 /// Matrix * Scalar

Matrix
operator*(Matrix const& m,
          mathType scalar )
{
  Matrix tmp;

  tmp[0] = m[0] * scalar;
  tmp[1] = m[1] * scalar;
  tmp[2] = m[2] * scalar;
  tmp[3] = m[3] * scalar;
  tmp[4] = m[4] * scalar;
  tmp[5] = m[5] * scalar;
  tmp[6] = m[6] * scalar;
  tmp[7] = m[7] * scalar;
  tmp[8] = m[8] * scalar;
  tmp[9] = m[9] * scalar;
  tmp[10] = m[10] * scalar;
  tmp[11] = m[11] * scalar;
  tmp[12] = m[12] * scalar;
  tmp[13] = m[13] * scalar;
  tmp[14] = m[14] * scalar;
  tmp[15] = m[15] * scalar;

  return tmp;
}


////////////////////////////////////////////////////////////////////////////////


 /// Matrix * Vector

Vector3
operator*(Matrix  const& m,
          Vector3 const& v )
{
  return Vector3(m[0]*v[0] + m[4]*v[1] + m[8] *v[2],
                 m[1]*v[0] + m[5]*v[1] + m[9] *v[2],
                 m[2]*v[0] + m[6]*v[1] + m[10]*v[2]);
}


////////////////////////////////////////////////////////////////////////////////


 /// matrix * point

Point3
operator*(Matrix const& m,
          Point3 const& p )
{
  return Point3(m[0]*p[0] + m[4]*p[1] + m[8] *p[2] + m[12],
                m[1]*p[0] + m[5]*p[1] + m[9] *p[2] + m[13],
                m[2]*p[0] + m[6]*p[1] + m[10]*p[2] + m[14]);
}


////////////////////////////////////////////////////////////////////////////////


 /// matrix * ray

Ray
operator*(Matrix const& m,
          Ray const& r )
{
  return Ray(m*r.getOrigin(), m*r.getDirection(), r.getT());
}


////////////////////////////////////////////////////////////////////////////////


 /// Matrix multiplication

Matrix
operator*(const Matrix& lhs,
           const Matrix& rhs)
{
  Matrix temp;
  // M * this

  temp[0] = lhs[0]*rhs[0] + lhs[4]*rhs[1] + lhs[8]*rhs[2]  + lhs[12]*rhs[3];
  temp[1] = lhs[1]*rhs[0] + lhs[5]*rhs[1] + lhs[9]*rhs[2]  + lhs[13]*rhs[3];
  temp[2] = lhs[2]*rhs[0] + lhs[6]*rhs[1] + lhs[10]*rhs[2] + lhs[14]*rhs[3];
  temp[3] = lhs[3]*rhs[0] + lhs[7]*rhs[1] + lhs[11]*rhs[2] + lhs[15]*rhs[3];

  temp[4] = lhs[0]*rhs[4] + lhs[4]*rhs[5] + lhs[8]*rhs[6]  + lhs[12]*rhs[7];
  temp[5] = lhs[1]*rhs[4] + lhs[5]*rhs[5] + lhs[9]*rhs[6]  + lhs[13]*rhs[7];
  temp[6] = lhs[2]*rhs[4] + lhs[6]*rhs[5] + lhs[10]*rhs[6] + lhs[14]*rhs[7];
  temp[7] = lhs[3]*rhs[4] + lhs[7]*rhs[5] + lhs[11]*rhs[6] + lhs[15]*rhs[7];


  temp[8]  = lhs[0]*rhs[8] + lhs[4]*rhs[9] + lhs[8]*rhs[10]  + lhs[12]*rhs[11];
  temp[9]  = lhs[1]*rhs[8] + lhs[5]*rhs[9] + lhs[9]*rhs[10]  + lhs[13]*rhs[11];
  temp[10] = lhs[2]*rhs[8] + lhs[6]*rhs[9] + lhs[10]*rhs[10] + lhs[14]*rhs[11];
  temp[11] = lhs[3]*rhs[8] + lhs[7]*rhs[9] + lhs[11]*rhs[10] + lhs[15]*rhs[11];


  temp[12] = lhs[0]*rhs[12] + lhs[4]*rhs[13] + lhs[8]*rhs[14]  + lhs[12]*rhs[15];
  temp[13] = lhs[1]*rhs[12] + lhs[5]*rhs[13] + lhs[9]*rhs[14]  + lhs[13]*rhs[15];
  temp[14] = lhs[2]*rhs[12] + lhs[6]*rhs[13] + lhs[10]*rhs[14] + lhs[14]*rhs[15];
  temp[15] = lhs[3]*rhs[12] + lhs[7]*rhs[13] + lhs[11]*rhs[14] + lhs[15]*rhs[15];

  return temp;
}


////////////////////////////////////////////////////////////////////////////////


 ///

bool operator==(const Matrix& lhs, const Matrix& rhs)
{
    return lhs.equals(rhs);
}


////////////////////////////////////////////////////////////////////////////////


 ///

bool operator!=(const Matrix& lhs, const Matrix& rhs)
{
    return !(lhs == rhs);
}


////////////////////////////////////////////////////////////////////////////////


 /// ostream

std::ostream&
operator<< (std::ostream& os, const Matrix& m)
{
  os << "matrix[" << std::fixed << std::endl;
  os << "       ("               << m[0] << ", " << m[4] << ", " << m[8]  << ", " << m[12] << ")," << std::endl;
  os << "       ("               << m[1] << ", " << m[5] << ", " << m[9]  << ", " << m[13] << ")," << std::endl;
  os << "       ("               << m[2] << ", " << m[6] << ", " << m[10] << ", " << m[14] << ")," << std::endl;
  os << "       ("               << m[3] << ", " << m[7] << ", " << m[11] << ", " << m[15] << ") ]" << std::endl;

  return os;
}


////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////




} // namespace gloost





