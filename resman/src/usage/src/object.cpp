/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "usage/object.h"
#include "main/resman.h"

void object::foo()
{
  resman r;
  //r.load();
  m_tex = r.get<texture>("mario.png");
  m_tex->get_id();
}