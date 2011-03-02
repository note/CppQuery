#include "Node.h"

using namespace CppQuery;
using namespace std;

Node::Node(HtmlStartTagAttr &tag){
	ptr = NodePtr(this);
	element_name = fusion::at_c<0>(tag);
	vector<fusion::vector<string, string> > attributes_vector = fusion::at_c<1>(tag);
	for(int i=0; i<attributes_vector.size(); ++i)
	    attributes [fusion::at_c<0>(attributes_vector[i])] = fusion::at_c<1>(attributes_vector[i]);
}


string Node::get_attribute(const string &attribute){
	map<string, string>::iterator it = attributes.find(attribute);
	if(it!=attributes.end())
		return (*it).second;
	return string();
}

bool Node::attr_exists(const string &attribute){
	return attributes.find(attribute) != attributes.end();
}

void Node::select_by_tag_name(const string &tag_name, vector<NodePtr> &v){
	//pre-order traversing
	if(element_name == tag_name)
		v.push_back(ptr);
			
	for(int i = 0; i < children.size(); i++)
		children[i]->select_by_tag_name(tag_name, v);
}

void Node::select_by_attribute(const string &attribute_name, const string &attribute_value, vector<NodePtr> &v){
	//pre-order traversing
	map<string, string>::iterator attr_it = attributes.find(attribute_name);
	if(attr_it!=attributes.end())
		if((*attr_it).second == attribute_value)
			v.push_back(ptr);

	for(int i = 0; i < children.size(); i++)
		select_by_attribute(attribute_name, attribute_value, v);
}
