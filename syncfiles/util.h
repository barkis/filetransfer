#include <string>
#include <vector>
#include <iterator>
#include <stdexcept>

namespace util {

	void exchange(std::string& stringin, const char *oldchar, const char *newchar);
	void SlashEnd(std::string& stringin, bool set = true);
	void SlashStart(std::string& stringin, bool set = true);
	std::vector<std::string> split(std::string strin, char delim = '\04');
	template<class Target, class Source>
	Target narrow_cast(Source v);

	template <typename T>
	class Array {
		T *pBuff;
		int m_Size;
	public:
		~Array() { delete[] pBuff; }
		Array() = delete;
		explicit Array(int size):m_Size(size),pBuff(new T[size]{}){}
		T& operator[](int index) { return pBuff[index]; }
		T* data() { return pBuff; };
		void copy(T* start, T* end, int startPos) {
			if (startPos + (end - start)  > m_Size) {
				throw std::range_error("Over size in util::Array::copy");
			}
			std::copy(start, end, pBuff + startPos);
		}
	};
}
