//#include "stdafx.h"
#include "ReagTbl.h"

CReagTbl::CReagTbl()
{
    SetItem(0, _T("ReagName"),			MYDB_TYPE_TEXT,	&m_szReagName);
    SetItem(1, _T("Catalog"),			MYDB_TYPE_TEXT,	&m_szCatalog);
    SetItem(2, _T("Type"),				MYDB_TYPE_TEXT,	&m_szType);
    SetItem(3, _T("Size"),				MYDB_TYPE_TEXT,	&m_szBottleSize);
    SetItem(4, _T("SN"),				MYDB_TYPE_TEXT, &m_szSN);
    SetItem(5, _T("Log"),				MYDB_TYPE_TEXT, &m_szLog);
    SetItem(6, _T("Vol"),				MYDB_TYPE_INT, &m_nVol);
    SetItem(7, _T("MfgDate"),			MYDB_TYPE_INT, &m_nMfgDate);
    SetItem(8, _T("ExpDate"),			MYDB_TYPE_INT, &m_nExpDate);
    SetItem(9, _T("Factory"),			MYDB_TYPE_INT, &m_nFact);
    SetItem(10, _T("VolCur"),			MYDB_TYPE_INT, &m_nVolCur);
    SetItem(11, _T("Sequence"),			MYDB_TYPE_INT, &m_nSequence);
    SetItem(12, _T("EditDate"),			MYDB_TYPE_INT, &m_nEditDate);
    SetItem(13, _T("Reserved"),			MYDB_TYPE_TEXT, &m_szReserved);
	SetItem(14, _T("UpdateDate"), MYDB_TYPE_INT, &m_nUpdateDate);
	SetItem(15, _T("UpdateTime"), MYDB_TYPE_INT, &m_nUpdateTime);
}

const TCHAR* CReagTbl::TableName() { return _T("REAGENT"); }
int CReagTbl::ItemCount() { return 16; }


bool CReagTbl::Open(const TCHAR* pFile)
{
	if(SQLiteTbl::IsOpen())
		return true;
    if (SQLiteTbl::Open(pFile)) {
        if (Query()) 
            return true;
    }
    Close();
    return false;
}


