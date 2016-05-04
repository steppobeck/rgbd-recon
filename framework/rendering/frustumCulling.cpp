//original code from DGL Wiki, for more information look at
//http://wiki.delphigl.com/index.php/Tutorial_Frustum_Culling

#include "frustumCulling.h"

frustumCulling::frustumCulling():
  ProjM(), ModM(), Clip(), frustum(),
  Right(0), Left(1),
  Bottom(2), Top(3),
  Back(4), Front(5),
  A(0), B(1), C(2), D(3)
{}


frustumCulling::~frustumCulling()
{}

void
frustumCulling::NormalizePlane(int plane){

  GLfloat magnitude = sqrtf( (powf(2, frustum[plane][A]) + powf(2, frustum[plane][B]) + powf(2, frustum[plane][C])) );
  //std::cerr << "magnitude " << magnitude<< std::endl;
  frustum[plane][A] = frustum[plane][A] / magnitude;
  frustum[plane][B] = frustum[plane][B] / magnitude;
  frustum[plane][C] = frustum[plane][C] / magnitude;
  frustum[plane][D] = frustum[plane][D] / magnitude;
}

//point's distance from a plane:
//distance = A * X + B * Y + C * Z + D
//A, B, C, and D are the four numbers that define the plane and X, Y, and Z are the point's coordinates.
bool
frustumCulling::IsPointWithin(const float& X, const float& Y, const float& Z ){

  for(int i=0; i<=5; i++){
    if( (frustum[i][A]*X + frustum[i][B]*Y + frustum[i][C]*Z + frustum[i][D]) <= 0 ){
        return false;
    }
  }
  return true;
}

bool
frustumCulling::IsSphereWithin(const float& X, const float& Y, const float& Z, const float& Radius){

  for(int i=0; i<=5; i++){
    if( (frustum[i][A]*X + frustum[i][B]*Y + frustum[i][C]*Z + frustum[i][D]) <= -Radius ){
      return false;
    }
  }
  return true;
}

#if 0
//// Funktion hier unglücklich plaziert aber erstmal am einfachsten für die Abstandberechnung eines Punktes zur Near_Plane, da hier
//durch den Aufruf von Callculate (wegen des Cullings) auch eine ständig akutalisierte Nearplane vorliegt
//vielelicht nochmal überdenken, wie und wo die Funktion besser plaziert werden kann, bzw. wie die Klasse hier
//sinnvoller umbenannt werden kann um die Funktion hier zu lassen

bool
frustumCulling::traverseFurther(const float& X, const float& Y, const float& Z, const double& treshold,const int& nodeID, const int& nodesintree_){

  int norm = ceil(log(nodesintree_)/log(2));
  double dist = (frustum[Front][A]*X + frustum[Front][B]*Y + frustum[Front][C]*Z + frustum[Front][D]); //abstand von punkt zu viewplane (Abstand Punkt zu Ebene berechnung)


//  if( dist <= (double)(norm - floor(log(nodeID)/log(2))) *treshold/20 ) {
//  if( dist*treshold/200 <= (double)(norm - floor(log(nodeID)/log(2))) / 2) {
  if( dist + treshold/200 <=  (norm - floor(log(nodeID+1)/log(2))) * treshold/20 ) {
    return true;
  }
  return false;
}

#endif

float
frustumCulling::getDistance(const float& X, const float& Y, const float& Z){
  return (frustum[Front][A]*X + frustum[Front][B]*Y + frustum[Front][C]*Z + frustum[Front][D]); //abstand von punkt zu viewplane (Abstand Punkt zu Ebene berechnung)
}

bool //funktion noch Überdenken!!!! vorerst nur simpel übernommen, auf richtige Funktion Testen!!!!
frustumCulling::IsBoxWithin(const float& minx, const float& miny, const float& minz, const float& maxx, const float& maxy, const float& maxz){

  for(int i=0; i<=5; i++){
    if((frustum[i][A]*maxx + frustum[i][B]*maxy + frustum[i][C]*maxz + frustum[i][D]) > 0) continue; //v0 point that is in front of the current plane,
    if((frustum[i][A]*minx + frustum[i][B]*maxy + frustum[i][C]*maxz + frustum[i][D]) > 0) continue; //v1 skip and move to next plane, saving calculations
    if((frustum[i][A]*minx + frustum[i][B]*miny + frustum[i][C]*maxz + frustum[i][D]) > 0) continue; //v2
    if((frustum[i][A]*maxx + frustum[i][B]*miny + frustum[i][C]*maxz + frustum[i][D]) > 0) continue; //v3
    if((frustum[i][A]*maxx + frustum[i][B]*miny + frustum[i][C]*minz + frustum[i][D]) > 0) continue; //v4
    if((frustum[i][A]*maxx + frustum[i][B]*maxy + frustum[i][C]*minz + frustum[i][D]) > 0) continue; //v5
    if((frustum[i][A]*minx + frustum[i][B]*maxy + frustum[i][C]*minz + frustum[i][D]) > 0) continue; //v6
    if((frustum[i][A]*minx + frustum[i][B]*miny + frustum[i][C]*minz + frustum[i][D]) > 0) continue; //v7
    return false;
  }
  return true;
}




float*
frustumCulling::getProjM(void){
  return ProjM;
}

float*
frustumCulling::getModM(void){
  return ModM;
}

