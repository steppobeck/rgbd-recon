#include "texture_blitter.hpp"

#include "screen_quad.hpp"

#include <glbinding/gl/enum.h>
#include <glbinding/gl/functions.h>
using namespace gl;
#include <globjects/Shader.h>

TextureBlitter::TextureBlitter()
 :m_program{new globjects::Program()}
{
  m_program->attach(
   globjects::Shader::fromFile(GL_VERTEX_SHADER,   "glsl/texture_passthrough.vs")
  ,globjects::Shader::fromFile(GL_FRAGMENT_SHADER, "glsl/texture_passthrough.fs")
  );
  // register to destroy at context destruction
  m_program->ref();
}

TextureBlitter::~TextureBlitter() {
  // if destroyed before context, free resources
  m_program->unref();
}

void TextureBlitter::blit(unsigned unit, unsigned layer, glm::uvec2 const& resolution) {
  auto& instance = inst();
  instance.m_program->setUniform("texture_array", GLint(unit));
  instance.m_program->setUniform("layer", layer);
  instance.m_program->setUniform("mode", 1u);
  blit(resolution);
}

void TextureBlitter::blit(unsigned unit, glm::uvec2 const& resolution) {
  auto& instance = inst();
  instance.m_program->setUniform("texture_handle", GLint(unit));
  instance.m_program->setUniform("mode", 0u);
  blit(resolution);
}

void TextureBlitter::blitLod(unsigned unit, unsigned lod, glm::uvec2 const& resolution) {
  auto& instance = inst();
  instance.m_program->setUniform("texture_handle", GLint(unit));
  instance.m_program->setUniform("mode", 2u);
  instance.m_program->setUniform("resolution", glm::ivec2{resolution});
  instance.m_program->setUniform("lod", int(lod));
  blit(resolution);
}

void TextureBlitter::blit(glm::uvec2 const& resolution) {
  auto& instance = inst();
  GLsizei old_vp_params[4];
  glGetIntegerv(GL_VIEWPORT, old_vp_params);

  glViewport(0, 0, resolution.x, resolution.y);

  instance.m_program->use();

  glDisable(GL_DEPTH_TEST);
  ScreenQuad::draw();
  glEnable(GL_DEPTH_TEST);
  
  instance.m_program->release();
  glViewport(old_vp_params[0], old_vp_params[1],
             old_vp_params[2], old_vp_params[3]);
}

TextureBlitter const& TextureBlitter::inst() {
  static TextureBlitter instance{};
  return instance;
}