/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resource.h"


bool AResource::IsLoaded() const
{
  return m_isLoaded;
}

AResource::IdType AResource::GetId() const
{
  return m_id;
}

void AResource::VUnload()
{
  //Nothing by default
}

void AResource::InternalLoad(const Filepath & fp)
{
  VLoad(fp);
  m_isLoaded = true;
  m_path = fp;
}

void AResource::InternalUnload()
{
  VUnload();
  m_isLoaded = false;
}
