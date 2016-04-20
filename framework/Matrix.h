
/*

  author: 2008 by Felix Weiﬂig
  mail:   thesleeper@gmx.net


  This class was taken from the openGG scenegraph project of Felix Weiﬂig.
  Read the LICENSE.TXT for copyright questions.

*/


#ifndef GLOOST_MATRIX_H
#define GLOOST_MATRIX_H


/// gloost includes
#include <gloostConfig.h>


/// cpp includes
#include <iostream>
#include <cstdlib>


namespace gloost
{


  class Point3;
  class Vector3;
  class Ray;


  /// GL conform 4x4 matrix

class Matrix
{

  friend bool operator==(const Matrix& lhs, const Matrix& rhs);
  friend bool operator!=(const Matrix& lhs, const Matrix& rhs);


	public:

  /// class constructor
  Matrix();

  /// copy constructor
  Matrix(const Matrix& m);

  ///
  Matrix(const mathType v[16]);

  ///
//  Matrix(const mathType* v);

  /// value constructor
  Matrix(const Vector3& col_1,
         const Vector3& col_2,
         const Vector3& col_3,
         const Point3&  col_4);

  /// class destructor
  virtual ~Matrix();



	/// get 4x4 matrix as an vector of 16 mathTypes
	const mathType* data() const;
	mathType* data();


	/// get/set within colum i and row j
	mathType get( unsigned int i, unsigned int j) const;
	void   set( unsigned int i, unsigned int j, mathType value);

  /// set all values of the matrix
  void setData(const mathType v[16]);


	/// set identity
	void setIdentity();

	/// invert matrix
	void invert ();

	/// returns a inverted version of this matrix
	Matrix inverted () const;

	/// transpose matrix
	void transpose ();

	/// returns transposed matrix
	Matrix transposed () const;

  ///  returns determinate of the rot/scale part of the matrix
	mathType det3() const;

  ///  returns determinate of the matrix
  mathType det() const;

	/// mult matrix with [lhs]*[this]
	void mult (const Matrix& lhs);

  /// compare to matrices
  bool equals(const Matrix& rhs, mathType epsylon = GLOOST_MATH_TYPE_EPSYLON) const;



  /// translate
	void    setTranslate(const mathType x, const mathType y, const mathType z);
	void    setTranslate(Vector3 const& v);
	void    setTranslate(Point3 const& p);
	Vector3 getTranslate() const;

	/// scale
	void    setScale(const mathType x, const mathType y, const mathType z);
	void    setScale(Vector3 const& v);
	void    setScale(Point3 const& p);
	void    setScale(mathType s);
	Vector3 getScale() const;

	/// rotate
	void setRotate(const mathType x, const mathType y, const mathType z);
	void setRotate(Vector3 const& v);
	void setRotate(Point3 const& p);
	void setRotate(Vector3 const& v, mathType angle);
	void setRotate(Point3 const& v, mathType angle);
  void setRotateX(mathType angle);
  void setRotateY(mathType angle);
  void setRotateZ(mathType angle);

  // assignment
  Matrix& operator=(Matrix const& m);

  /// indexing (write, read)
  mathType& operator[](unsigned int i);
  const mathType& operator[](unsigned int i) const;

private:

  mathType _data[16];

  mathType det3 (const mathType a1, const mathType a2, const mathType a3,
	             const mathType b1, const mathType b2, const mathType b3,
	             const mathType c1, const mathType c2, const mathType c3) const;


};

/// matrix + matrix
Matrix  operator+(const Matrix& lhs, const Matrix& rhs);
/// matrix - matrix
Matrix  operator-(const Matrix& lhs, const Matrix& rhs);
/// matrix == matrix ?
bool    operator==(const Matrix& m, const Matrix& n);

/// scalar * matrix
Matrix  operator*(mathType scalar,   Matrix const& m);
/// matrix * scalar
Matrix  operator*(const Matrix& m, mathType scalar);
/// matrix * vector
Vector3 operator*(const Matrix& m, const Vector3& v);
/// matrix * point
Point3  operator*(Matrix const& m, const Point3& p);
/// matrix * ray
Ray operator*(Matrix const& m, const Ray& r);
/// matrix * matrix
Matrix  operator*(const Matrix& lhs, const Matrix& rhs);

extern std::ostream& operator<< (std::ostream&, const Matrix&);


} // namespace gloost


#endif // GLOOST_MATRIX_H
