#ifndef SlideTbl_h
#define SlideTbl_h

#include "..\SQLLite\sqlite-tbl\SQLiteTbl.h"

class CSlideTbl : public SQLiteTbl
{
public:
    CSlideTbl();

    virtual bool Open(const TCHAR* pFile);
    virtual const TCHAR* TableName();
    virtual int ItemCount();

    int          m_nSequence;
    int          m_nDate;
	SQLiteString m_szTime;
    SQLiteString m_szPatient_name;
    SQLiteString m_szPatient_ID;
    SQLiteString m_szPatient_case;
    SQLiteString m_szPatient_block;
    SQLiteString m_szPA_name;
    SQLiteString m_szCatalog;
    int          m_nPA_vol;
    int          m_nPA_incub;
    SQLiteString m_szPA_ar;
    int          m_nFact;
	int          m_nState;
    SQLiteString m_szReserved;
};

#endif