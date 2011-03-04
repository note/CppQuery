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

	template<typename Str>
	class Node{
		typedef boost::shared_ptr<Node> NodePtr;
		typedef fusion::vector<Str, std::vector<fusion::vector<Str, Str> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		
		public:
		Node(){}
		    
		Node(HtmlStartTagAttr &tag);
		
		NodePtr get_shared_ptr(){
			return ptr;
		}

		void addChild(NodePtr child){
			children.push_back(child);
		}

		void appendText(const Str &txt){
			text += txt;
		}

		Str get_text(){
			return text;
		}

		Str get_attribute(const Str &attribute);
		bool attr_exists(const Str &attribute);
		
		void select_by_tag_name(const Str &tag_name, std::vector<NodePtr> &v);
		void select_by_attribute(const Str &attribute_name, const Str &attribute_value, std::vector<NodePtr> &v);

		private:
		NodePtr ptr;
		Str element_name;
		std::map<Str, Str> attributes;
		Str text;
		std::vector<NodePtr> children; //boost::shared_ptr is safe because whole structure containing HTML will be a tree - so there will be no cycles in it
	};
}

#endif //CPP_QUERY_NODE_H

