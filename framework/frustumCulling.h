#ifndef FRUSTUMCULLING_H
#define FRUSTUMCULLING_H

#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>


class frustumCulling{

  public:
    frustumCulling();
    ~frustumCulling();

    void NormalizePlane(int pPlane);
    bool IsPointWithin(const float& X, const float& Y, const float& Z );
    bool IsSphereWithin(const float& X, const float& Y, const float& Z, const float& Radius);
    bool IsBoxWithin(const float& minx, const float& miny, const float& minz, const float& maxx, const float& maxy, const float& maxz);
#if 0
    bool IsOccluded(vec3 &p);
    bool traverseFurther(const float& X, const float& Y, const float& Z, const double& treshold, const int& nodeID, const int& nodesintree_);
#endif
    float getDistance(const float& X, const float& Y, const float& Z);
    void Calculate(void);

    float* getProjM(void);
    float* getModM(void);


  private:

    GLfloat ProjM[16];
    GLfloat ModM[16];
    GLfloat Clip[16];

    GLfloat frustum[6][4];

    const int Right;
    const int Left;
    const int Bottom;
    const int Top;
    const int Back;
    const int Front;
    const int A;
    const int B;
    const int C;
    const int D;

};

#endif // FRUSTUMCULLING_H
