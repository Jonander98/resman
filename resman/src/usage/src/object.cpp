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
  m_tex = r.get("mario.png");
}