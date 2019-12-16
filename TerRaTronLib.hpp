// *******************************************
//
//	File:		TerRaTronLib.hpp
//
//	Created:	13 Noviembre 2019
//	Author:		Julio Calvo
//
// *******************************************

#pragma once

#if ( defined (T_TERRATRONLIBDEF_IMPL) )
#	define T_TERRATRONLIBDEF_CLASS __declspec ( dllexport )
#	define T_TERRATRONLIBDEF_EXPIMP
#else
#	define T_TERRATRONLIBDEF_CLASS __declspec ( dllimport )
#	define T_TERRATRONLIBDEF_EXPIMP extern extern
#endif