#ifndef  __CROTON_MASTER_MSPROTOCAL_H__
#define  __CROTON_MASTER_MSPROTOCAL_H__
#include "json/json.h"
#include "Common.h"


class MSProtocal
{
public:
	MSProtocal();
	~MSProtocal();
	int  BuildPackage(std::string& package);
	int  PrasePackage(std::string package );
	inline PKG_TYPE  GetPkgType(){ return pkg_type_; };
	inline void  SetPkgType(PKG_TYPE  type){ pkg_type_=type; };
	inline QP_TYPE  GetQpType(){ return qp_type_; };
	inline void  SetQpType(QP_TYPE  type){ qp_type_=type; };
	//����������Ϣ ��Ϊbody
	inline Json::Value  GetBody(){ return body_; }; 
	inline void   SetBody(Json::Value& body){body_=body; };
private:
	PKG_TYPE pkg_type_;
	QP_TYPE  qp_type_;
	Json::Value  body_;
};

#endif
