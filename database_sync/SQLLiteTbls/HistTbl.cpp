/*  HistTbl.cpp : implementation file
*/

#include "stdafx.h"
#include "HistTbl.h"


CHistTbl::CHistTbl()
{
    SetItem(0, _T("Date"),				MYDB_TYPE_INT,  &m_nDate);
    SetItem(1, _T("Sequence"),			MYDB_TYPE_INT,  &m_nSequence);
    SetItem(2, _T("Time"),				MYDB_TYPE_INT,  &m_nTime);
    SetItem(3, _T("Log"),				MYDB_TYPE_TEXT, &m_szLog);
 }

const TCHAR* CHistTbl::TableName() { return _T("History"); }
int CHistTbl::ItemCount() { return 4; }


bool CHistTbl::Open(const TCHAR* pFile)
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



CHist_Slide::CHist_Slide()
{
	m_bOccupied=FALSE;
	m_ID.Empty();
}

CHist_Module::CHist_Module()
{
	m_bHasSlide=FALSE;
}

CHist_Reagent::CHist_Reagent()
{
	m_bOccupied=FALSE;
	m_ID.Empty();
}


CHist_Run::CHist_Run()
{
}

#include <fstream>
#define SZ_HIST_VER "01"
using namespace std;

void _tcharToAnsi(const TCHAR *ptszInput, char *pszOutput)
{
	strcpy(pszOutput, (LPCTSTR)ptszInput);
	if(strlen(pszOutput)==0)
		strcpy(pszOutput, "empty");
}

void _ansiToTchar(char *pszInput, TCHAR *ptszOutput)
{
	strcpy(ptszOutput, pszInput);
}

BOOLEAN CHist_Run::mfSave(LPCTSTR ptszFile)
{
	char szAnsi[1000];
	_tcharToAnsi((LPCTSTR)ptszFile, szAnsi);

	ofstream f;
	f.open(szAnsi);
	if(f.fail())
		return FALSE;

	f << SZ_HIST_VER << endl;

	_tcharToAnsi((LPCTSTR)m_csProtocol, szAnsi);
	f << szAnsi << endl;

	f << m_nARtemp << endl;
	f << m_nARincubMin << endl;

	int n;
	for(n=0; n<12*6; n++) {
		f << (n+1) << endl;
		f << m_Slide[n].m_bOccupied << endl;

		if(m_Slide[n].m_bOccupied) {
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_ID, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strPatient, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strCase, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strBlock, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strPA, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strCat, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strLoc, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strVol, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strIncub, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strAR, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Slide[n].m_strDate, szAnsi);
			f << szAnsi << endl;
		}
	}

	for(n=0; n<80; n++) {
		f << (n+1) << endl;
		f << m_Reagent[n].m_bOccupied << endl;
		if(m_Reagent[n].m_bOccupied) {
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_ID, szAnsi);
			f << szAnsi << endl;

			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strReagName, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strSN, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strLot, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strVolumeBefore, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strExpDate, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strVolumeAfter, szAnsi);
			f << szAnsi << endl;

			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strCat, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strType, szAnsi);
			f << szAnsi << endl;
			_tcharToAnsi((LPCTSTR)m_Reagent[n].m_strSize, szAnsi);
			f << szAnsi << endl;
		}
	}
	f.close();
	return TRUE;
}

BOOLEAN CHist_Run::mfRestore(LPCTSTR ptszFile)
{
	char szAnsi[1000];
	TCHAR tszData[1000];

	_tcharToAnsi((LPCTSTR)ptszFile, szAnsi);

	ifstream f;
	f.open(szAnsi);
	if(f.fail())
		return FALSE;

	f.getline(szAnsi, 99);
	if(strcmp(szAnsi, SZ_HIST_VER)) {
		f.close();
		return FALSE;
	}

	f.getline(szAnsi, 99);
	_ansiToTchar(szAnsi, tszData);
	m_csProtocol=tszData;

	f >> m_nARtemp;
	f >> m_nARincubMin;

	int n;
	for(n=0; n<12*6; n++) {
		int nSlideNo;
		f >> nSlideNo;

		f >> m_Slide[n].m_bOccupied;

		if(m_Slide[n].m_bOccupied) {
			f.getline(szAnsi, 99);
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_ID=tszData;

			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strPatient=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strCase=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strBlock=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strPA=tszData;

			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strCat=tszData;

			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strLoc=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strVol=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strIncub=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strAR=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Slide[n].m_strDate=tszData;
		}
	}

	for(n=0; n<80; n++) {
		TCHAR tszData[1000];

		int nReagNo;
		f >> nReagNo;

		f >> m_Reagent[n].m_bOccupied;

		if(m_Reagent[n].m_bOccupied) {
			f.getline(szAnsi, 99);
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_ID=tszData;

			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strReagName=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strSN=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strLot=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strVolumeBefore=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strExpDate=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strVolumeAfter=tszData;


			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strCat=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strType=tszData;
			f.getline(szAnsi, 99);
			_ansiToTchar(szAnsi, tszData);
			m_Reagent[n].m_strSize=tszData;
		}
	}

	f.close();
	return TRUE;
}

// end of file