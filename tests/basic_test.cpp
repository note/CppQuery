// vim: set fileencoding=utf-8 :
#include "../CppQuery.h"
#include <iostream>
#include <boost/locale.hpp>
#include <string>

extern template class CppQuery::Query<std::wstring>;
extern template class CppQuery::Query<std::string>;
using namespace std;

int main(){
	boost::locale::generator gen;
	locale loc = gen("pl.UTF-8");
	locale::global(loc);
	
	wcout.imbue(loc);
	ios_base::sync_with_stdio(false);

	CppQuery::Query<wstring> q(L"<html><div id='main'><h3 class='sp'>It is header zażółć</h3><p class='special'>abc</p><p>Lorem ipsum dolor</p><p class='sp'>Second is here</p></div><div id='footer'><p id='copy'>Copyright</p></div><a class=\"special\">tagged</html>");
	//std::cout << "RERERE" << std::endl;
	//std::wcout << L"helloł" << std::endl;
	wcout << q(L"a").text() << endl;
	wcout << q(L"#footer p").text() << endl;
	wcout << q(L".sp").size() << endl;
	wcout << q(L":contains(is)").size() << endl;
	wcout << q(L":contains(ipsum)").text() << endl;

// 	CppQuery::Query<std::string> q("<div id='main'><h3 class='sp'>It is header za</h3><p>Lorem ipsum dolor</p><p class='sp'>Second</p></div>");
// 	std::cout << q("p[class=sp]").text() << std::endl;

}

