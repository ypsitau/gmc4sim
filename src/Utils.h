// GMC-4 Simulator
// Copyright (C) 2009 Dan-san
// http://dansan.air-nifty.com/blog/gmc4-simulator.html

#ifndef __UTILS_H__
#define __UTILS_H__

#define NUMBEROF(x) (sizeof(x) / sizeof(x[0]))
#define foreach(T, i, c) for (T::iterator i = (c).begin(); i != (c).end(); i++)
#define foreach_const(T, i, c) for (T::const_iterator i = (c).begin(); i != (c).end(); i++)

#endif
