// vim: set fileencoding=utf-8 :
// vim: set encoding=utf-8 :
#include "../CppQuery.h"
#include <iostream>
#include <boost/locale.hpp>
#include <string>

extern template class CppQuery::Query<std::wstring>;
extern template class CppQuery::Query<std::string>;

int main(){
	boost::locale::generator gen;
	std::locale loc = gen("pl.UTF-8");
	std::locale::global(loc);
	
	std::wcout.imbue(loc);
	std::ios_base::sync_with_stdio(false);

	CppQuery::Query<std::wstring> q(L"<div id='main'><h3ąśb class='sp'>It is header zażółć</h3ąśb><p>Lorem ipsum dolor</p><p class='sp'>Second</p>></div>");
	//std::cout << "hello" << std::endl;
	std::wcout << q(L"h3").text() << std::endl;
}

