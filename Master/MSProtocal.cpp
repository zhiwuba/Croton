#include "MSProtocal.h"
#include "Porting/CrotonPorting.h"


MSProtocal::MSProtocal()
{
	pkg_type_=INVALID_PKG;
	qp_type_=INVALID_QP;
}


MSProtocal::~MSProtocal()
{
	
}

int  MSProtocal::BuildPackage(std::string& package)
{
	Json::Value pack;
	pack["PKG"]=pkg_type_;
	pack["QP"]=qp_type_;
	pack["BODY"]=body_;

	Json::FastWriter  writer;
	package=writer.write(pack);
	return 0;
}

int  MSProtocal::PrasePackage(std::string package )
{
	int nret=0;
	Json::Reader  reader;
	Json::Value    pack;
	if ( reader.parse(package, pack))
	{
		pkg_type_=PKG_TYPE(pack["PKG"].asInt());
		qp_type_=QP_TYPE(pack["QP"].asInt());
		body_=pack["BODY"];
	}
	else
	{
		LLOG(L_ERROR, "PrasePackage Error.");
		nret=-1;
	}
	return nret;
}

