#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/algorithm.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include <boost/bind.hpp>

namespace spirit = boost::spirit;
using spirit::qi::raw;
using spirit::ascii::alpha;
using spirit::ascii::alnum;
using spirit::ascii::char_;
using spirit::no_skip;
using spirit::omit;
using spirit::lit;
using boost::bind;
namespace fusion = boost::fusion;
//using namespace boost::phoenix;
//using namespace boost::phoenix::arg_names;


typedef fusion::vector<std::string, std::vector<fusion::vector<std::string, std::string> > > HtmlTagAttr;
typedef std::vector<HtmlTagAttr> HtmlAttr;
typedef spirit::qi::rule<std::string::iterator, fusion::vector<std::string, std::string>(), spirit::ascii::space_type> HtmlAttributeRule;
typedef spirit::qi::rule<std::string::iterator, HtmlTagAttr(), spirit::ascii::space_type> HtmlTagRule;
typedef spirit::qi::rule<std::string::iterator, std::string(), spirit::ascii::space_type> HtmlEndTagRule;
//typedef spirit::qi::rule<std::string::iterator, HtmlAttr(), spirit::ascii::space_type> HtmlRule;
typedef spirit::qi::rule<std::string::iterator, spirit::ascii::space_type> HtmlRule;

void print_attr(const fusion::vector<std::string, std::string> &v){
	std::cout << fusion::at_c<0>(v) << std::endl << fusion::at_c<1>(v) << std::endl;
}

void print_attrs(const std::vector<fusion::vector<std::string, std::string> > &v){
	for(int i=0; i<v.size(); ++i)
		print_attr(v[i]);
}

void print_tag(HtmlTagAttr &tag){
	std::cout << fusion::at_c<0>(tag) << std::endl;
	print_attrs(fusion::at_c<1>(tag));
}

void test_tag(std::string &in, HtmlTagAttr &res){
	using spirit::ascii::blank;
	using spirit::ascii::space;
 	HtmlAttributeRule attribute;
 	HtmlTagRule start_tag;
 	attribute = +(char_-'=') >> '=' >> -(lit('\'') | '"')  >> *(char_-(lit('\'') | '"')) >> -(lit('\'') | '"');
 	start_tag %= no_skip['<' >> +(char_-(blank | '>' | '/')) >> blank] >> *(attribute) >> '>';
 	
 	spirit::qi::phrase_parse(in.begin(), in.end(), start_tag, space, res);
 	std::cout << fusion::at_c<0>(res) << std::endl;
 	print_attrs(fusion::at_c<1>(res));
}

void handle_start_tag(HtmlTagAttr &tag){
	std::cout << "Beginning of tag: " << fusion::at_c<0>(tag) << std::endl;
}

struct Creator{
	void handle_start_tag(HtmlTagAttr &tag){
		std::cout << "Beginning of tag: " << fusion::at_c<0>(tag) << std::endl;
	}

	void handle_end_tag(std::string &tag){
		std::cout << "End of tag: " << tag << std::endl;
	}

	void handle_start_end_tag(HtmlTagAttr &tag){
		std::cout << "Beginning and end of tag: " << fusion::at_c<0>(tag) << std::endl;
	}
};

void test_html(std::string &in, HtmlAttr &res){
	using namespace std;
	using spirit::ascii::space;
	//HtmlRule html;
	HtmlAttributeRule attribute;
 	HtmlTagRule start_tag, start_end_tag;
	HtmlEndTagRule end_tag;
 	
	Creator creator;

	attribute = +(char_-(char_('=') | '>' | '<')) >> '=' >> -(lit('\'') | '"')  >> *(char_-(lit('\'') | '"')) >> -(lit('\'') | '"');
 	start_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> '>';
	start_end_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> lit("/>");
	end_tag = no_skip[lit("</") >> +(alnum)] >> '>';

	HtmlRule html = *(omit[*(char_-'<')] >> (start_tag[bind(&Creator::handle_start_tag, &creator, _1)] | end_tag[bind(&Creator::handle_end_tag, &creator, _1)] | start_end_tag[bind(&Creator::handle_start_end_tag, &creator, _1)]));
//	html = *(omit[*(char_-'<')] >> (start_end_tag[bind(&Creator::handle_start_end_tag, &creator, _1)]));
	//html = *(omit[*(char_-'<')] >> end_tag);
//	html = *(omit[*(char_-'<')] >> '<' >> (+(char_-'>'))[&handle_start_tag2] >> '>');

	spirit::qi::phrase_parse(in.begin(), in.end(), html, space);
}

int main(){
	HtmlTagAttr res, res2, res3;
	
	std::string in = "<p id=\"logo\">";
	std::string in2 = "<h3 id='logo' class=\"special\">";
	std::string in3 = "<  p>"; //should not be parsed correctly

// 	test_tag(in, res);
// 	test_tag(in2, res2);
// 	test_tag(in3, res3);

	HtmlAttr out; 
	in = "<p>fsdafsa<h2 id=\"logo\" class=\"classname\"></h2><img href='example.gif' /></p>";
	test_html(in, out);
}
