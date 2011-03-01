#include <vector>
#include <iostream>
#include <boost/bind.hpp>

#include "QueryImpl.h"

using namespace CppQuery;
using namespace std;

QueryImpl::QueryImpl(const string &html){
	using spirit::ascii::alpha;
	using spirit::ascii::alnum;
	using spirit::ascii::char_;
	using spirit::ascii::space;
	using spirit::no_skip;
	using spirit::omit;
	using spirit::lit;
	using boost::bind;
	
	//if html document that is being parsed is correct then it should have root element. To handle incorrect document correctly root is arbitrarily set.
	//Then it should be ignored by query methods
	root = NodePtr(new Node());

	HtmlAttributeRule attribute = +(char_-(char_('=') | '>' | '<')) >> '=' >> -(lit('\'') | '"')  >> *(char_-(lit('\'') | '"')) >> -(lit('\'') | '"');
 	HtmlStartTagRule start_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> '>';
	HtmlStartTagRule start_end_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> lit("/>");
	HtmlEndTagRule end_tag = no_skip[lit("</") >> +(alnum)] >> '>';

	HtmlRule html_rule = *(omit[*(char_-'<')] >> (start_tag[bind(&QueryImpl::handle_start_tag, this, _1)] | end_tag[bind(&QueryImpl::handle_end_tag, this, _1)] | start_end_tag[bind(&QueryImpl::handle_start_end_tag, this, _1)]));
	
	open_tags.push(root);
	string::const_iterator begin = html.begin(), end = html.end();
	spirit::qi::phrase_parse(begin, end, html_rule, space);
}

void QueryImpl::handle_start_tag(HtmlStartTagAttr &tag){
	std::cout << "Beginning of tag: " << fusion::at_c<0>(tag) << std::endl;
	NodePtr new_element(new Node(tag));
	
	open_tags.top()->addChild(new_element);
	open_tags.push(new_element);
}

void QueryImpl::handle_start_end_tag(HtmlStartTagAttr &tag){
	std::cout << "Beginning and end of tag: " << fusion::at_c<0>(tag) << std::endl;
	NodePtr new_element(new Node(tag));
	
	open_tags.top()->addChild(new_element);
}

void QueryImpl::handle_end_tag(const string &tag){
	std::cout << "End of tag: " << tag << std::endl;
	if(open_tags.size() > 1) //it always should be one tag open - root. If open_tags.size()==1 and end tags was occured it means that html document is incorrect.
	    open_tags.pop();
}
