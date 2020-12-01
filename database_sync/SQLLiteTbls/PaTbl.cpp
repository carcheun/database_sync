//#include "stdafx.h"
#include "PaTbl.h"


CPaTbl::CPaTbl()
{
    SetItem(0, _T("FullName"),          MYDB_TYPE_TEXT, &m_szFull);
    SetItem(1, _T("Alias"),             MYDB_TYPE_TEXT, &m_szAlias);
    SetItem(2, _T("Source"),            MYDB_TYPE_TEXT, &m_szSource);
    SetItem(3, _T("Catalog"),           MYDB_TYPE_TEXT, &m_szCat);
    SetItem(4, _T("Volume"),            MYDB_TYPE_INT,  &m_nVol);
    SetItem(5, _T("Incub"),             MYDB_TYPE_INT,  &m_nIncub);
    SetItem(6, _T("AR"),				MYDB_TYPE_TEXT, &m_szAR);
    SetItem(7, _T("Description"),       MYDB_TYPE_TEXT, &m_szDesp);
    SetItem(8, _T("Factory"),			MYDB_TYPE_INT, &m_nFact);
    SetItem(9, _T("Date"),              MYDB_TYPE_INT,  &m_nDate);
    SetItem(10, _T("Time"),             MYDB_TYPE_INT,  &m_nTime);
 }

const TCHAR* CPaTbl::TableName() { return _T("PA"); }
int CPaTbl::ItemCount() { return 11; }


bool CPaTbl::Open(const TCHAR* pFile)
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



