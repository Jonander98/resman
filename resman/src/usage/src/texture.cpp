/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "usage/texture.h"
#include "utils/filepath.h"


bool Texture::VLoad(const Filepath & path)
{
  (void)path;
  //std::cout << "LOAD: texture" << std::endl;

  return true;
}

void Texture::VUnload()
{
  //std::cout << "UNLOAD: texture" << std::endl;
}
