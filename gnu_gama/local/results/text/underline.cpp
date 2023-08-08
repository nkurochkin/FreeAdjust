/*
    GNU Gama -- adjustment of geodetic networks
    Copyright (C) 2001  Ales Cepek <cepek@fsv.cvut.cz>

    This file is part of the GNU Gama C++ library.

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <string>
#include <gnu_gama/local/results/text/underline.h>
#include <gnu_gama/xml/encoding.h>

namespace GNU_gama { namespace local {

std::string underline(std::string text, char c)
{
  int i;
  std::string s;
  unsigned char* p = (unsigned char*)text.c_str();
  while (*p)
    {
      p += GNU_gama::Utf8Decode(i, p);
      s += c;
    }
  return s;
}

std::string set_width(std::string s, int n)
{
  int N=0, i;
  unsigned char* p = (unsigned char*)s.c_str();
  while (*p)
    {
      p += GNU_gama::Utf8Decode(i, p);
      N++;
    }
  std::string t(s);
  while (n-- > N) t += ' ';
  return t;
}

}}
