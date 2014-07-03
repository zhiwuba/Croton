#ifndef  __CROTON_SLAVE_SMPROTOCAL_H__
#define __CROTON_SLAVE_SMPROTOCAL_H__
#include "Common.h"
#include "json/json.h"

class SMProtocal
{
public:
	SMProtocal()
	{
		pkg_type_=INVALID_PKG;
		qp_type_=INVALID_QP;
	}
	~SMProtocal(){};
	int  BuildPackage(std::string& package);
	int  PrasePackage(std::string package );
	inline PKG_TYPE  GetPkgType(){ return pkg_type_; };
	inline void  SetPkgType(PKG_TYPE  type){ pkg_type_=type; };
	inline QP_TYPE  GetQpType(){ return qp_type_; };
	inline void  SetQpType(QP_TYPE  type){ qp_type_=type; };
	//包含任务信息 称为body
	inline Json::Value  GetBody(){ return body_; }; 
	inline void   SetBody(Json::Value& body){body_=body; };
private:
	PKG_TYPE pkg_type_;
	QP_TYPE  qp_type_;
	Json::Value  body_;
};

#endif
