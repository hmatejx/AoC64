#ifndef __MATH_H__
#define __MATH_H__

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define ABS(x)   ((x)<0?-(x):(x))
#define SWAP(x,y) do {auto tmp=(x); (x)=(y); (y)=tmp;} while(0);

#endif // __MATH_H__