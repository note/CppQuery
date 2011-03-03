// vim: set fileencoding=utf-8 :
// vim: set encoding=utf-8 :
#include "../CppQuery.h"
#include <iostream>

int main(){
	CppQuery::Query q("<div id='main'><h3 class='sp'>It is header</h3><p>Lorem ipsum dolor</p><p class='sp'>Second</p>></div>");
	std::cout << "hello" << std::endl;
	std::cout << q("h3").text() << std::endl;
}
