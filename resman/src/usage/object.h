/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "texture.h"
#include "main/resource.h"

class Object
{
  void Foo();
private:
  ResourcePtr<Texture> m_tex;

};