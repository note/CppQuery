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
	using spirit::no_skip;
	using spirit::omit;
	using spirit::lit;
	using boost::bind;
	

	HtmlAttributeRule attribute = +(Chars<Str>::char_-(Chars<Str>::char_('=') | '>' | '<')) >> '=' >> -(lit('\'') | '"')  >> *(Chars<Str>::char_-(lit('\'') | '"')) >> -(lit('\'') | '"');
 	HtmlStartTagRule start_tag = no_skip['<' >> +(Chars<Str>::alnum)] >> *(attribute) >> '>';
	HtmlStartTagRule start_end_tag = no_skip['<' >> +(Chars<Str>::alnum)] >> *(attribute) >> lit("/>");
	HtmlEndTagRule end_tag = no_skip[lit("</") >> +(Chars<Str>::alnum)] >> '>';
	HtmlEndTagRule text = no_skip[+(Chars<Str>::char_-'<')];
	HtmlRule html_rule = *(-text[bind(&QueryImpl::handle_text, this, _1)] >> (start_tag[bind(&QueryImpl::handle_start_tag, this, _1)] | end_tag[bind(&QueryImpl::handle_end_tag, this, _1)] | start_end_tag[bind(&QueryImpl::handle_start_end_tag, this, _1)]));

	typename Str::const_iterator begin = html.begin(), end = html.end();
	spirit::qi::phrase_parse(begin, end, html_rule, Chars<Str>::space);
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
	reset();
	//cout << "HERE" << endl;
	//v.clear();
	v = roots;
	using spirit::qi::int_;
	using spirit::lit;
	using spirit::omit;
	using spirit::no_skip;
	using spirit::eps;

using boost::bind;
	typedef spirit::qi::rule<typename Str::const_iterator, Str, typename CharsTypes<Str>::space_type> BasicSelectorRule;
	typedef spirit::qi::rule<typename Str::const_iterator, fusion::vector<Str, Str>(), typename CharsTypes<Str>::space_type> ParenthesisedOperator;
	typedef spirit::qi::rule<typename Str::const_iterator> Symbol;
	typedef spirit::qi::rule<typename Str::const_iterator, Str()> StdRule;
	typedef spirit::qi::rule<typename Str::const_iterator, fusion::vector<Str, Str>()> AttributeRule;
	Symbol special_chars = no_skip[lit('.') | '#' | ':'  | '[' | ' ' | '>'];
	StdRule element = +(Chars<Str>::char_-special_chars);
	StdRule class_ = lit('.') >> +(Chars<Str>::char_-special_chars);
	StdRule id = '#' >> +(Chars<Str>::char_-special_chars);
	StdRule contains = ":contains(" >> +(Chars<Str>::char_-(special_chars | ')')) >> ')';
	AttributeRule attr = '[' >> +(Chars<Str>::char_-(special_chars | '=')) >> '=' >> +(Chars<Str>::char_-']') >> ']';
	
 	StdRule pre_operator = +(class_[bind(&QueryImpl::handle_class, this, _1)] | id[bind(&QueryImpl::handle_id, this, _1)] | contains[bind(&QueryImpl::handle_contains, this, _1)] | attr[bind(&QueryImpl::handle_attr, this, _1)] | element[bind(&QueryImpl::handle_element, this, _1)] | lit(" > ")[bind(&QueryImpl::handle_child, this)] | (lit(' '))[bind(&QueryImpl::handle_descendant, this)]);
 	typename Str::const_iterator begin = selector.begin(), end = selector.end();
	spirit::qi::parse(begin, end, pre_operator);



	return new QueryImpl(v);
}

template<typename Str>
void QueryImpl<Str>::handle_start_tag(HtmlStartTagAttr &tag){
	//std::cout << "Beginning of tag: ";// << fusion::at_c<0>(tag) << std::endl;
	Node<Str> *new_element_ptr = new Node<Str>(tag);
	NodePtr new_element = new_element_ptr->get_shared_ptr();

	if(open_tags.size())
		open_tags.top()->add_child(new_element);
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
		open_tags.top()->add_child(new_element);
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
	//std::cout << "TEXT";// << text << std::endl;
	if(open_tags.size())
		open_tags.top()->append_text(text);
}

template<typename Str>
void QueryImpl<Str>::reset(){
	first_selector = true;
	descendant = false;
}

