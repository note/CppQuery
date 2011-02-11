#include <string>

class CppQuery{
	public:
	CppQuery(){}
	CppQuery(const std::string &html){}
	std::string text() const{}
	std::string operator [] (const std::string &attr) const {}
	CppQuery operator [] (int) const {}
	CppQuery operator () (const std::string &selector) const {}
	int size() const {}
};
