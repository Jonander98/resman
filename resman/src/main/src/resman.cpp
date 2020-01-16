/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resman.h"

resman::~resman()
{
  //Clean all the registered resource containers
  for (auto pair : m_resources)
    delete pair.second;
}
