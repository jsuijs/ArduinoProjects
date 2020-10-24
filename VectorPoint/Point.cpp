//-----------------------------------------------------------------------------
// Point.cpp
//
// Copyright (c) 2013-2018 Joep Suijs - All rights reserved.
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
//----------------------------------------------------------------------------
// TPoint::DistanceSq - Square of distance to point
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
long TPoint::DistanceSq(const TPoint &P)
{
   TPoint T = *this;
   T -= P;
   return T.LengthSq();
}

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::DistanceSq - Get distance to point
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
float TPoint::Distance(const TPoint &P)
{
   TPoint T = *this;
   T -= P;
   return T.Length();
}

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator = Point
//----------------------------------------------------------------------------
// this = Point
//----------------------------------------------------------------------------
void TPoint::operator = (const TPoint &InPoint)
{
   X = InPoint.X;
   Y = InPoint.Y;
}

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator = Vector
//----------------------------------------------------------------------------
// this = Vector
//----------------------------------------------------------------------------
void TPoint::operator = (const TVector &Vector)
{
   X = (short) (Vector.X());
   Y = (short) (Vector.Y());
}

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator +=
//----------------------------------------------------------------------------
// this = this + Point
//----------------------------------------------------------------------------
void TPoint::operator += (const TPoint &Point)
   {
      X += Point.X;
      Y += Point.Y;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator +=
//----------------------------------------------------------------------------
// this = this + Vector
//----------------------------------------------------------------------------
void TPoint::operator += (const TVector &Vector)
   {
      TPoint P = Vector;

      X += P.X;
      Y += P.Y;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator -=
//----------------------------------------------------------------------------
// this = this - Point
//----------------------------------------------------------------------------
void TPoint::operator -= (const TPoint &Point)
   {
      X -= Point.X;
      Y -= Point.Y;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator -=
//----------------------------------------------------------------------------
// this = this - Vector
//----------------------------------------------------------------------------
void TPoint::operator -= (const TVector &Vector)
   {
      TPoint P = Vector;

      X -= P.X;
      Y -= P.Y;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator +
//----------------------------------------------------------------------------
// return (this + Point)
//----------------------------------------------------------------------------
TPoint TPoint::operator + (const TPoint &Point)
   {
      TPoint Result = *this;

      Result += Point;

      return Result;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator +
//----------------------------------------------------------------------------
// return (this + Vector)
//----------------------------------------------------------------------------
TPoint TPoint::operator + (const TVector   &Vector)
   {
      TPoint Result = *this;

      Result += Vector;

      return Result;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator -
//----------------------------------------------------------------------------
// return (this - Point)
//----------------------------------------------------------------------------
TPoint TPoint::operator - (const TPoint &Point)
   {
      TPoint Result = *this;

      Result -= Point;

      return Result;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator -
//----------------------------------------------------------------------------
// return (this - Vector)
//----------------------------------------------------------------------------
TPoint TPoint::operator - (const TVector   &Vector)
   {
      TPoint Result = *this;

      Result -= Vector;

      return Result;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator ==
//----------------------------------------------------------------------------
// return true when point values are equal
//----------------------------------------------------------------------------
bool TPoint::operator == (const TPoint &Point)
   {
      if (Point.X != this->X) return false;
      if (Point.Y != this->Y) return false;

      return true;
   }

//# cblock 1 utilities
//----------------------------------------------------------------------------
// TPoint::operator !=
//----------------------------------------------------------------------------
// return true when point values are not equal
//----------------------------------------------------------------------------
bool TPoint::operator != (const TPoint &Point)
   {
      if (*this == Point) return false;

      return true;
   }


