#include <vector>
#include <iostream>
#include <boost/bind.hpp>

#include "QueryImpl.h"

using namespace CppQuery;
using namespace std;

extern template class Node<string>;
extern template class Node<wstring>;

template <typename Str>
QueryImpl<Str>::QueryImpl(const Str &html){
	using spirit::standard_wide::alpha;
	using spirit::standard_wide::alnum;
	using spirit::standard_wide::char_;
	using spirit::standard_wide::space;
	using spirit::no_skip;
	using spirit::omit;
	using spirit::lit;
	using boost::bind;
	

	HtmlAttributeRule attribute = +(char_-(char_('=') | '>' | '<')) >> '=' >> -(lit('\'') | '"')  >> *(char_-(lit('\'') | '"')) >> -(lit('\'') | '"');
 	HtmlStartTagRule start_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> '>';
	HtmlStartTagRule start_end_tag = no_skip['<' >> +(alnum)] >> *(attribute) >> lit("/>");
	HtmlEndTagRule end_tag = no_skip[lit("</") >> +(alnum)] >> '>';
HtmlEndTagRule text = no_skip[+(char_-'<')];
	//HtmlRule html_rule = *((*(char_-'<'))[bind(&QueryImpl::handle_text, this, _1)] >> (start_tag[bind(&QueryImpl::handle_start_tag, this, _1)] | end_tag[bind(&QueryImpl::handle_text, this, _1)] | start_end_tag[bind(&QueryImpl::handle_start_end_tag, this, _1)]));
	HtmlRule html_rule = *(-text[bind(&QueryImpl::handle_text, this, _1)] >> (start_tag[bind(&QueryImpl::handle_start_tag, this, _1)] | end_tag[bind(&QueryImpl::handle_end_tag, this, _1)] | start_end_tag[bind(&QueryImpl::handle_start_end_tag, this, _1)]));
	
	typename Str::const_iterator begin = html.begin(), end = html.end();
	spirit::qi::phrase_parse(begin, end, html_rule, space);
}

template<typename Str>
Str QueryImpl<Str>::text(){
	Str res;
	for(int i = 0; i < roots.size(); ++i)
		res += roots[i]->get_text();
	return res;
}

template<typename Str>
Str QueryImpl<Str>::get_attribute(const Str &attribute){
	Str res;
	for(int i = 0; i < roots.size(); ++i)
		res += roots[i]->get_attribute(attribute);
	return res;
}

template<typename Str>
bool QueryImpl<Str>::attr_exists(const Str &attribute){
	for(size_t i = 0; i < roots.size(); ++i)
		if(roots[i]->attr_exists(attribute))
			return true;

	return false;
}

template<typename Str>
QueryImpl<Str> * QueryImpl<Str>::get_ith(int index){
	if(index<roots.size()){
		vector<NodePtr> selected(1);
		selected.push_back(roots[index]);
		return new QueryImpl(selected);
	}else
		return new QueryImpl();
}

template<typename Str>
QueryImpl<Str> * QueryImpl<Str>::select(const Str &selector){
	vector<NodePtr> selected;
	for(int i = 0; i<roots.size(); ++i)
		roots[i]->select_by_tag_name(selector, selected);
	
	return new QueryImpl(selected);
}

template<typename Str>
void QueryImpl<Str>::handle_start_tag(HtmlStartTagAttr &tag){
	//std::cout << "Beginning of tag: " << fusion::at_c<0>(tag) << std::endl;
	Node<Str> *new_element_ptr = new Node<Str>(tag);
	NodePtr new_element = new_element_ptr->get_shared_ptr();

	if(open_tags.size())
		open_tags.top()->addChild(new_element);
	else
		roots.push_back(new_element);
	
	open_tags.push(new_element);
}

template<typename Str>
void QueryImpl<Str>::handle_start_end_tag(HtmlStartTagAttr &tag){
	//std::cout << "Beginning and end of tag: " << fusion::at_c<0>(tag) << std::endl;
	Node<Str> *new_element_ptr = new Node<Str>(tag);
	NodePtr new_element = new_element_ptr->get_shared_ptr();
	
	if(open_tags.size())
		open_tags.top()->addChild(new_element);
	else
		roots.push_back(new_element);
}

template<typename Str>
void QueryImpl<Str>::handle_end_tag(const Str &tag){
	//std::cout << "End of tag: " << tag << std::endl;
	if(open_tags.size() > 0) //it always should be bigger than zero. If open_tags.size()==0 and end tags was found it means that html document is incorrect.
	    open_tags.pop();
}

template<typename Str>
void QueryImpl<Str>::handle_text(const Str &text){
	//std::cout << "TEXT" << text << std::endl;
	if(open_tags.size())
		open_tags.top()->appendText(text);
}

template class QueryImpl<string>;
template class QueryImpl<wstring>;

