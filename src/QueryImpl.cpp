#include <vector>
#include <iostream>
#include <boost/bind.hpp>

#include "QueryImpl.h"

using namespace std;
using namespace CppQuery;

namespace qi = spirit::qi;

extern template class Node<string>;
extern template class Node<wstring>;

template <typename Str>
QueryImpl<Str>::QueryImpl(const Str &html){
	using spirit::no_skip;
	using spirit::omit;
	using spirit::lit;
	using boost::bind;
	
	roots.push(vector<NodePtr>());

	HtmlAttributeRule attribute = +(Chars<Str>::char_-(Chars<Str>::char_('=') | '>' | '<')) >> '=' >> -(lit('\'') | '"')  >> *(Chars<Str>::char_-(lit('\'') | '"')) >> -(lit('\'') | '"');
 	HtmlStartTagRule start_tag = no_skip['<' >> +(Chars<Str>::alnum)] >> *(attribute) >> '>';
	HtmlStartTagRule start_end_tag = no_skip['<' >> +(Chars<Str>::alnum)] >> *(attribute) >> lit("/>");
	HtmlEndTagRule end_tag = no_skip[lit("</") >> +(Chars<Str>::alnum)] >> '>';
	HtmlEndTagRule text = no_skip[+(Chars<Str>::char_-'<')];
	HtmlRule html_rule = *(-text[bind(&QueryImpl::handle_text, this, _1)] >> (start_tag[bind(&QueryImpl::handle_start_tag, this, _1)] | end_tag[bind(&QueryImpl::handle_end_tag, this, _1)] | start_end_tag[bind(&QueryImpl::handle_start_end_tag, this, _1)]));

	typename Str::const_iterator begin = html.begin(), end = html.end();
	qi::phrase_parse(begin, end, html_rule, Chars<Str>::space);
}

template<typename Str>
Str QueryImpl<Str>::text(){
	Str res;
	for(int i = 0; i < roots.top().size(); ++i)
		res += roots.top()[i]->get_text();
	return res;
}

template<typename Str>
Str QueryImpl<Str>::get_attribute(const Str &attribute){
	Str res;
	for(int i = 0; i < roots.top().size(); ++i)
		res += roots.top()[i]->get_attribute(attribute);
	return res;
}

template<typename Str>
bool QueryImpl<Str>::attr_exists(const Str &attribute){
	for(size_t i = 0; i < roots.top().size(); ++i)
		if(roots.top()[i]->attr_exists(attribute))
			return true;

	return false;
}

template<typename Str>
QueryImpl<Str> * QueryImpl<Str>::get_ith(int index){
	if(index<roots.top().size()){
		vector<NodePtr> selected(1);
		selected.push_back(roots.top()[index]);
		return new QueryImpl(selected);
	}else
		return new QueryImpl();
}

