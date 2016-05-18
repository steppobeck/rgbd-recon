#ifndef TEXTURE_BLITTER_HPP
#define TEXTURE_BLITTER_HPP

#include <globjects/Program.h>
#include <glm/gtc/type_precision.hpp>

// quad singleton
class TextureBlitter {
 public:
  static void blit(unsigned unit, unsigned layer, glm::uvec2 const& resolution);
  static void blit(unsigned unit, glm::uvec2 const& resolution);
  
 private:
  static void blit(glm::uvec2 const& resolution);
  static TextureBlitter const & inst();

  // prevent construction by user
  TextureBlitter();
  ~TextureBlitter();
  TextureBlitter(TextureBlitter const&) = delete;
  TextureBlitter& operator=(TextureBlitter const&) = delete;

  globjects::Program*              m_program;
};

#endif //TEXTURE_BLITTER_HPP