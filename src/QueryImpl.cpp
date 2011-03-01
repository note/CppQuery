#include <vector>
#include <boost/bind.hpp>

#include "QueryImpl.h"

using namespace CppQuery;

QueryImpl::QueryImpl(const string &html){
	using spirit::ascii::alpha;
	using spirit::ascii::alnum;
	using spirit::ascii::char_;
	using spirit::no_skip;
	using spirit::omit;
	using spirit::lit;
	using boost::bind;

	HtmlAttributeRule attribute = +(char_-(char_('=') | '>' | '<')) >> '=' >> -(lit('\'') | '"')  >> *(char_-(lit('\'') | '"')) >> -(lit('\'') | '"');
 	HtmlStartTagRule start_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> '>';
	HtmlStartTagRule start_end_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> lit("/>");
	HtmlEndTagRule end_tag = no_skip[lit("</") >> +(alnum)] >> '>';

	HtmlRule html_rule = *(omit[*(char_-'<')] >> (start_tag[bind(&QueryImpl::handle_start_tag, this, _1)] | end_tag[bind(&QueryImpl::handle_end_tag, this, _1)] | start_end_tag[bind(&QueryImpl::handle_start_end_tag, this, _1)]));
}

void QueryImpl::handle_start_tag(HtmlStartTagAttr &tag){

}

void QueryImpl::handle_start_end_tag(HtmlStartTagAttr &tag){

}

void QueryImpl::handle_end_tag(const string &tag){

}
