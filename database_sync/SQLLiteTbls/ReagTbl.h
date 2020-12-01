#ifndef ReagTbl_h
#define ReagTbl_h

#include "..\SQLLite\sqlite-tbl\SQLiteTbl.h"

#define REAG_FILE	(_T("Reag8.d")) 
#define PA_REAG_FILE (_T("C:ReagPA8.d")) 


class CReagTbl : public SQLiteTbl
{
public:
    CReagTbl();

    virtual bool Open(const TCHAR* pFile);
    virtual const TCHAR* TableName();
    virtual int ItemCount();

    SQLiteString	m_szReagName;
    SQLiteString	m_szCatalog;
    SQLiteString	m_szType;
	SQLiteString	m_szBottleSize;
	SQLiteString	m_szSN;
	SQLiteString	m_szLog;
	int				m_nVol;
	int				m_nMfgDate;
	int				m_nExpDate;
	int				m_nFact;
	int				m_nVolCur;
	int				m_nSequence;
	int				m_nEditDate;
	SQLiteString	m_szReserved;
	int				m_nUpdateDate;
	int				m_nUpdateTime;

};

#endif