#include <gtest/gtest.h>
#include <json/json.h>

TEST(json_testcase,   writejson )
{
	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;
	Json::StyledWriter writer;

	item["cpp"]="jsoncpp";
	item["java"]="jsoninjava";
	item["php"]="support";
	arrayObj.append(item);

	root["name"]="json";
	root["array"]=arrayObj;

	std::cout<<writer.write(root)<<std::endl;
}

TEST(json_testcase, readjson)
{
	std::string strValue="{\"name\":\"json\",\"array\":[	{\"cpp\":\"jsoncpp\"},{\"java\":\"jsoninjava\"},{\"php\":\"support\"}	]}";
	Json::Reader reader;
	Json::Value   value;
	if ( reader.parse(strValue,value) )
	{
		std::cout<<value["name"]<<std::endl;
		Json::Value arrayObj=value["array"];
		for ( unsigned int i=0; i<arrayObj.size(); i++ )
		{
			if ( arrayObj[i].isMember("cpp") )
			{
				std::cout<<arrayObj[i]["cpp"].asString()<<std::endl;
			}
		}
	}
}

TEST(json_testcase, WriteTwice)
{
	Json::Value root_1;
	root_1["1"]="number1";
	root_1["2"]="number2";
	Json::Value root_2;
	root_2["3"]="number3";
	root_2["4"]="number4";

	Json::Value root_3;
	root_3.append(1234);
	root_3.append(567.8899);

	Json::Value root_4;
	root_4.append("CCCC");
	root_4.append("DDDD");


	std::string str_1=root_3.toStyledString();
	std::string str_2=root_4.toStyledString();

	Json::Reader reader;
	Json::Value   str_11, str_22;
	if ( reader.parse(str_1 , str_11) &&reader.parse(str_2, str_22 ) )
	{
		root_1["3"]=str_11;
		root_1["4"]=str_22;
		std::cout<<root_1.toStyledString()<<std::endl;
	}
	else
	{
		std::cout<<"Error"<<std::endl;
	}
	Json::Value array1=root_1["3"];
	Json::Value::iterator iter=array1.begin();
	for (  ; iter!=array1.end() ; iter++)
	{
		std::cout<<*iter<<std::endl;
	}

	//std::cout<<array1[0][]<<std::endl;	
}

TEST(json_testcase, xml)
{
	

}

