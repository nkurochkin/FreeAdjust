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

#ifndef gama_local_LocalNetwork_gso_h
#define gama_local_LocalNetwork_gso_h

#include <gnu_gama/local/network.h>
#include <gnu_gama/adj/adj_gso.h>

namespace GNU_gama { namespace local
{
  class LocalNetwork_gso : public LocalNetwork
    {
      typedef GNU_gama::AdjGSO<Double, GNU_gama::local::MatVecException> OLS_gso;
      OLS_gso* ols_gso;

    public:

      LocalNetwork_gso()
      {
        ols_gso = new OLS_gso;
        set_algorithm(ols_gso);
      }
      ~LocalNetwork_gso()
      {
        delete ols_gso;
      }

      bool   lindep(Index i) { return ols_gso->lindep(i); }
      Double cond()          { return ols_gso->cond();    }

      const char* const algorithm() const { return "gso"; }
    };
}}

#endif









