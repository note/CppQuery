#include <string>
#include <map>
#include <vector>

namespace CppQuery{
	class Node{
		typedef boost::shared_ptr<Node> NodePtr;

		std::string element_name;
		std::map<std::string, std::string> attributes;
		std::string text;
		std::vector<NodePtr> children; //boost::shared_ptr is safe because whole structure containing HTML will be tree - so there will be no cycles in it
	};
}

