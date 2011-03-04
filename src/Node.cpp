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
	//pre-order traversing
	if(element_name == tag_name)
		v.push_back(ptr);
			
	for(int i = 0; i < children.size(); i++)
		children[i]->select_by_tag_name(tag_name, v);
}

template<typename Str>
void Node<Str>::select_by_attribute(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v){
	//pre-order traversing
	typename map<Str, Str>::iterator attr_it = attributes.find(attribute_name);
	if(attr_it!=attributes.end())
		if((*attr_it).second == attribute_value)
			v.push_back(ptr);

	for(int i = 0; i < children.size(); i++)
		select_by_attribute(attribute_name, attribute_value, v);
}

template class Node<string>;
template class Node<wstring>;

