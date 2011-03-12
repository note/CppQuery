#include "Node.h"

using namespace CppQuery;
using namespace std;

template<typename Str>
Node<Str>::Node(HtmlStartTagAttr &tag){
	ptr = NodePtr(this);
	element_name = fusion::at_c<0>(tag);
	vector<fusion::vector<Str, Str> > attributes_vector = fusion::at_c<1>(tag);
	for(int i=0; i<attributes_vector.size(); ++i)
		attributes [fusion::at_c<0>(attributes_vector[i])] = fusion::at_c<1>(attributes_vector[i]);
}

template<typename Str>
Node<Str>::Node(const Str &tag_name, std::vector<Str> attr_names, std::vector<Str> attr_values){
	assert(attr_names.size() == attr_values.size());
	ptr = NodePtr(this);
	element_name = tag_name;
	for(int i=0; i<attr_names.size(); ++i)
		attributes [attr_names[i]] = attr_values[i];
}

template<typename Str>
Str Node<Str>::get_attribute(const Str &attribute){
	typename map<Str, Str>::iterator it = attributes.find(attribute);
	if(it!=attributes.end())
		return (*it).second;
	return Str();
}

template<typename Str>
bool Node<Str>::attr_exists(const Str &attribute){
	return attributes.find(attribute) != attributes.end();
}

//begin of "has_" methods
template<typename Str>
bool Node<Str>::has_tag_name(const Str &tag_name){
	return (element_name == tag_name);
}


template<typename Str>
bool Node<Str>::has_attribute_value(const Str &attribute_name, const Str &attribute_value){	
	typename map<Str, Str>::iterator attr_it = attributes.find(attribute_name);
	if(attr_it!=attributes.end())
		if((*attr_it).second == attribute_value)
			return true;
	return false;
}

template<typename Str>
bool Node<Str>::has_text(const Str &txt){
	return (text.find(txt) != Str::npos);
}
//end of "has_" methods

template<typename Str>
void Node<Str>::search_by_tag_name(const Str &tag_name, vector<NodePtr> &v){
	search_by_tag_name_(tag_name, v, false);
}

template<typename Str>
void Node<Str>::search_by_attribute(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v){
	search_by_attribute_(attribute_name, attribute_value, v, false);
}

template<typename Str>
void Node<Str>::search_with_text(const Str &txt, vector<NodePtr> &v){
	search_with_text_(txt, v, false);
}

//search_inside* variants

template<typename Str>
void Node<Str>::search_inside_by_tag_name(const Str &txt, vector<NodePtr> &v){
	search_by_tag_name_(txt, v, true);
}

template<typename Str>
void Node<Str>::search_inside_by_attribute(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v){
	search_by_attribute_(attribute_name, attribute_value, v, true);
}

template<typename Str>
void Node<Str>::search_inside_with_text(const Str &txt, vector<NodePtr> &v){
	search_with_text_(txt, v, true);
}


template<typename Str>
void Node<Str>::search_among_children_by_tag_name(const Str &tag_name, std::vector<NodePtr> &v){
	search_by_tag_name_(tag_name, v, true, false);
}

template<typename Str>
void Node<Str>::search_among_children_by_attribute(const Str &attribute_name, const Str &attribute_value, std::vector<NodePtr> &v){
	search_by_attribute_(attribute_name, attribute_value, v, true, false);
}

template<typename Str>
void Node<Str>::search_among_children_with_text(const Str &text, std::vector<NodePtr> &v){
	search_with_text_(text, v, true, false);
}

//actual implementation of search_by_tag_name, search_by_attribute and search_with_text
//because search* and search_inside* variants differs only in the way they treat root element then each pair is implemented with one method:

template<typename Str>
void Node<Str>::search_by_tag_name_(const Str &tag_name, vector<NodePtr> &v, bool ignore_root, bool deep){
	//pre-order traversing
	if(!ignore_root && has_tag_name(tag_name))
		v.push_back(ptr);
	
	if(deep){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_by_tag_name_(tag_name, v);
	}else{
		for(int i = 0; i < children.size(); ++i)
			if(children[i]->has_tag_name(tag_name))
				v.push_back(children[i]->get_shared_ptr());
	}
}

template<typename Str>
void Node<Str>::search_by_attribute_(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v, bool ignore_root, bool deep){
	//pre-order traversing
	if(!ignore_root && has_attribute_value(attribute_name, attribute_value))
		v.push_back(ptr);
	
	if(deep){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_by_attribute_(attribute_name, attribute_value, v);
	}else{
		for(int i = 0; i < children.size(); ++i)
			if(children[i]->has_attribute_value(attribute_name, attribute_value))
				v.push_back(children[i]->get_shared_ptr());
	}
}

template<typename Str>
void Node<Str>::search_with_text_(const Str &txt, vector<NodePtr> &v, bool ignore_root, bool deep){
	if(!ignore_root && has_text(txt))
		v.push_back(ptr);
	
	if(deep){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_with_text_(txt, v);
	}else{
		for(int i = 0; i < children.size(); ++i)
			if(children[i]->has_text(txt))
				v.push_back(children[i]->get_shared_ptr());
	}
}


template<typename Str>
bool Node<Str>::is_descendant_of(const NodePtr &potential_parent){
	if(!parent)
		return false;

	if(parent == potential_parent)
		return true;

	return parent->is_descendant_of(potential_parent);
}

//see: http://www.parashift.com/c++-faq-lite/templates.html#faq-35.15
template class Node<string>;
template class Node<wstring>;
