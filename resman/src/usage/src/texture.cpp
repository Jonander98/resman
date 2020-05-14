/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "usage/texture.h"
#include "utils/filepath.h"


bool texture::load(const filepath & path)
{
  (void)path;
  //std::cout << "LOAD: texture" << std::endl;

  return true;
}

void texture::unload()
{
  //std::cout << "UNLOAD: texture" << std::endl;
}
