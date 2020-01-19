/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "usage/texture.h"
#include "utils/filepath.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"


bool texture::load(const file_path & path)
{
  //std::cout << "LOAD: texture" << std::endl;
  //Flip Y for opengl
  stbi_set_flip_vertically_on_load(true);

  i32 width, height, nrChannels;
  u8 *data = stbi_load(path.get_fullpath().c_str(), &width, &height, &nrChannels, 0);
  if (!data)
    return false;

  stbi_image_free(data);
  return true;
}

void texture::unload()
{
  //std::cout << "UNLOAD: texture" << std::endl;
}
