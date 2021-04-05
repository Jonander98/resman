/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "usage/object.h"
#include "main/resman.h"

void Object::Foo()
{
  Resman r;
  //r.load();
  m_tex = r.Get<Texture>("mario.png");
  m_tex->GetId();
}