#include <gtest/gtest.h>
#include "../CppQuery.h"
#include "../src/Node.h"
#include <sstream>
#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;
using namespace CppQuery;

typedef boost::shared_ptr<Node<string> > NodePtr;

vector<string> make_v(const string &str){
	vector<string> v;
	v.push_back(str);
	return v;
}

class SelectorsTest : public ::testing::Test{
	public:
	SelectorsTest(){
		typedef Node<string> NodeS;
		vector<string> empty;
		NodeS *html = new NodeS("html", empty, empty), *d1=new NodeS("div", make_v("id"), make_v("header")), *d2=new NodeS("div", make_v("id"), make_v("main"));
 		html->add_child(d1->get_shared_ptr());
 		html->add_child(d2->get_shared_ptr());

		NodeS *d3 = new NodeS("div", make_v("id"), make_v("inner-div"));
		d2->add_child(d3->get_shared_ptr());

		vector<string> attr_names, attr_val;
		attr_names.push_back("id");
		attr_names.push_back("class");
		attr_val.push_back("p1");
		attr_val.push_back("special");
		NodeS *p1 = new NodeS("p", attr_names, attr_val);
		p1->append_text("czerwony");
		NodeS *a1 = new NodeS("a", empty, empty), *a2 = new NodeS("a", make_v("href"), make_v("www"));
		a1->append_text("zielony");
		a2->append_text("niebieski");
		
		p1->add_child(a1->get_shared_ptr());
		p1->add_child(a2->get_shared_ptr());
		d2->add_child(p1->get_shared_ptr());
	 
		set_root(html->get_shared_ptr());
	}
	void set_root(NodePtr r){
	    root = r;
	}
	protected:
	NodePtr root;
};

TEST_F(SelectorsTest, select_by_tag_name){
  	
  	std::vector<NodePtr> res;
 	root->select_by_tag_name("html", res);
	EXPECT_EQ(1, res.size());
 	res.clear();
	
	root->select_by_tag_name("html435", res);
	EXPECT_EQ(0, res.size());
	res.clear();
	
	root->select_by_tag_name("div", res);
	EXPECT_EQ(3, res.size());
	res.clear();

}

TEST_F(SelectorsTest, select_by_attribute){
	vector<NodePtr> res;
	root->select_by_attribute("id", "p1", res);
	EXPECT_EQ(1, res.size());
	if(res.size()>0)
		EXPECT_STREQ("czerwony", res[0]->get_text().c_str());
	res.clear();

	root->select_by_attribute("href", "www2", res);
	EXPECT_EQ(0, res.size());
	res.clear();

	root->select_by_attribute("href", "www", res);
	EXPECT_EQ(1, res.size());
	if(res.size()>0)
		EXPECT_STREQ("a", res[0]->get_tag_name().c_str());
	res.clear();
}

TEST_F(SelectorsTest, select_with_text){
	vector<NodePtr> res;
	root->select_with_text("czerwony", res);
	EXPECT_EQ(1, res.size());
	EXPECT_STREQ("p1", res[0]->get_attribute("id").c_str());
	res.clear();
}

TEST_F(SelectorsTest, search_inside_by_tag_name){

 	vector<NodePtr> res, tmp;
 	root->search_inside_by_tag_name("html", res);
 	EXPECT_EQ(0, res.size());
 	res.clear();

	root->search_inside_by_attribute("id", "main", tmp);
	tmp[0]->search_inside_by_tag_name("div", res);
	EXPECT_EQ(1, res.size());
	EXPECT_STREQ("inner-div", res[0]->get_attribute("id").c_str());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
