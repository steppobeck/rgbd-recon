#include "frustum.hpp"

// #include <glbinding/gl/gl.h>
#include <glbinding/gl/functions.h>
#include <glbinding/gl/enum.h>
using namespace gl;

namespace kinect{

static glm::fvec3 closestPoint(glm::fvec3 p, glm::fvec3 u, glm::fvec3 q, glm::fvec3 v);
static std::array<glm::fvec3, 12> getEdgeCenters(std::array<glm::fvec3, 8> const& points_corner);
static std::array<glm::fvec3, 6> getSideNormals(std::array<glm::fvec3, 8> const& points_corner);
static std::array<glm::fvec3, 6> getSideCenters(std::array<glm::fvec3, 8> const& points_corner);
static std::array<glm::fvec4, 6> getPlanes(std::array<glm::fvec3, 8> const& points_corner);

Frustum::Frustum(std::array<glm::fvec3, 8> const& corners)
 :m_corners(corners)
 ,m_planes(getPlanes(m_corners))
 {}

glm::fvec3 Frustum::getCameraPos() const {
  // calculate camera pos lying on view dir
  glm::fvec3 center_near((m_corners[0] + m_corners[1] + m_corners[2] + m_corners[3]) / 4.0f);
  glm::fvec3 center_far((m_corners[4] + m_corners[5] + m_corners[6] + m_corners[7]) / 4.0f);
  glm::fvec3 view_dir{center_far - center_near};

  glm::fvec3 p3{closestPoint(m_corners[0], m_corners[0] - m_corners[4], center_near, view_dir)};
  glm::fvec3 p4{closestPoint(m_corners[1], m_corners[1] - m_corners[5], center_near, view_dir)};
  
  glm::fvec3 p5{closestPoint(m_corners[2], m_corners[2] - m_corners[6], center_near, view_dir)};
  glm::fvec3 p6{closestPoint(m_corners[3], m_corners[3] - m_corners[7], center_near, view_dir)};
  return (p3 + p4 + p5 + p6) / 4.0f;
}


bool Frustum::inside(glm::fvec3 const& point) const {
  for(auto const& plane : m_planes) {
    if(glm::dot(plane, glm::fvec4{point, 1.0f}) < 0.0f) {
      return false;
    }
  }
  return true;
}

void Frustum::draw() const {
  glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(m_corners[0].x, m_corners[0].y, m_corners[0].z);
    glVertex3f(m_corners[4].x, m_corners[4].y, m_corners[4].z);
    
    glVertex3f(m_corners[1].x, m_corners[1].y, m_corners[1].z);
    glVertex3f(m_corners[5].x, m_corners[5].y, m_corners[5].z);
    
    glVertex3f(m_corners[2].x, m_corners[2].y, m_corners[2].z);
    glVertex3f(m_corners[6].x, m_corners[6].y, m_corners[6].z);
    
    glVertex3f(m_corners[3].x, m_corners[3].y, m_corners[3].z);
    glVertex3f(m_corners[7].x, m_corners[7].y, m_corners[7].z);


    glVertex3f(m_corners[0].x, m_corners[0].y, m_corners[0].z);
    glVertex3f(m_corners[1].x, m_corners[1].y, m_corners[1].z);
    
    glVertex3f(m_corners[1].x, m_corners[1].y, m_corners[1].z);
    glVertex3f(m_corners[2].x, m_corners[2].y, m_corners[2].z);
    
    glVertex3f(m_corners[2].x, m_corners[2].y, m_corners[2].z);
    glVertex3f(m_corners[3].x, m_corners[3].y, m_corners[3].z);
    
    glVertex3f(m_corners[3].x, m_corners[3].y, m_corners[3].z);
    glVertex3f(m_corners[0].x, m_corners[0].y, m_corners[0].z);


    glVertex3f(m_corners[4].x, m_corners[4].y, m_corners[4].z);
    glVertex3f(m_corners[5].x, m_corners[5].y, m_corners[5].z);
    
    glVertex3f(m_corners[5].x, m_corners[5].y, m_corners[5].z);
    glVertex3f(m_corners[6].x, m_corners[6].y, m_corners[6].z);
    
    glVertex3f(m_corners[6].x, m_corners[6].y, m_corners[6].z);
    glVertex3f(m_corners[7].x, m_corners[7].y, m_corners[7].z);

    glVertex3f(m_corners[7].x, m_corners[7].y, m_corners[7].z);
    glVertex3f(m_corners[4].x, m_corners[4].y, m_corners[4].z);
  glEnd();

  glm::fvec3 cam_pos{getCameraPos()};
  auto side_centers(getSideCenters(m_corners));

  glPointSize(3.0f);
  glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(cam_pos.x, cam_pos.y, cam_pos.z);
  glEnd();
}

static glm::fvec3 closestPoint(glm::fvec3 p, glm::fvec3 u, glm::fvec3 q, glm::fvec3 v) {
  glm::fvec3 w0 = p - q;
  float a = glm::dot(u, u);
  float b = glm::dot(u, v);
  float c = glm::dot(v, v);
  float d = glm::dot(u, w0);
  float e = glm::dot(v, w0);

  float sc = (b * e - c * d) / (a * c - b * b);
  float tc = (a * e - b * d) / (a * c - b * b);
  glm::fvec3 pc = p + u * sc;
  glm::fvec3 qc = q + v * tc;

  return (pc + qc) * 0.5f;
}

static std::array<glm::fvec3, 6> getSideCenters(std::array<glm::fvec3, 8> const& points_corner) {
  std::array<glm::fvec3, 6> points_sides{};
  // near
  points_sides[0] = (points_corner[0] + points_corner[1] + points_corner[2] + points_corner[3]) / 4.0f;
  // far
  points_sides[1] = (points_corner[4] + points_corner[5] + points_corner[6] + points_corner[7]) / 4.0f;
  // left
  points_sides[2] = (points_corner[0] + points_corner[1] + points_corner[4] + points_corner[5]) / 4.0f;
  // right
  points_sides[3] = (points_corner[2] + points_corner[3] + points_corner[6] + points_corner[7]) / 4.0f;
  // top
  points_sides[4] = (points_corner[1] + points_corner[2] + points_corner[5] + points_corner[6]) / 4.0f;
  // bottom
  points_sides[5] = (points_corner[0] + points_corner[3] + points_corner[4] + points_corner[7]) / 4.0f;
  return points_sides;
}

static std::array<glm::fvec3, 12> getEdgeCenters(std::array<glm::fvec3, 8> const& points_corner) {
  std::array<glm::fvec3, 12> points_edges{};
  points_edges[0] = (points_corner[0] + points_corner[1]) * 0.5f;
  points_edges[1] = (points_corner[1] + points_corner[2]) * 0.5f;
  points_edges[2] = (points_corner[2] + points_corner[3]) * 0.5f;
  points_edges[3] = (points_corner[3] + points_corner[0]) * 0.5f;
  
  points_edges[4] = (points_corner[4] + points_corner[5]) * 0.5f;
  points_edges[5] = (points_corner[5] + points_corner[6]) * 0.5f;
  points_edges[6] = (points_corner[6] + points_corner[7]) * 0.5f;
  points_edges[7] = (points_corner[7] + points_corner[4]) * 0.5f;

  points_edges[8] = (points_corner[0] + points_corner[4]) * 0.5f;
  points_edges[9] = (points_corner[1] + points_corner[5]) * 0.5f;
  points_edges[10] = (points_corner[2] + points_corner[6]) * 0.5f;
  points_edges[11] = (points_corner[3] + points_corner[7]) * 0.5f;
  return points_edges;
}

static std::array<glm::fvec3, 6> getSideNormals(std::array<glm::fvec3, 8> const& points_corner) {
  auto edge_centers(getEdgeCenters(points_corner));
  std::array<glm::fvec3, 6> normals{};
  // near
  normals[0] = glm::normalize(glm::cross(edge_centers[0] - edge_centers[2], edge_centers[3] - edge_centers[2]));
  // far
  normals[1] = glm::normalize(glm::cross(edge_centers[4] - edge_centers[6], edge_centers[5] - edge_centers[7]));
  // left
  normals[2] = glm::normalize(glm::cross(edge_centers[0] - edge_centers[4], edge_centers[9] - edge_centers[8]));
  // right
  normals[3] = glm::normalize(glm::cross(edge_centers[2] - edge_centers[6], edge_centers[11] - edge_centers[10]));
  // top
  normals[4] = glm::normalize(glm::cross(edge_centers[9] - edge_centers[10], edge_centers[1] - edge_centers[5]));
  // bottom
  normals[5] = glm::normalize(glm::cross(edge_centers[8] - edge_centers[11], edge_centers[7] - edge_centers[3]));

  return normals;
}
static std::array<glm::fvec4, 6> getPlanes(std::array<glm::fvec3, 8> const& points_corner) {
  auto side_centers(getSideCenters(points_corner));
  auto side_normals(getSideNormals(points_corner));
  std::array<glm::fvec4, 6> planes{};
  
  for(unsigned i =0; i < 6; ++i) {
    planes[i] = glm::fvec4{side_normals[i], -glm::dot(side_normals[i], side_centers[i])};
  }

  return planes;
}

}