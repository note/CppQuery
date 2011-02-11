#include <gtest/gtest.h>
#include "CppQuery.h"
#include <sstream>
#include <fstream>

using namespace std;

const string realFile = "pyquery.html";
const string simpleFile = "simple.html";

class SimpleCase : public ::testing::Test{
	public:
	SimpleCase(){
		ifstream s(simpleFile.c_str());
		stringstream ss;
		ss << s.rdbuf();
		html = ss.str();
		cq(html);
	}

	protected:
	string html;
	CppQuery cq;
};

class RealCase : public ::testing::Test{
	public:
	RealCase(){
		ifstream s(realFile.c_str());
		stringstream ss;
		ss << s.rdbuf();
		html = ss.str();
	}
	
	protected:
	string html;
	CppQuery cq;
};

TEST_F(SimpleCase, Selectors){
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
