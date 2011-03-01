#include "Node.h"

using namespace CppQuery;
using namespace std;

Node::Node(HtmlStartTagAttr &tag){
	element_name = fusion::at_c<0>(tag);
	vector<fusion::vector<string, string> > attributes_vector = fusion::at_c<1>(tag);
	for(int i=0; i<attributes_vector.size(); ++i)
	    attributes [fusion::at_c<0>(attributes_vector[i])] = fusion::at_c<1>(attributes_vector[i]);
}
