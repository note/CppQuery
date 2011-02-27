#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/algorithm.hpp>
#include <vector>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

namespace spirit = boost::spirit;
using spirit::qi::raw;
using spirit::ascii::alpha;
using spirit::ascii::alnum;
using spirit::ascii::char_;
using spirit::lit;
using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;
namespace fusion = boost::fusion;

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
	fusion::for_each(v, arg1 = "");
 
 	qi::phrase_parse(in2.begin(), in2.end(), attribute, ascii::space, v);
	print_v(v);
	fusion::for_each(v, arg1 = "");

 	qi::phrase_parse(in3.begin(), in3.end(), attribute, ascii::space, v);
	print_v(v);
	fusion::for_each(v, arg1 = "");
}