void
frustumCulling::Calculate(void){

  

  glGetFloatv(GL_PROJECTION_MATRIX, ProjM);
  glGetFloatv(GL_MODELVIEW_MATRIX, ModM);

  

  Clip[ 0] = ModM[ 0]*ProjM[ 0] + ModM[ 1]*ProjM[ 4] + ModM[ 2]*ProjM[ 8] + ModM[ 3]*ProjM[12];
  Clip[ 1] = ModM[ 0]*ProjM[ 1] + ModM[ 1]*ProjM[ 5] + ModM[ 2]*ProjM[ 9] + ModM[ 3]*ProjM[13];
  Clip[ 2] = ModM[ 0]*ProjM[ 2] + ModM[ 1]*ProjM[ 6] + ModM[ 2]*ProjM[10] + ModM[ 3]*ProjM[14];
  Clip[ 3] = ModM[ 0]*ProjM[ 3] + ModM[ 1]*ProjM[ 7] + ModM[ 2]*ProjM[11] + ModM[ 3]*ProjM[15];


  Clip[ 4] = ModM[ 4]*ProjM[ 0] + ModM[ 5]*ProjM[ 4] + ModM[ 6]*ProjM[ 8] + ModM[ 7]*ProjM[12];
  Clip[ 5] = ModM[ 4]*ProjM[ 1] + ModM[ 5]*ProjM[ 5] + ModM[ 6]*ProjM[ 9] + ModM[ 7]*ProjM[13];
  Clip[ 6] = ModM[ 4]*ProjM[ 2] + ModM[ 5]*ProjM[ 6] + ModM[ 6]*ProjM[10] + ModM[ 7]*ProjM[14];
  Clip[ 7] = ModM[ 4]*ProjM[ 3] + ModM[ 5]*ProjM[ 7] + ModM[ 6]*ProjM[11] + ModM[ 7]*ProjM[15];

  Clip[ 8] = ModM[ 8]*ProjM[ 0] + ModM[ 9]*ProjM[ 4] + ModM[10]*ProjM[ 8] + ModM[11]*ProjM[12];
  Clip[ 9] = ModM[ 8]*ProjM[ 1] + ModM[ 9]*ProjM[ 5] + ModM[10]*ProjM[ 9] + ModM[11]*ProjM[13];
  Clip[10] = ModM[ 8]*ProjM[ 2] + ModM[ 9]*ProjM[ 6] + ModM[10]*ProjM[10] + ModM[11]*ProjM[14];
  Clip[11] = ModM[ 8]*ProjM[ 3] + ModM[ 9]*ProjM[ 7] + ModM[10]*ProjM[11] + ModM[11]*ProjM[15];

  Clip[12] = ModM[12]*ProjM[ 0] + ModM[13]*ProjM[ 4] + ModM[14]*ProjM[ 8] + ModM[15]*ProjM[12];
  Clip[13] = ModM[12]*ProjM[ 1] + ModM[13]*ProjM[ 5] + ModM[14]*ProjM[ 9] + ModM[15]*ProjM[13];
  Clip[14] = ModM[12]*ProjM[ 2] + ModM[13]*ProjM[ 6] + ModM[14]*ProjM[10] + ModM[15]*ProjM[14];
  Clip[15] = ModM[12]*ProjM[ 3] + ModM[13]*ProjM[ 7] + ModM[14]*ProjM[11] + ModM[15]*ProjM[15];

  

  frustum[Right][A] = Clip[ 3] - Clip[ 0]; //Right(0), Left(1)

  frustum[Right][B] = Clip[ 7] - Clip[ 4]; //Bottom(2), Top(3)
  frustum[Right][C] = Clip[11] - Clip[ 8]; //Back(4), Front(5)
  frustum[Right][D] = Clip[15] - Clip[12]; //A(0), B(1), C(2), D(3)
  
  NormalizePlane(Right);
  
  frustum[Left][A] = Clip[ 3] + Clip[ 0];
  frustum[Left][B] = Clip[ 7] + Clip[ 4];
  frustum[Left][C] = Clip[11] + Clip[ 8];
  frustum[Left][D] = Clip[15] + Clip[12];
  NormalizePlane(Left);

  frustum[Bottom][A] = Clip[ 3] + Clip[ 1];
  frustum[Bottom][B] = Clip[ 7] + Clip[ 5];
  frustum[Bottom][C] = Clip[11] + Clip[ 9];
  frustum[Bottom][D] = Clip[15] + Clip[13];
  NormalizePlane(Bottom);

  frustum[Top][A] = Clip[ 3] - Clip[ 1];
  frustum[Top][B] = Clip[ 7] - Clip[ 5];
  frustum[Top][C] = Clip[11] - Clip[ 9];
  frustum[Top][D] = Clip[15] - Clip[13];
  NormalizePlane(Top);

  frustum[Back][A] = Clip[ 3] - Clip[ 2];
  frustum[Back][B] = Clip[ 7] - Clip[ 6];
  frustum[Back][C] = Clip[11] - Clip[10];
  frustum[Back][D] = Clip[15] - Clip[14];
  NormalizePlane(Back);

  frustum[Front][A] = Clip[ 3] + Clip[ 2];
  frustum[Front][B] = Clip[ 7] + Clip[ 6];
  frustum[Front][C] = Clip[11] + Clip[10];
  frustum[Front][D] = Clip[15] + Clip[14];
  NormalizePlane(Front);

}

