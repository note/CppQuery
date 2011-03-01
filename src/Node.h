#ifndef CPP_QUERY_NODE_H
#define CPP_QUERY_NODE_H

#include <string>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

namespace CppQuery{
	class Node{
		typedef boost::shared_ptr<Node> NodePtr;
		typedef fusion::vector<string, vector<fusion::vector<string, string> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		
		public:
		Node();

		private:
		std::string element_name;
		std::map<std::string, std::string> attributes;
		std::string text;
		std::vector<NodePtr> children; //boost::shared_ptr is safe because whole structure containing HTML will be a tree - so there will be no cycles in it
	};
}

#endif //CPP_QUERY_NODE_H

