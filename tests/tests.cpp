// vim: fileencoding=utf-8 :
#include <gtest/gtest.h>
#include "../CppQuery.h"
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

const string real_file = "pyquery.html";
const string simple_file = "simple.html";
const string open_tags_file = "open_tags.html";

class CppQueryTest : public ::testing::Test{
	public:
	CppQueryTest(const string &fileName){
		ifstream s(fileName.c_str());
		stringstream ss;
		ss << s.rdbuf();
		html = ss.str();
		cq.load(html);
	}

	protected:
	string html;
	CppQuery::Query<string> cq;
};

class SimpleCase : public CppQueryTest{
	public:
	SimpleCase() : CppQueryTest(simple_file) {}
};

class RealCase : public CppQueryTest{
	public:
	RealCase() : CppQueryTest(real_file){}
};

class OpenTagsCase : public CppQueryTest{
	public:
	OpenTagsCase() : CppQueryTest(open_tags_file){}
};

TEST_F(SimpleCase, Selectors){
// 	//should return 5 because one paragraph is inside comment
 	EXPECT_EQ(5, cq("p").size());
 	EXPECT_EQ(5, cq("P").size());
// 	
// 	//:contains
// 	EXPECT_STREQ("Lorem ipsum dolor ...", cq(":contains(dolor)").text().c_str());
// 	EXPECT_EQ(2, cq(":contains(\"androids)").size());
// 	EXPECT_EQ(1, cq("p:contains(\"androids)").size());
// 	EXPECT_STREQ("Do \"androids dream ...", cq("p:contains(\"androids").text().c_str());
// 
// 	//:has()
// 	EXPECT_STREQ("second", cq("div:has(h1)")["id"].c_str());
// 
// 	// descendant selector
// 	EXPECT_EQ(3, cq("#second p").size());
// 	
// 	/* there is html like that:
// 	<div id="second">
// 	<div id="third"></div></div>
// 
// 	cq("#second div")["id"] <-- This code should not return "second", should return "third"
// 	*/
// 	EXPECT_STREQ("third", cq("#second div")["id"].c_str());
// 	
// 	// ">" selector
// 	EXPECT_EQ(2, cq("body > div").size());
// 	EXPECT_EQ(3, cq("body >").size());
// 
// 	//the same problem as with descendant selector
// 	EXPECT_STREQ("third", cq("#second > div")["id"].c_str());
// 
// 	//attribute equals selector [name=value]
// 	EXPECT_STREQ("chbox", cq("[name=team]")["id"].c_str());
// 
// 	//:not() selector
// 	EXPECT_EQ(3, cq("p:not(.some)").size());
// 	EXPECT_STREQ("Lorem ipsum dolor ...", cq("p:not(.some)")[0].text().c_str());
// 
// 	//more complex examples:
// 	EXPECT_EQ(1, cq("div:has(p:has(b))").size());
// 	EXPECT_STREQ("first", cq("div:has(p:has(b))")["id"].c_str());
// 	EXPECT_EQ(0, cq("div:has(p:has(u))").size());
// 
// 
// 	//utf-8
// 	CppQuery::Query<std::wstring> cq(std::wstring(html));
// 	EXPECT_STREQ(L"zaÅ¼Ã³ÅÄ", cq(L"#find-me").text().c_str()); //checks if deals with: <h3   id="find-me"  >zaÅ¼Ã³ÅÄ/h3   >
// 	EXPECT_EQ(6, cq(L"#find-me").text().size());
// 	EXPECT_EQ(1, cq(L":contains(zaÅ¼Ã³ÅÄ").size());
// 	EXPECT_STREQ(L"find-me", cq(L":contains(zaÅ¼Ã³ÅÄ)")[L"id"].c_str());
// 
}

TEST_F(SimpleCase, Problematic){
	//checks if deals with: <p class="some">Second <b>paragraph</b> ...</p>
	EXPECT_STREQ("Second paragraph ...", cq("p")[1].text().c_str());
	
	/*EXPECT_STREQ("Lorem ipsum dolor ...Second paragraph ...", cq("#first p").text().c_str()); //check how it deals when text() is called on object containing a few nodes (should concatenate)
	
	EXPECT_STREQ("class-name", cq("#attr")["class"].c_str()); //checks if deals with: <h3 id="attr" class='class-name'>Another</h3>
	
	//check if deals with: <input type="checkbox" id="chbox" name="team" value="team" checked>Spurs
	//to distinguish between situations when "checked" is not on attributes list and is on it but empty - use attr_exists()
	EXPECT_TRUE(cq("#chbox")["checked"].empty());
	EXPECT_TRUE(cq("#chbox").attr_exists("checked"));

	//checks if deals with: <h3 id='no-quote'  class=no-quote>Just another</h3>
	EXPECT_STREQ("no_quote", cq("#no-quote")["class"].c_str());

	//if cq("selector")==1 then: cq("selector")[0] returns cq("selector"):
	EXPECT_STREQ("special", cq("#first")["class"].c_str());
	EXPECT_STREQ("special", cq("#first")[0]["class"].c_str());

	//copy constructor
	cq = cq("#first");
	EXPECT_EQ(2, cq("p").size());
	CppQuery::Query<std::string> cq2(cq);
	EXPECT_EQ(2, cq2("p").size());

	CppQuery::Query<std::string> cq3;
	//operator = ()
	cq3 = cq2;
	EXPECT_EQ(2, cq3("p").size());*/

}

/*TEST_F(RealCase, Selectors){
	//simple selector and ["attribute"] notation
	EXPECT_STREQ("homepage", cq("logolink")["alt"].c_str());

	//nested selector and text()
	EXPECT_STREQ("Quickstart", cq("#quickstart h1").text().c_str());

	//alternative version of nested selector
	EXPECT_STREQ("Quickstart", cq("quickstart")("h1").text().c_str());

	//get n-th element with [n] notation
	EXPECT_STREQ("quickstart", cq(".section")[2]["id"].c_str());

	//get number of elements with size()
	EXPECT_EQ(6, cq(".section").size());
}

TEST_F(RealCase, Problematic){
	//more problematic situations:
	//out of range, there is only 6 nodes with class "section"
	string tmp = cq(".section")[6].text();
	EXPECT_EQ(NULL, &tmp);
	
	tmp =  cq("#logolink")["nonexisting"];
	EXPECT_TRUE(tmp.empty());
	EXPECT_FALSE(cq("#logolink").attr_exists("nonexisting"));
	EXPECT_EQ(0, cq(".nonexisting").size());
	EXPECT_EQ(0, cq(".nonexisting")("nonexisting2").size());
	tmp =  cq(".nonexisting").text();
	EXPECT_TRUE(tmp.empty());
}

TEST_F(OpenTagsCase, OpenTags){
	//checks if deals with: <img id="logo" src="some.gif" />
	EXPECT_STREQ("", cq("#logo").text().c_str());
	EXPECT_STREQ("some.gif", cq("#logo")["src"].c_str());
	
	//checks if deals with: <p>some paragraph
	EXPECT_STREQ("some paragraph", cq("p")[0].text().c_str());

	//checks if deals with: <li>First
	EXPECT_STREQ("first", cq("li")[0].text().c_str());
}*/

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
