#ifndef CPP_QUERY_NODE_H
#define CPP_QUERY_NODE_H

#include <string>
#include <map>
#include <vector>

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/fusion/iterator.hpp>
#include <boost/fusion/algorithm.hpp>

#include <boost/shared_ptr.hpp>

#include "using.h"

namespace CppQuery{
	class Node{
		typedef boost::shared_ptr<Node> NodePtr;
		typedef fusion::vector<std::string, std::vector<fusion::vector<std::string, std::string> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		
		public:
		Node(){}
		    
		Node(HtmlStartTagAttr &tag);
		
		void addChild(NodePtr child){
		    children.push_back(child);
		}
		
		std::vector<NodePtr> select_by_tag_name(const std::string &tag_name);
		std::vector<NodePtr> select_by_attribute(const std::string &attribute_name, const std::string &attribute_value);

		private:
		std::string element_name;
		std::map<std::string, std::string> attributes;
		std::string text;
		std::vector<NodePtr> children; //boost::shared_ptr is safe because whole structure containing HTML will be a tree - so there will be no cycles in it
	};
}

#endif //CPP_QUERY_NODE_H

