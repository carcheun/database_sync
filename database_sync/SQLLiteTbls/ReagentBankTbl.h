#ifndef ReagentBankTbl_h
#define ReagentBankTbl_h

#include "..\SQLLite\sqlite-tbl\SQLiteTbl.h"
#define REAGENT_BANK	(_T("R_Bank8.d"))

class CSQLreagentBankTbl : public SQLiteTbl
{
public:
	CSQLreagentBankTbl();

	virtual bool Open(const TCHAR * pFile);
	virtual const TCHAR * TableName();
	virtual int ItemCount();

	bool isExist(const TCHAR * pFile);

public:
	unsigned int	m_unSN;
	SQLiteString	m_szName;
	int				m_nVolCur;
	int				m_nVolOrg;

};


#endif