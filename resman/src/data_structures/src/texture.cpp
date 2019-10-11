
#include "pch.h"
#include "data_structures/texture.h"
#include "utils/filepath.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

bool Texture::load(const FilePath & path)
{
  //Flip Y for opengl
  stbi_set_flip_vertically_on_load(true);

  i32 width, height, nrChannels;
  u8 *data = stbi_load(path.get_fullpath().c_str(), &width, &height, &nrChannels, 0);
  if (!data)
    return false;

  stbi_image_free(data);
  return true;
}