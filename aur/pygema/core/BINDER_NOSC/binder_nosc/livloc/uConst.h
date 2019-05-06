//=== File description =========================================================
/*!   \file uConst.h
\brief  Definition of a unique data type nomenclature for basic data types.
.. Geowissenschaften Uni Potsdam A. Weidauer V/2002
*/

/*
 * This file is part of binder.
 *
 * Copyright (C) 2009 Andy Heath, Stuart Nippress & Andreas Rietbrock,
 *                    University of Liverpool
 *
 * This work was funded as part of the NERIES (JRA5) project.
 * Additional funding for Nippress from NERC research grant NE/C000315/1
 *
 * binder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * binder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with binder.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __UCONST_H__
#define __UCONST_H__


//--- Modules -------------------------------------------------------------------
#include <cstdlib>

//--- Constants -----------------------------------------------------------------
#define nil 0
#define NIL 0

#define dIndexNIL -1

//--- Types ---------------------------------------------------------------------
//! common pointer type
typedef void*            Pointer;
//! common boolean type (false/true)
typedef bool             tBoolean;
//! 32 bit real type
typedef float            tFloat;
//! 64 bit real type
typedef double           tDouble;
//! 128 bit real type
typedef long double      tLargeDouble;
//! signed 32 bit integer type
typedef int              tInteger;
//! signed 64 bit integer type
typedef long             tExtended;
//! unsigned 32 bit integer type
typedef unsigned int     tWord;
//! unsigned 64 bit integer type
typedef unsigned long    tCardinal;
//! unsigned 16 bit integer type
typedef unsigned short   tByte;
//! unsigned 8 bit integer type like byte
typedef unsigned char    tChar;
//! signed 128 bit integer type
typedef long long        tLargeIntger;
//! unsigned 128 bit integer type
typedef unsigned long long tLargeCardinal;

//! pointer to  32 bit real type
typedef float*           pFloat;
//! pointer to 64 bit real type
typedef double*          pDouble;
//! pointer to 128 bit real type
typedef long double*     pLargeDouble;
//! pointer to signed 32 bit integer type
typedef int*             pInteger;
//! pointer to signed 64 bit integer type
typedef long*            pExtended;
//! pointer to unsigned 32 bit integer type
typedef unsigned int*    pWord;
//! pointer to unsigned 64 bit integer type
typedef unsigned long*   pCardinal;
//! pointer to unsigned 16 bit integer type
typedef unsigned short*  pByte;
//! pointer to unsigned 8 bit integer type like byte
typedef unsigned char*   pChar;

//! pointer to a chracter array 
typedef const char*      pConst;

//! open array to a 32 bit float 
typedef float*           tFloatArray;
//! open array to a 64 bit float 
typedef double*          tDoubleArray;
//! open array to a 128 bit float 
typedef long double*     tLargeDoubleArray;
//! open array to a 32 bit signed integer
typedef int*             tIntegerArray;
//! open array to a 64 bit signed integer
typedef long*            tExtendedArray;
//! open array to a 32 bit unsigned integer
typedef unsigned int*    tWordArray;
//! open array to a 64 bit unsigned integer
typedef unsigned long*   tCardinalArray;
//! open array to a 16 bit unsigned integer
typedef unsigned short*  tByteArray;
//! open array to a 8 bit unsigned integer like a byte
typedef unsigned char*   tCharArray;

#endif // __UCONST_H__
//------------------------------------------------------------------------------
// EndOf
//------------------------------------------------------------------------------
