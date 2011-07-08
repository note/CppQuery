#ifndef CPP_QUERY_NODE_H
#define CPP_QUERY_NODE_H

#include <string>
#include <map>
#include <vector>
#include <locale>

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
		typedef std::vector<NodePtr> VectorNodePtr;
		typedef fusion::vector<Str, std::vector<fusion::vector<Str, Str> > > HtmlStartTagAttr; //each start tag consists of tag name and some number of pairs <attribute-name, attribute-value>
		
		public:
		class Flags{
			public:
			static int descendants, children, roots;
		};
		Node(){}
		    
		explicit Node(HtmlStartTagAttr &tag);
		
		//this constructor is useful when creating unit tests, but it's not used by any CppQuery class
		explicit Node(const Str &tag_name, std::vector<Str> attr_names, std::vector<Str> attr_values);
		
		NodePtr get_shared_ptr(){
			return ptr;
		}
		   
		//methods for constructing
		void add_child(NodePtr child){
			children.push_back(child);
			child->add_parent(get_shared_ptr());
		}

		void add_parent(NodePtr parent_node){
			parent = parent_node;
		}

		void append_text(const Str &txt){
			texts.push_back(txt);
		}
		//end of methods for constructing
		
		//getters
		Str get_text(){
			Str res;
			for(int i=0; i<children.size(); ++i)
				res += children[i]->get_text();
			return res;
		}
		Str get_attribute(const Str &attribute);
		Str get_tag_name(){
			return element_name;
		}
		//end of getters
		
		bool is_descendant_of(const NodePtr &potential_parent) const;
		bool attr_exists(const Str &attribute) const;
		void get_all(VectorNodePtr & v, const int flags) const;
		void get_ancestors(VectorNodePtr & v, bool ignore_current_node) const;
		bool get_ancestors_inside(VectorNodePtr & descendants, VectorNodePtr & parents, VectorNodePtr & res, bool ignore_current_node = true);
		
		//select methods:
		void search_by_tag_name(const Str &tag_name, VectorNodePtr &v);
		void search_by_attribute(const Str &attribute_name, const Str &attribute_value, VectorNodePtr &v);
		void search_with_text(const Str &txt, VectorNodePtr &v);
		
		//search_inside* variants
		void search_inside_by_tag_name(const Str &tag_name, VectorNodePtr &v);
		void search_inside_by_attribute(const Str &attribute_name, const Str &attribute_value, VectorNodePtr &v);
		void search_inside_with_text(const Str &text, VectorNodePtr &v);
		
		bool has_tag_name(const Str &tag_name);
		bool has_attribute_value(const Str &attribute_name, const Str &attribute_value);
		bool has_text(const Str &text);
		
		void search_among_children_by_tag_name(const Str &tag_name, VectorNodePtr &v);
		void search_among_children_by_attribute(const Str &attribute_name, const Str &attribute_value, VectorNodePtr &v);
		void search_among_children_with_text(const Str &text, VectorNodePtr &v);
		//end of select methods
		
		private:
		NodePtr ptr; // Node holds shared_ptr to itself
		Str element_name;
		std::map<Str, Str> attributes;
		
		// it holds text parts.
		// For: <p>first part <b>something</b> second part</p>
		// texts[0] = "first part ", texts[1] = " second part"
		std::vector<Str> texts;
		
		NodePtr parent;
		VectorNodePtr children; //boost::shared_ptr is safe because whole structure containing HTML will be a tree - so there will be no cycles in it

		//if deep is set to false then nodes that are at lower level than children are ignored.
		//So eg. ignore_root=true and deep=false performs checks only for children
		void search_by_tag_name_(const Str &tag_name, VectorNodePtr &v, const int flags);
		void search_by_attribute_(const Str &attribute_name, const Str &attribute_value, VectorNodePtr &v, const int flags);
		void search_with_text_(const Str &txt, VectorNodePtr &v, const int flags);
		
	};
	template <typename Str> int Node<Str>::Flags::descendants = 1;
	template <typename Str> int Node<Str>::Flags::children = 2;
	template <typename Str> int Node<Str>::Flags::roots = 4;
}

#endif //CPP_QUERY_NODE_H

