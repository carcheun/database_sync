/*  PaTbl.h
*/

#ifndef PaTbl_h
#define PaTbl_h

#include "..\SQLLite\sqlite-tbl\SQLiteTbl.h"
#define PA_FACT	(_T("PA_fact8.d"))
#define PA_USER	(_T("PA_user8.d"))

class CPaTbl : public SQLiteTbl
{
public:
    CPaTbl();

    virtual bool Open(const TCHAR * pFile);
    virtual const TCHAR * TableName();
    virtual int ItemCount();

    SQLiteString m_szFull;
    SQLiteString m_szAlias;
    SQLiteString m_szSource;
    SQLiteString m_szCat;
    int          m_nVol;
    int          m_nIncub;
    SQLiteString m_szAR;
    SQLiteString m_szDesp;
    int          m_nFact;
    int          m_nDate;
    int          m_nTime;
};

#endif