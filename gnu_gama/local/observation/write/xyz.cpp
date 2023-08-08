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

#include <iostream>
#include <iomanip>
#include <gnu_gama/local/observation.h>
#include <gnu_gama/local/pobs/format.h>

using namespace GNU_gama::local;
using namespace std;


void X::write(std::ostream& out, bool) const
{
  using namespace std;
  out << "<!-- " << from() << " x = "
      << setprecision(Format::coord_p()) << value() << " --!>";
}


void Y::write(std::ostream& out, bool) const
{
  using namespace std;
  out << "<!-- " << from() << " y = "
      << setprecision(Format::coord_p()) << value() << " --!>";
}


void Z::write(std::ostream& out, bool) const
{
  using namespace std;
  out << "<!-- " << from() << " z = "
      << setprecision(Format::coord_p()) << value() << " --!>";
}