template<typename Str>
void QueryImpl<Str>::remove_children(vector<NodePtr> &vec){
	HaveParentAmong<Str> predicat(vec);
	vec.erase(remove_if(vec.begin(), vec.end(), predicat), vec.end());
}

namespace CppQuery{
template<>
void QueryImpl<string>::handle_class(const string &str){
	fusion::vector<string, string> attr_v("class", str);
	handle_attr(attr_v);
}

template<>
void QueryImpl<wstring>::handle_class(const wstring &str){
	fusion::vector<wstring, wstring> attr_v(L"class", str);
	handle_attr(attr_v);
}

template<>
void QueryImpl<string>::handle_id(const string &str){
	fusion::vector<string, string> attr_v("id", str);
	handle_attr(attr_v);
}

template<>
void QueryImpl<wstring>::handle_id(const wstring &str){
	fusion::vector<wstring, wstring> attr_v(L"id", str);
	handle_attr(attr_v);
}
} //end of namespace CppQuery

template<typename Str>
void QueryImpl<Str>::handle_contains(const Str &txt){
	//cout << "contains ";// << str << endl;

	if(first_selector){
		vector<NodePtr> res;
		for(int i=0; i<v.size(); ++i)
			v[i]->search_with_text(txt, res);
		v = res;
	}else if(descendant){
		//remove all nodes that have parents among vector v
		//eg. <a1><a2><a3></a3></a2></a1> - if we are searching a3 inside a1 then searching inside a2 is redundant
		remove_children(v);

		vector<NodePtr> res;
		for(int i=0; i<v.size(); ++i)
			v[i]->search_inside_with_text(txt, res);
		v = res;

		descendant = false; //descendant's been handled - reset flag
	}else{
		NotMatchingText<Str> predicat(txt);
		v.erase(remove_if(v.begin(), v.end(), predicat), v.end());
	}

	first_selector = false;
}

template<typename Str>
void QueryImpl<Str>::handle_attr(fusion::vector<Str, Str> attr_v){
	//cout << "attr";// << fusion::at_c<0>(attr_v) << ":" << fusion::at_c<1>(attr_v) << endl;
	
wcout << L"attr" << v.size() << endl;
	if(first_selector){
		vector<NodePtr> res;
		for(int i=0; i<v.size(); ++i)
			v[i]->search_by_attribute(fusion::at_c<0>(attr_v), fusion::at_c<1>(attr_v), res);
		v = res;
	}else if(descendant){
		//remove all nodes that have parents among vector v
		//eg. <a1><a2><a3></a3></a2></a1> - if we are searching a3 inside a1 then searching inside a2 is redundant
		remove_children(v);

		vector<NodePtr> res;
		for(int i=0; i<v.size(); ++i)
			v[i]->search_inside_by_attribute(fusion::at_c<0>(attr_v), fusion::at_c<1>(attr_v), res);
		v = res;

		descendant = false; //descendant's been handled - reset flag
	}else{
		NotMatchingAttr<Str> predicat(fusion::at_c<0>(attr_v), fusion::at_c<1>(attr_v));
		v.erase(remove_if(v.begin(), v.end(), predicat), v.end());
	}
wcout << v.size() << endl;
	first_selector = false;
}

//before call v contains elements that are roots of elements that should be searched
//after call v contains results
template<typename Str>
void QueryImpl<Str>::handle_element(const Str &el_name){
	if(first_selector){
		vector<NodePtr> res;
		for(int i=0; i<v.size(); ++i)
			v[i]->search_by_tag_name(el_name, res);
		v = res;
	}else if(descendant){
		//remove all nodes that have parents among vector v
		//eg. <a1><a2><a3></a3></a2></a1> - if we are searching a3 inside a1 then searching inside a2 is redundant
		remove_children(v);

		vector<NodePtr> res;
		for(int i=0; i<v.size(); ++i)
			v[i]->search_inside_by_tag_name(el_name, res);
		v = res;

		descendant = false; //descendant's been handled - reset flag
	}else{
		NotMatchingElement<Str> predicat(el_name);
		v.erase(remove_if(v.begin(), v.end(), predicat), v.end());
	}
	//cout << v.size() << endl;
	first_selector = false;
}

template<typename Str>
void QueryImpl<Str>::handle_descendant(){
	wcout << L"desc_handler" << endl;
	descendant = true;
}

template<typename Str>
void QueryImpl<Str>::handle_child(){
	cout << "child" << endl;
}


template class QueryImpl<string>;
template class QueryImpl<wstring>;

