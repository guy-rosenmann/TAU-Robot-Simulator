#ifndef __MAKE_UNIQUE__H_
#define __MAKE_UNIQUE__H_

#include <memory>

#ifndef __cpp_lib_make_unique
#ifndef _WINDOWS_

template<typename T, typename ...Args>
std::unique_ptr<T> make_unique(Args&& ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif // _WINDOWS_
#endif // __cpp_lib_make_unique


#endif //__MAKE_UNIQUE__H_