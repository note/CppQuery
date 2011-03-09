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
		    
		explicit Node(HtmlStartTagAttr &tag);
		
		//this constructor is useful when crearing unit tests, but it's not used by any CppQuery class
		explicit Node(const Str &tag_name, std::vector<Str> attr_names, std::vector<Str> attr_values);
		
		NodePtr get_shared_ptr(){
			return ptr;
		}
		   
		//methods for constructing
		void add_child(NodePtr child){
			children.push_back(child);
		}

		void append_text(const Str &txt){
			text += txt;
		}
		//end of methods for constructing
		
		//getters
		Str get_text(){
			return text;
		}
		Str get_attribute(const Str &attribute);
		Str get_tag_name(){
			return element_name;
		}
		//end of getters
		
		bool attr_exists(const Str &attribute);
		
		//select methods:
		void select_by_tag_name(const Str &tag_name, std::vector<NodePtr> &v);
		void select_by_attribute(const Str &attribute_name, const Str &attribute_value, std::vector<NodePtr> &v);
		void select_with_text(const Str &txt, std::vector<NodePtr> &v);
		
		//search_inside* variants
		void search_inside_by_tag_name(const Str &tag_name, std::vector<NodePtr> &v);
		void search_inside_by_attribute(const Str &attribute_name, const Str &attribute_value, std::vector<NodePtr> &v);
		void search_inside_with_text(const Str &text, std::vector<NodePtr> &v);
		//end of select methods
		
		private:
		NodePtr ptr;
		Str element_name;
		std::map<Str, Str> attributes;
		Str text;
		std::vector<NodePtr> children; //boost::shared_ptr is safe because whole structure containing HTML will be a tree - so there will be no cycles in it

		
		void select_by_tag_name_(const Str &tag_name, std::vector<NodePtr> &v, bool root=false);
		void select_by_attribute_(const Str &attribute_name, const Str &attribute_value, std::vector<NodePtr> &v, bool root=false);
		void select_with_text_(const Str &txt, std::vector<NodePtr> &v, bool root=false);
	};
}

#endif //CPP_QUERY_NODE_H

