/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/resource.h"


class model : public resource
{
public:
  file_path get_file_path()const;
private:
  bool load(const file_path &)override final;
private:
  file_path m_file;
};