template<typename Str>
QueryImpl<Str> * QueryImpl<Str>::select(const Str &selector){
	reset();
	//cout << "HERE" << endl;
	//tmp_res.top().clear();
	tmp_res.push(roots.top());
	using qi::int_;
	using spirit::lit;
	using spirit::omit;
	using spirit::no_skip;
	using spirit::eps;

using boost::bind;
	typedef qi::rule<typename Str::const_iterator, Str, typename CharsTypes<Str>::space_type> BasicSelectorRule;
	typedef qi::rule<typename Str::const_iterator, fusion::vector<Str, Str>(), typename CharsTypes<Str>::space_type> ParenthesisedOperator;
	typedef qi::rule<typename Str::const_iterator> Symbol;
	typedef qi::rule<typename Str::const_iterator, Str()> StdRule;
	typedef qi::rule<typename Str::const_iterator, fusion::vector<Str, Str>()> AttributeRule;
	typedef qi::rule<typename Str::const_iterator, qi::unused_type()> UnusedTypeRule;
	Symbol special_chars = no_skip[lit('.') | '#' | ':'  | '[' | ' ' | '>' | ')'];
	StdRule selectors;
	StdRule element = +(Chars<Str>::char_-special_chars);
	StdRule class_ = lit('.') >> +(Chars<Str>::char_-special_chars);
	StdRule id = '#' >> +(Chars<Str>::char_-special_chars);
	StdRule contains = ":contains(" >> +(Chars<Str>::char_-(special_chars | ')')) >> ')';
	AttributeRule attr = '[' >> +(Chars<Str>::char_-(special_chars | '=')) >> '=' >> +(Chars<Str>::char_-']') >> ']';
	StdRule not_ = lit(":not(")[bind(&QueryImpl::handle_start_not, this)] >> spirit::omit[selectors] >> lit(')')[bind(&QueryImpl::handle_end_not, this)];
	StdRule has_ = lit(":has(")[bind(&QueryImpl::handle_start_has, this)] >> spirit::omit[selectors] >> lit(')')[bind(&QueryImpl::handle_end_has, this)];
	
 	selectors = +(class_[bind(&QueryImpl::handle_class, this, _1)] | id[bind(&QueryImpl::handle_id, this, _1)] | contains[bind(&QueryImpl::handle_contains, this, _1)] | 
 	attr[bind(&QueryImpl::handle_attr, this, _1)] | element[bind(&QueryImpl::handle_element, this, _1)] | lit(" > ")[bind(&QueryImpl::handle_child, this)] |
 	(lit(' '))[bind(&QueryImpl::handle_descendant, this)] | not_ | has_);
 	typename Str::const_iterator begin = selector.begin(), end = selector.end();
	qi::parse(begin, end, selectors);


	QueryImpl * tmp = new QueryImpl(tmp_res.top());
	tmp_res.pop();
	return tmp;
}

template<typename Str>
void QueryImpl<Str>::handle_start_tag(HtmlStartTagAttr &tag){
	//std::cout << "Beginning of tag: ";// << fusion::at_c<0>(tag) << std::endl;
	Node<Str> *new_element_ptr = new Node<Str>(tag);
	NodePtr new_element = new_element_ptr->get_shared_ptr();

	if(open_tags.size())
		open_tags.top()->add_child(new_element);
	else
		roots.top().push_back(new_element);
	
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
		roots.top().push_back(new_element);
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
		for(int i=0; i<tmp_res.top().size(); ++i)
			tmp_res.top()[i]->search_with_text(txt, res);
		tmp_res.top() = res;
	}else if(descendant){
		//remove all nodes that have parents among vector v
		//eg. <a1><a2><a3></a3></a2></a1> - if we are searching a3 inside a1 then searching inside a2 is redundant
		remove_children(tmp_res.top());

		vector<NodePtr> res;
		for(int i=0; i<tmp_res.top().size(); ++i)
			tmp_res.top()[i]->search_inside_with_text(txt, res);
		tmp_res.top() = res;

		descendant = false; //descendant's been handled - reset flag
	}else{
		NotMatchingText<Str> predicat(txt);
		tmp_res.top().erase(remove_if(tmp_res.top().begin(), tmp_res.top().end(), predicat), tmp_res.top().end());
	}

	first_selector = false;
}

template<typename Str>
void QueryImpl<Str>::handle_attr(fusion::vector<Str, Str> attr_v){
	//cout << "attr";// << fusion::at_c<0>(attr_v) << ":" << fusion::at_c<1>(attr_v) << endl;
	
wcout << L"attr" << tmp_res.top().size() << endl;
	if(first_selector){
		vector<NodePtr> res;
		for(int i=0; i<tmp_res.top().size(); ++i)
			tmp_res.top()[i]->search_by_attribute(fusion::at_c<0>(attr_v), fusion::at_c<1>(attr_v), res);
		tmp_res.top() = res;
	}else if(descendant){
		//remove all nodes that have parents among vector v
		//eg. <a1><a2><a3></a3></a2></a1> - if we are searching a3 inside a1 then searching inside a2 is redundant
		remove_children(tmp_res.top());

		vector<NodePtr> res;
		for(int i=0; i<tmp_res.top().size(); ++i)
			tmp_res.top()[i]->search_inside_by_attribute(fusion::at_c<0>(attr_v), fusion::at_c<1>(attr_v), res);
		tmp_res.top() = res;

		descendant = false; //descendant's been handled - reset flag
	}else{
		NotMatchingAttr<Str> predicat(fusion::at_c<0>(attr_v), fusion::at_c<1>(attr_v));
		tmp_res.top().erase(remove_if(tmp_res.top().begin(), tmp_res.top().end(), predicat), tmp_res.top().end());
	}
wcout << tmp_res.top().size() << endl;
	first_selector = false;
}

