#include <string>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/spirit/include/qi.hpp>
#include <iostream>
#include <boost/bind.hpp>

using namespace boost;
using std::cout;
using std::endl;

struct handle_child_operator{
	void operator ()(const std::string &str){
		cout << "post_operator" << str << endl;
	}
};

struct handle_descendant_operator{
	void operator ()(const std::string &str){
		cout << "post_operator" << str << endl;
	}
};

struct handle_id{
	void operator ()(const std::string &str){
		cout << "id" << str << endl;
	}
};

struct Handlers{
	void handle_class(const std::string &str){
		cout << "class " << str << endl;
	}

	void handle_id(const std::string &str){
		cout << "id " << str << endl;
	}

	void handle_contains(const std::string &str){
		cout << "contains " << str << endl;
	}

	void handle_attr(fusion::vector<std::string, std::string> v){
		cout << "attr" << fusion::at_c<0>(v) << ":" << fusion::at_c<1>(v) << endl;
	}

	void handle_element(const std::string &str){
		cout << "element " << str << endl;
	}

	void handle_descendant(){
		cout << "descendant" << endl;
	}

	void handle_child(){
		cout << "child" << endl;
	}
};

void handle_class(const std::string &str){
	cout << "class " << str << endl;
}

void handle_id(const std::string &str){
	cout << "id " << str << endl;
}

void handle_contains(const std::string &str){
	cout << "contains " << str << endl;
}

void handle_attr(fusion::vector<std::string, std::string> v){
	cout << "attr" << fusion::at_c<0>(v) << ":" << fusion::at_c<1>(v) << endl;
}

void handle_element(const std::string &str){
	cout << "element " << str << endl;
}

void handle_descendant(){
	cout << "descendant" << endl;
}

void handle_child(){
	cout << "child" << endl;
}

int main(){
	using spirit::ascii::alnum;
	using spirit::ascii::char_;
	using spirit::qi::int_;
	using spirit::lit;
	using spirit::omit;
	using spirit::no_skip;
	using spirit::eps;
	typedef spirit::qi::rule<std::string::const_iterator, std::string(), spirit::ascii::space_type> BasicSelectorRule;
	typedef spirit::qi::rule<std::string::const_iterator, fusion::vector<std::string, std::string>(), spirit::ascii::space_type> ParenthesisedOperator;
	typedef spirit::qi::rule<std::string::const_iterator> Symbol;
	typedef spirit::qi::rule<std::string::const_iterator, std::string()> StdRule;
	typedef spirit::qi::rule<std::string::const_iterator, fusion::vector<std::string, std::string>()> AttributeRule;
	Symbol special_chars = no_skip[lit('.') | '#' | ':'  | '[' | ' ' | '>'];
	//parenthesised_operator = ':' >> (spirit::string("has") | spirit::string("contains") | spirit::string("not")) >> '(' >> +(char_-')') >> ')';
	//post_operator = (' ')[handle_descendant_operator] | (str(" > ") | " >" | str("> ") | str(">")[handle_child_operator()]);
	Handlers h;
	StdRule element = +(char_-special_chars);
	StdRule class_ = lit('.') >> +(char_-special_chars);
	StdRule id = '#' >> +(char_-special_chars) >> eps;
	StdRule contains = ":contains(" >> +(char_-(special_chars | ')')) >> ')';
	AttributeRule attr = '[' >> +(char_-(special_chars | '=')) >> '=' >> +(char_-']') >> ']';
	//StdRule has = ":has(" >> +(char_-')') >> ')';
	
	StdRule pre_operator = +(class_[&handle_class] | id[&handle_id] | contains[boost::bind(&Handlers::handle_contains, &h, _1)] | attr[&handle_attr] | element[&handle_element] | lit(" > ")[&handle_child] | (lit(' '))[&handle_descendant]);
//	BasicSelectorRule selector = +(-('.' | '#' | (':' >> alnum )) >> alnum >> (char_(' ') | string(" > ") | ))
	std::string in = "p:contains(tekst).special #my_id > div";
	std::string::const_iterator begin = in.begin(), end = in.end();
	spirit::qi::parse(begin, end, pre_operator);

}
