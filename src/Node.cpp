#include "Node.h"

using namespace CppQuery;
using namespace std;

namespace CppQuery{
template<>
Node<string>::Node(HtmlStartTagAttr &tag){
	ptr = NodePtr(this);
	string tmp = fusion::at_c<0>(tag);	
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));
	element_name = tmp;
	vector<fusion::vector<string, string> > attributes_vector = fusion::at_c<1>(tag);
	for(int i=0; i<attributes_vector.size(); ++i)
		/*if(attributes_vector.size() == 1)
			attributes [fusion::at_c<0>(attributes_vector[i])] = string("1"); //making it by specializing constructor sucks (because it's the only line that differs
		else*/
			attributes [fusion::at_c<0>(attributes_vector[i])] = fusion::at_c<1>(attributes_vector[i]);
			
}

template<>
Node<wstring>::Node(HtmlStartTagAttr &tag){
	ptr = NodePtr(this);
	wstring tmp = fusion::at_c<0>(tag);	
	std::transform(tmp.begin(), tmp.end(), tmp.begin(), std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));
	element_name = tmp;
	vector<fusion::vector<wstring, wstring> > attributes_vector = fusion::at_c<1>(tag);
	for(int i=0; i<attributes_vector.size(); ++i){
		/*if(attributes_vector.size() == 1)
			attributes [fusion::at_c<0>(attributes_vector[i])] = wstring(L"1");
		else*/
			attributes [fusion::at_c<0>(attributes_vector[i])] = fusion::at_c<1>(attributes_vector[i]);
			wcout << "adding " << fusion::at_c<0>(attributes_vector[i]) << endl;
	}
}
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
bool Node<Str>::attr_exists(const Str &attribute) const{
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
	return (get_text().find(txt) != Str::npos);
}
//end of "has_" methods

template<typename Str>
void Node<Str>::search_by_tag_name(const Str &tag_name, vector<NodePtr> &v){
	search_by_tag_name_(tag_name, v, Flags::roots | Flags::descendants);
}

template<typename Str>
void Node<Str>::search_by_attribute(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v){
	search_by_attribute_(attribute_name, attribute_value, v, Flags::roots | Flags::descendants);
}

template<typename Str>
void Node<Str>::search_with_text(const Str &txt, vector<NodePtr> &v){
	search_with_text_(txt, v, Flags::roots | Flags::descendants);
}

//search_inside* variants

template<typename Str>
void Node<Str>::search_inside_by_tag_name(const Str &txt, vector<NodePtr> &v){
	search_by_tag_name_(txt, v, Flags::descendants);
}

template<typename Str>
void Node<Str>::search_inside_by_attribute(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v){
	search_by_attribute_(attribute_name, attribute_value, v, Flags::descendants);
}

template<typename Str>
void Node<Str>::search_inside_with_text(const Str &txt, vector<NodePtr> &v){
	search_with_text_(txt, v, Flags::descendants);
}


template<typename Str>
void Node<Str>::search_among_children_by_tag_name(const Str &tag_name, std::vector<NodePtr> &v){
	search_by_tag_name_(tag_name, v, Flags::children);
}

template<typename Str>
void Node<Str>::search_among_children_by_attribute(const Str &attribute_name, const Str &attribute_value, std::vector<NodePtr> &v){
	search_by_attribute_(attribute_name, attribute_value, v, Flags::children);
}

template<typename Str>
void Node<Str>::search_among_children_with_text(const Str &text, std::vector<NodePtr> &v){
	search_with_text_(text, v,  Flags::children);
}

//actual implementation of search_by_tag_name, search_by_attribute and search_with_text
//because search* and search_inside* variants differs only in the way they treat root element then each pair is implemented with one method:

template<typename Str>
void Node<Str>::search_by_tag_name_(const Str &tag_name, vector<NodePtr> &v, const int flags){
	//pre-order traversing
	if((flags & Flags::roots) && has_tag_name(tag_name))
		v.push_back(ptr);
	
	if(flags & Flags::descendants){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_by_tag_name_(tag_name, v, Flags::roots | Flags::descendants);
	}else if(flags & Flags::children){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_by_tag_name_(tag_name, v, Flags::roots);
	}
}

template<typename Str>
void Node<Str>::search_by_attribute_(const Str &attribute_name, const Str &attribute_value, vector<NodePtr> &v, const int flags){
	//pre-order traversing
	if((flags & Flags::roots) && has_attribute_value(attribute_name, attribute_value))
		v.push_back(ptr);
	
	if(flags & Flags::descendants){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_by_attribute_(attribute_name, attribute_value, v, Flags::roots | Flags::descendants);
	}else if(flags & Flags::children){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_by_attribute_(attribute_name, attribute_value, v, Flags::roots);
	}
}

template<typename Str>
void Node<Str>::search_with_text_(const Str &txt, vector<NodePtr> &v, const int flags){
	if((flags & Flags::roots) && has_text(txt))
		v.push_back(ptr);
	
	if(flags & Flags::descendants){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_with_text_(txt, v, Flags::roots | Flags::descendants);
	}else if(flags & Flags::children){
		for(int i = 0; i < children.size(); ++i)
			children[i]->search_with_text_(txt, v, Flags::roots);
	}
}

template<typename Str>
void Node<Str>::get_all(vector<NodePtr> & v, const int flags) const{
	if(flags & Flags::roots)
		v.push_back(ptr);
	
	if(flags & Flags::descendants){
		for(int i = 0; i< children.size(); ++i)
			children[i]->get_all(v, Flags::roots | Flags::descendants);
	}else if(flags & Flags::children){
		for(int i = 0; i < children.size(); ++i)
			children[i]->get_all(v, Flags::roots);
	}
}

template<typename Str>
void Node<Str>::get_ancestors(std::vector<NodePtr> & v, bool ignore_current_node) const{
	if(parent)
		parent->get_ancestors(v, false);
	
	if(!ignore_current_node)
		v.push_back(ptr);
}

template<typename Str>
bool Node<Str>::get_ancestors_inside(VectorNodePtr & descendants, VectorNodePtr & parents, VectorNodePtr & res, bool ignore_current_node){
	bool success = false;
	if(parent)
		success = parent->get_ancestors_inside(descendants, parents, res, false);
	
	if(!ignore_current_node){
		descendants.erase(remove(descendants.begin(), descendants.end(), ptr), descendants.end()); // to prevent from unneccessary checking
		if(success){
			res.push_back(ptr);
			return true;
		}else
			if(find(parents.begin(), parents.end(), ptr) != parents.end())
				return true;
	}
	return false;
}

template<typename Str>
bool Node<Str>::is_descendant_of(const NodePtr &potential_parent) const{
	if(!parent)
		return false;

	if(parent == potential_parent)
		return true;

	return parent->is_descendant_of(potential_parent);
}

//see: http://www.parashift.com/c++-faq-lite/templates.html#faq-35.15
template class Node<string>;
template class Node<wstring>;
