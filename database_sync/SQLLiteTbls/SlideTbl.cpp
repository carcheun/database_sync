//#include "stdafx.h"
#include "Slidetbl.h"


CSlideTbl::CSlideTbl()
{
    SetItem(0, _T("Sequence"),			MYDB_TYPE_INT,  &m_nSequence);
    SetItem(1, _T("Date"),				MYDB_TYPE_INT,  &m_nDate);
    SetItem(2, _T("Time"),				MYDB_TYPE_TEXT,  &m_szTime);
    SetItem(3, _T("PatientName"),		MYDB_TYPE_TEXT, &m_szPatient_name);
    SetItem(4, _T("PatientID"),			MYDB_TYPE_TEXT, &m_szPatient_ID);
    SetItem(5, _T("PatientCase"),		MYDB_TYPE_TEXT, &m_szPatient_case);
    SetItem(6, _T("PatientBlock"),		MYDB_TYPE_TEXT, &m_szPatient_block);
    SetItem(7, _T("PAname"),			MYDB_TYPE_TEXT, &m_szPA_name);
    SetItem(8, _T("Catalog"),			MYDB_TYPE_TEXT, &m_szCatalog);

    SetItem(9, _T("PAvol"),				MYDB_TYPE_INT,  &m_nPA_vol);
    SetItem(10, _T("PAincub"),			MYDB_TYPE_INT,  &m_nPA_incub);
    SetItem(11, _T("PAar"),				MYDB_TYPE_TEXT, &m_szPA_ar);

    SetItem(12, _T("Fact"),				MYDB_TYPE_INT,  &m_nFact);
    SetItem(13, _T("State"),			MYDB_TYPE_INT,	&m_nState);
    SetItem(14, _T("Reserved"),			MYDB_TYPE_TEXT, &m_szReserved);
}

const TCHAR* CSlideTbl::TableName() { return _T("SLIDE"); }
int CSlideTbl::ItemCount() { return 14; }


bool CSlideTbl::Open(const TCHAR* pFile)
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


