// vim: set fileencoding=utf-8 :
// vim: set encoding=utf-8 :
#include <gtest/gtest.h>
#include "CppQuery.h"
#include <sstream>
#include <fstream>

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
		cq(html);
	}

	protected:
	string html;
	CppQuery cq;
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
}

TEST_F(SimpleCase, Selectors){
	//should return 5 because one paragraph is inside comment
	EXPECT_EQ(5, cq("p").size());
	EXPECT_EQ(5, cq("P").size());

	//checks if deals with: <p class="some">Next <b>paragraph</b></p>
	EXPECT_STREQ("Next paragraph", cq("p")[1].text());
	
	//:contains
	EXPECT_STREQ("Lorem ipsum dolor ...", cq(":contains('dolor')").text().c_str());
	EXPECT_EQ(2, cq(":contains('\"androids')").size());
	EXPECT_EQ(1, cq("p:contains('\"androids')").size());
	EXPECT_STREQ("Do \"androids dream ...", cq("p:contains('\"androids'").text().c_str());

	//:has()
	EXPECT_STREQ("second", cq("div:has(h1)")["id"].c_str());

	// ">" selector
	EXPECT_EQ(2, cq("body > div").size());
	EXPECT_EQ(3, cq("body >").size());
}

TEST_F(SimpleCase, Problematic){
	EXPECT_STREQ("Lorem ipsum dolor ...Second paragraph ...", cq("#first p").text().c_str()); //check how it deals when text() is called on object containing a few nodes (should concatenate)
	
	EXPECT_STREQ("zażółć", cq("#find-me").text().c_str()); //checks if deals with: <h3   id="find-me"  >zażółć/h3   >
	EXPECT_STREQ("class-name", cq("#attr")["class"].c_str()); //checks if deals with: <h3 id="attr" class='class-name'>Another</h3>
	
	//check if deals with: <input type="checkbox" id="chbox" name="team" value="team" checked>Spurs
	//it's important that it returns empty string ("") to distinguish between situation when "checked" is not on attributes list - then it returns NULL
	EXPECT_STREQ("", cq("#chbox")["checked"].c_str());

	//checks if deals with: <h3 id='no-quote'  class=no-quote>Just another</h3>
	EXPECT_STREQ("no_quote", cq("#no-quote")["class"].c_str());

	//if cq("selector")==1 then: cq("selector")[0] returns cq("selector"):
	EXPECT_STREQ("special", cq("#first")["class"].c_str());
	EXPECT_STREQ("special", cq("#first")[0]["class"].c_str());

	//copy constructor
	cq = cq("#first");
	EXPECT_EQ(2, cq("p").size());
	CppQuery cq2(cq);
	EXPECT_EQ(2, cq2("p").size());

	CppQuery cq3;
	//operator = ()
	cq3 = cq2;
	EXPECT_EQ(2, cq3("p").size());

}

TEST_F(RealCase, Selectors){
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
	EXPECT_EQ(NULL, &tmp);
	EXPECT_EQ(0, cq(".nonexisting").size());
	EXPECT_EQ(0, cq(".nonexisting")("nonexisting2").size());
	tmp =  cq(".nonexisting").text();
	EXPECT_EQ(NULL, &tmp);	
}

TEST_F(OpenTagsCase, OpenTags){
	//checks if deals with: <img id="logo" src="some.gif" />
	EXPECT_STREQ("", cq("#logo").text().c_str());
	EXPECT_STREQ("some.gif", cq("#logo")["src"].c_str());
	
	//checks if deals with: <p>some paragraph
	EXPECT_STREQ("some paragraph", cq("p")[0].text().c_str());

	//checks if deals with: <li>First
	EXPECT_STREQ("first", cq("li")[0].text().c_str());
}
