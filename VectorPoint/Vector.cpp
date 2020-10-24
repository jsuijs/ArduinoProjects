//-----------------------------------------------------------------------------
// Vector.cpp
//
// Copyright (c) 2013-2020 Joep Suijs - All rights reserved.
//
// RobotLib tags: RED_CORE
//-----------------------------------------------------------------------------
// This file is part of RobotLib.
//
// RobotLib is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RobotLib is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RobotLib.  If not, see <http://www.gnu.org/licenses/>.
//
// See http://wiki.robotmc.be/index.php?title=RobotLib for more information.
//-----------------------------------------------------------------------------
// Deze file is een RODE CORE file.
// Een RODE file is een standaard file, die voor iedere toepassing gelijk is.
// CORE files behoren tot de kern van RobotLib en worden in iedere toepassing
// gebruikt.
//-----------------------------------------------------------------------------
// tags_end
//-----------------------------------------------------------------------------
#include "Point.h"
#include "Vector.h"

//# cblock 1 utilities
//------------------------------------------------------------------------
// TVector::Degrees - return vector angle in degrees
//------------------------------------------------------------------------
//------------------------------------------------------------------------
int TVector::Degrees()
   {
      return Angle * (180.0 / M_PI);
   }

//# cblock 1 utilities
//------------------------------------------------------------------------
// TVector::Degrees - Set vector angle to degrees param
//------------------------------------------------------------------------
//------------------------------------------------------------------------
void TVector::Degrees(int InDegrees)
   {
      Angle = InDegrees / (180.0 / M_PI);
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TVector::operator = Vector
//----------------------------------------------------------------------------
// this = Vector
//----------------------------------------------------------------------------
void TVector::operator = (const TVector &InVector)
   {
      Angle  = InVector.Angle;
      Length = InVector.Length;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TVector::operator = Point
//----------------------------------------------------------------------------
// this = Point
//----------------------------------------------------------------------------
void TVector::operator = (const TPoint &Point)
   {
      Angle = Point.Angle();
      Length = (short) (0.5 + Point.Length());
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TVector::operator +=
//----------------------------------------------------------------------------
// this = this + Point
//----------------------------------------------------------------------------
void TVector::operator += (const TPoint &Point)
   {
      TPoint P = *this; // vector->point conversion
      P += Point;
      *this = P;        // vector->point conversion
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TVector::operator +=
//----------------------------------------------------------------------------
// this = this + Vector
//----------------------------------------------------------------------------
void TVector::operator += (const TVector   &Vector)
   {
      TPoint P = Vector;   // vector->point conversion
      *this += P;          // add + point->vector convesion
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TVector::operator +
//----------------------------------------------------------------------------
// return (this + Vector)
//----------------------------------------------------------------------------
TPoint TVector::operator + (const TVector   &Vector)
   {
      TPoint Result = *this;  // vector->point conversion
      Result += Vector;       // vector->point conversion

      return Result;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TVector::operator -
//----------------------------------------------------------------------------
// return (this - Vector)
//----------------------------------------------------------------------------
TPoint TVector::operator - (const TVector   &Vector)
   {
      TVector T = Vector;
      T.Angle  += M_PI;

      TPoint Result = *this;  // vector->point conversion
      Result   += T;          // vector->point conversion

      return Result;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TVector::operator +
//----------------------------------------------------------------------------
// return (this + point)
//----------------------------------------------------------------------------
TPoint TVector::operator + (const TPoint   &Point)
   {
      TPoint Result = *this; // vector->point conversion
      Result += Point;

      return Result;
   }