//before call v contains elements that are roots of elements that should be searched
//after call v contains results
template<typename Str>
void QueryImpl<Str>::handle_element(const Str &el_name){
	if(first_selector){
		vector<NodePtr> res;
		for(int i=0; i<tmp_res.top().size(); ++i)
			tmp_res.top()[i]->search_by_tag_name(el_name, res);
		tmp_res.top() = res;
	}else if(descendant){
		//remove all nodes that have parents among vector v
		//eg. <a1><a2><a3></a3></a2></a1> - if we are searching a3 inside a1 then searching inside a2 is redundant
		remove_children(tmp_res.top());

		vector<NodePtr> res;
		for(int i=0; i<tmp_res.top().size(); ++i)
			tmp_res.top()[i]->search_inside_by_tag_name(el_name, res);
		tmp_res.top() = res;

		descendant = false; //descendant's been handled - reset flag
	}else{
		NotMatchingElement<Str> predicat(el_name);
		tmp_res.top().erase(remove_if(tmp_res.top().begin(), tmp_res.top().end(), predicat), tmp_res.top().end());
	}
	//cout << tmp_res.top().size() << endl;
	first_selector = false;
}

template <typename Str>
void QueryImpl<Str>::diff(const QueryImpl<Str> * other, vector<NodePtr> & res){
	//vector<NodePtr> res;
	for(int i=0; i<tmp_res.top().size(); ++i)
		if(find(other->roots.top().begin(), other->roots.top().end(), tmp_res.top()[i]) != other->roots.top().end())
			res.push_back(tmp_res.top()[i]);
//	return res;
}

template<typename Str>
void QueryImpl<Str>::handle_start_not(){
	//it's quite subtle. it may seem that tmp_res.top() should be pushed, but consider eg. "p :not(#main :sp)"
	push_flags();
	tmp_res.push(roots.top());
	reset();
	wcout << L"HANDLE_START_NOT" << endl;
}


template<typename Str>
void QueryImpl<Str>::handle_end_not(){
	wcout << L"HANDLE_END_NOT" << endl;
	vector<NodePtr> another = tmp_res.top();
	tmp_res.pop();
	pop_flags();
	IsPresent<Str> predicat(another);
	
	vector<NodePtr> tmp;
	if(first_selector){
		remove_children(tmp_res.top());
		get_all(tmp, Node<Str>::Flags::roots | Node<Str>::Flags::descendants);
		tmp_res.top() = tmp;
	}
	
	if(descendant){
		remove_children(tmp_res.top());
		get_all(tmp, Node<Str>::Flags::descendants);
		tmp_res.top() = tmp;
	}
	
	tmp_res.top().erase(remove_if(tmp_res.top().begin(), tmp_res.top().end(), predicat), tmp_res.top().end());
}

template<typename Str>
void QueryImpl<Str>::handle_start_has(){
	wcout << L"HANDLE_START_HAS" << endl;
	push_flags();
	tmp_res.push(roots.top());
	reset();
}

//when that method is called in tmp_res.top() there are nodes found with expression :has()
template<typename Str>
void QueryImpl<Str>::handle_end_has(){
	wcout << L"HANDLE_END_HAS" << endl;
	vector<NodePtr> another;
	pop_flags();
	
	if(first_selector){
		get_ancestors(another); // get ancestors of nodes in tmp_res.top()
		tmp_res.pop(); // we don't need nodes found with expression :has() anymore
		tmp_res.top() = another;
	}else if(descendant){
		vector<NodePtr> res;
		another = tmp_res.top();
		tmp_res.pop();
		get_ancestors_inside(another, res);
		tmp_res.top() = res;
	}else{
		another = tmp_res.top();
		tmp_res.pop();
		HaveNotChildrenAmong<Str> predicat(another);
		tmp_res.top().erase(remove_if(tmp_res.top().begin(), tmp_res.top().end(), predicat), tmp_res.top().end());
	}
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

