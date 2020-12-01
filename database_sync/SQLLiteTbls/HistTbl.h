/*  HistTbl.h
*/

#ifndef HistTbl_h
#define HistTbl_h

#include "..\SQLLite\sqlite-tbl\SQLiteTbl.h"
#define T_HIST	(_T("Hist8.d"))

class CHistTbl : public SQLiteTbl
{
public:
	CHistTbl();

	virtual bool Open(const TCHAR * pFile);
	virtual const TCHAR * TableName();
	virtual int ItemCount();

	int			m_nDate;
	int			m_nSequence;
	int			m_nTime;
	SQLiteString m_szLog;
};

////////////////////////////////////////////////
////////////////////////////////////////////////

class CHist_Slide {
  public:
	CHist_Slide();
  public:
	BOOLEAN		m_bOccupied;
	CString		m_ID;
	CString		m_strPatient;
	CString		m_strCase;
	CString		m_strBlock;
	CString		m_strPA;
	CString		m_strCat;
	CString		m_strLoc;
	CString		m_strVol;
	CString		m_strIncub;
	CString		m_strAR;
	CString		m_strDate;
};

class CHist_Module {
  public:
	CHist_Module();
  public:
	BOOLEAN		m_bHasSlide;
	BOOLEAN		m_bARbath;
};


class CHist_Reagent {
  public:
	CHist_Reagent();
  public:
	BOOLEAN		m_bOccupied;
	CString		m_ID;

	CString		m_strReagName;
	CString		m_strSN;
	CString		m_strLot;
	CString		m_strVolumeBefore;
	CString		m_strExpDate;
	CString		m_strVolumeAfter;
	CString		m_strCat;
	CString		m_strType;
	CString		m_strSize;


};

class CHist_Run {
  public:
	CHist_Run();
	BOOLEAN mfSave(LPCTSTR ptszFile);
	BOOLEAN mfRestore(LPCTSTR ptszFile);

  public:
	CHist_Slide		m_Slide[12*6];
	CHist_Module	m_Module[12];
	CHist_Reagent	m_Reagent[80];

	CString	m_csProtocol;
	int		m_nARtemp;
	int		m_nARincubMin;

};

#endif