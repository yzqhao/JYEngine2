#ifndef THREADSAFEOPRATOR
#error( "Do not include this file in other files" )
#endif

namespace JYE
{

//------------------------------------------------------------------------------------------------------
static int Increment(volatile int* var)
{
	return __sync_add_and_fetch(var, 1);
}
//------------------------------------------------------------------------------------------------------
static int Decrement(volatile int* var)
{
	return __sync_sub_and_fetch(var, 1);
}
//------------------------------------------------------------------------------------------------------
static int Add(volatile int* var, int add)
{
	return __sync_add_and_fetch(var, add);
}
	
}
