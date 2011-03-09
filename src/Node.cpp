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

template<typename Str>
void Node<Str>::select_by_tag_name(const Str &tag_name, vector<NodePtr> &v){
	select_by_tag_name_(tag_name, v, false);
}

template<typename Str>
void Node<Str>::select_by_attribute(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v){
	select_by_attribute_(attribute_name, attribute_value, v, false);
}

template<typename Str>
void Node<Str>::select_with_text(const Str &txt, vector<NodePtr> &v){
	select_with_text_(txt, v, false);
}

//search_inside* variants

template<typename Str>
void Node<Str>::search_inside_by_tag_name(const Str &txt, vector<NodePtr> &v){
	select_by_tag_name_(txt, v, true);
}

template<typename Str>
void Node<Str>::search_inside_by_attribute(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v){
	select_by_attribute_(attribute_name, attribute_value, v, true);
}

template<typename Str>
void Node<Str>::search_inside_with_text(const Str &txt, vector<NodePtr> &v){
	select_with_text_(txt, v, true);
}

//actual implementation of select_by_tag_name, select_by_attribute and select_with_text
//because select* and search_inside* variants differs only in the way they treat root element then each pair is implemented with one method:

template<typename Str>
void Node<Str>::select_by_tag_name_(const Str &tag_name, vector<NodePtr> &v, bool root){
	//pre-order traversing
	if(!root)
		if(element_name == tag_name)
			v.push_back(ptr);
			
	for(int i = 0; i < children.size(); ++i)
		children[i]->select_by_tag_name_(tag_name, v);
}

template<typename Str>
void Node<Str>::select_by_attribute_(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v, bool root){
	//pre-order traversing
	typename map<Str, Str>::iterator attr_it = attributes.find(attribute_name);
	if(!root)
		if(attr_it!=attributes.end())
			if((*attr_it).second == attribute_value)
				v.push_back(ptr);

	for(int i = 0; i < children.size(); ++i)
		children[i]->select_by_attribute_(attribute_name, attribute_value, v);
}

template<typename Str>
void Node<Str>::select_with_text_(const Str &txt, vector<NodePtr> &v, bool root){
	if(!root)
		if(text.find(txt) != Str::npos)
			v.push_back(ptr);
	
	for(int i = 0; i < children.size(); ++i)
		children[i]->select_with_text_(txt, v);
}

//see: http://www.parashift.com/c++-faq-lite/templates.html#faq-35.15
template class Node<string>;
template class Node<wstring>;

