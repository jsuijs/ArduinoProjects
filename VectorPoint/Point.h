//-----------------------------------------------------------------------------
// Point.h
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
#ifndef POINTH
#define POINTH

#include <math.h>

class TVector;

class TPoint
{
   public:
   	short   X;
	   short   Y;

      bool IsZero() { return ((X==0) && (Y==0));};

      TPoint() { Set(0,0); }
      TPoint(short InX, short InY)  { Set(InX, InY);  };
      TPoint(const TPoint  &Point ) { *this = Point;  }
      TPoint(const TVector &Vector) { *this = Vector; }

      void Set(short InX, short InY) { X = InX; Y = InY; }

      float Angle()  const { return atan2((double) Y, (double) X); }   // indeed: y is 1st param, x = 2nd
      long  LengthSq()  const { return (long)X * (long)X + (long)Y * (long)Y; }
      float Length()    const { return sqrt(LengthSq()); }

      long  DistanceSq(const TPoint &P);
      float Distance(const TPoint &P);
      //- operators -----------------------------------
      bool     operator == (const TPoint  &Point );
      bool     operator != (const TPoint  &Point );
      void     operator  = (const TPoint  &Point );
      void     operator  = (const TVector &Vector);

      void     operator += (const TPoint  &Point );
      void     operator += (const TVector &Vector);

      void     operator -= (const TPoint  &Point );
      void     operator -= (const TVector &Vector);

      TPoint   operator  + (const TPoint  &Point );
      TPoint   operator  + (const TVector &Vector);

      TPoint   operator  - (const TPoint  &Point );
      TPoint   operator  - (const TVector &Vector);

   private:

};

#endif
