/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "usage/model.h"
#include "utils/filepath.h"

bool Model::VLoad(const Filepath & path)
{
  //std::cout << "LOAD: model" << std::endl;
  (void)path;
  return true;
}

void Model::VUnload()
{
  //std::cout << "UNLOAD: model" << std::endl;
}
