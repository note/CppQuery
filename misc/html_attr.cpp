#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/container/vector.hpp>
//#include <boost/fusion/include/at_c.hpp>
//#include <boost/fusion/include/clear.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/algorithm.hpp>
//#include <boost/fusion/iterator/next.hpp>

namespace spirit = boost::spirit;
using spirit::qi::raw;
using spirit::ascii::alpha;
using spirit::ascii::alnum;
using spirit::ascii::char_;
using spirit::lit;
namespace fusion = boost::fusion;

struct empty_string{
	void operator () (std::string &str) const {
		str = "";
	}
};

void print_v(const fusion::vector<std::string, std::string> &v){
	std::cout << fusion::at_c<0>(v) << std::endl << fusion::at_c<1>(v) << std::endl;
}

int main(){
	using namespace spirit;
	typedef qi::rule<std::string::iterator, fusion::vector<std::string, std::string>(), ascii::space_type> HtmlAttribute;
	
	fusion::vector<std::string, std::string> v;
	HtmlAttribute attribute;
	attribute %= +(char_-'=') >> '=' >> -(lit('\'') | '"')  >> *(char_-(lit('\'') | '"')) >> -(lit('\'') | '"');

	std::string in = "id=\"logo\"";
	std::string in2 = "id='logo'";
	std::string in3 = "id=logo";

//	std::string::const_iterator begin = in.v
	qi::phrase_parse(in.begin(), in.end(), attribute, ascii::space, v);
	print_v(v);
	fusion::for_each(v, empty_string());
 
 	qi::phrase_parse(in2.begin(), in2.end(), attribute, ascii::space, v);
	print_v(v);
	fusion::for_each(v, empty_string());

 	qi::phrase_parse(in3.begin(), in3.end(), attribute, ascii::space, v);
	print_v(v);
	fusion::for_each(v, empty_string());
}
