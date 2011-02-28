#include <string>

namespace CppQuery{
	class Query{
		public:
		Query(){}
		Query(const std::string &html){}
		std::string text() const{}
		std::string operator [] (const std::string &attr) const {}
		Query operator [] (int) const {}
		Query operator () (const std::string &selector) const {}
		int size() const {}
	};
}
