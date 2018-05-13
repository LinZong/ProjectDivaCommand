#include <string>
int S2I(const char* num_str);
std::string I2S(int);
template<typename T>
inline T Absolute(T num)
{
	return (num >= 0 ? num : -num);
}
char* G2U(const char* gb2312);
char* U2G(const char* utf8);
