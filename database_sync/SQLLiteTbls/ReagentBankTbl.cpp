//#include "stdafx.h"
#include "ReagentBankTbl.h"

const TCHAR* CSQLreagentBankTbl::TableName() { return _T("RBank"); }
int CSQLreagentBankTbl::ItemCount() { return 4; }


bool CSQLreagentBankTbl::Open(const TCHAR* pFile)
{
	if (SQLiteTbl::Open(pFile)) {
		if (Query())
			return true;
	}
	Close();
	return false;
}

bool CSQLreagentBankTbl::isExist(const TCHAR * pFile)
{
	bool bRet = false;
	if (SQLiteTbl::Open(pFile)) {
		if (Query())
			bRet = true;
	}
	Close();
	return bRet;
}

CSQLreagentBankTbl::CSQLreagentBankTbl()
{
	SetItem(0, _T("SN"), MYDB_TYPE_INT, &m_unSN);
	SetItem(1, _T("Name"), MYDB_TYPE_TEXT, &m_szName);
	SetItem(2, _T("VolCur"), MYDB_TYPE_INT, &m_nVolCur);
	SetItem(3, _T("VolOrg"), MYDB_TYPE_INT, &m_nVolOrg);
}

