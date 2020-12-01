/*
SQLiteTbl - Version 1.3

SQLite Table wrapper class.  See SQLiteTbl.h

Brad Kremer, Fluke Networks - December 2008

*/

//#include "stdafx.h"
#include "SQLiteTbl.h"

void SQLiteBlob8::Clean()
{
    if (m_pData) {
        delete [] m_pData;
        m_pData = NULL;
        m_size = 0;
    }
}

void SQLiteBlob8::Set(const char *pData, int size)
{
    Clean();

    if (pData && size > 0) {
        m_pData = new char[size];
        memcpy(m_pData, pData, size);
        m_size = size;
    }
}


void SQLiteBlob::Clean()
{
    if (m_pData) {
        delete [] m_pData;
        m_pData = NULL;
        m_size = 0;
    }
}

void SQLiteBlob::Set(const TCHAR *pData, int size)
{
    Clean();

    if (pData && size > 0) {
        m_pData = new TCHAR[size];
        memcpy(m_pData, pData, size*sizeof(TCHAR));
        m_size = size;
    }
}

TCHAR SQLiteString::operator [](int i)
{
    const TCHAR * pData = GetData();
    int size = GetSize();
    if (i >= 0 && i < size && pData) {
        return pData[i];
    }
    return '\0';
}

void SQLiteString::operator +=(const TCHAR * pStr)
{
    const TCHAR * pData = GetData();
    if (pData && pStr) {
        int len = GetSize() + (int)strlen(pStr);
        char* pBuf = new char[len+1];
        strcpy(pBuf, pData);
        strcat(pBuf, pStr);
        SetString(pBuf);
        delete [] pBuf;
    }
}

bool SQLiteString::operator ==(const TCHAR * pStr)
{
    const TCHAR * pData = GetData();
    if (pData && pStr) {
        return strcmp(pData, pStr) == 0;
    }
    return false;
}

bool SQLiteString::operator <(const TCHAR * pStr)
{
    const TCHAR * pData = GetData();
    if (pData && pStr) {
        return strcmp(pData, pStr) < 0;
    }
    return false;
}

bool SQLiteString::operator >(const TCHAR * pStr)
{
    const TCHAR * pData = GetData();
    if (pData && pStr) {
        return strcmp(pData, pStr) > 0;
    }
    return false;
}

void SQLiteString::Format(const TCHAR * fmt, ...)
{

    char* pBuf = new char[10000];
    va_list ap;
    
    va_start(ap, fmt);
    vsprintf(pBuf, fmt, ap);
    SetString(pBuf);
    va_end(ap);
    delete [] pBuf;
} 

void SQLiteString::SetString(const TCHAR * pData) 
{ 
    Clean(); 
    if (pData) {
        int l = (int)strlen(pData); 
        Set(pData, l+1); 
    } 
}

void SQLiteString::SetAt(int i, TCHAR c)
{
    TCHAR* pData = (TCHAR *)GetData();
    int len = GetSize();
    if (pData && len>0 && i < len) {
        pData[i] = c;
    }
}


SQLiteTbl::SQLiteTbl()
{
    m_pDb = NULL;
    m_pRowids = NULL;
    m_pItems = NULL;
    m_is_open = false;
    m_row_count = 0;
    m_row_pos = -1;
    m_col_count = 0;
    m_col_loaded = false;
    m_rowid_edit = 0;
    m_add = false;
}

SQLiteTbl::~SQLiteTbl()
{    
    if (m_pDb)
        Close();

    if (m_pItems) {
        delete [] m_pItems;
        m_pItems = NULL;
    }

    if (m_pRowids) {
        delete [] m_pRowids;
        m_pRowids = NULL;
    }
}

#define ROWID_GROW 5000

void SQLiteTbl::AddRowid(int id)
{
    if (m_pRowids) {
        if (m_row_count+1 >= m_rowid_size) {
            int new_size = m_rowid_size + ROWID_GROW;
            int* pNew = new int[new_size];
            if (pNew) {
                memset(pNew, 0, sizeof(pNew));
                memcpy(pNew, m_pRowids, sizeof(m_pRowids));
                delete [] m_pRowids;
                m_pRowids = pNew;
                m_rowid_size = new_size;
            }
        }        
    }
    else {
        m_rowid_size= m_row_count + ROWID_GROW;
        m_pRowids = new int[m_rowid_size];
    }

    m_pRowids[m_row_count++] = id;
}

void SQLiteTbl::SetItem(int item, const TCHAR * pName, MYDB_TYPE type, void* pData)
{
    int size = ItemCount(); 

    if (!m_pItems && size) {        
        m_pItems = new MYDB_ITEM[size];
    }

    if (m_pItems && item < size) {
        if (strlen(pName) < 100) {
            strcpy(m_pItems[item].name, pName);
            m_pItems[item].type = type;
            m_pItems[item].pData = pData;
            m_pItems[item].item_col = -1;
        }
    }
}



bool SQLiteTbl::Open(const TCHAR * pFile)
{
    if (!m_is_open && !m_pDb) {

		int i;
        i = sqlite3_open_v2(pFile, &m_pDb, SQLITE_OPEN_READWRITE, NULL);
        if (i == SQLITE_OK && m_pDb) {
            m_is_open = true;
            m_path = pFile;

            //first set the pragma to auto_vacuum
#ifdef MYSQLITE_AUTOVAC_ON
            ExeSQL(_T("PRAGMA auto_vacuum=1"));
#endif
            return true;
        }
    }

    return false;
}

bool SQLiteTbl::Create(const TCHAR * pFile)
{
    if (!m_is_open && !m_pDb) {
#if defined(_UNICODE) || defined(UNICODE)
        int i = sqlite3_open16(pFile, &m_pDb);
#else
        int i = sqlite3_open_v2(pFile, &m_pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
#endif
        if (i == SQLITE_OK && m_pDb) {
            m_is_open = true;
            m_path = pFile;

            //first set the pragma to auto_vacuum
#ifdef MYSQLITE_AUTOVAC_ON
            ExeSQL(_T("PRAGMA auto_vacuum=1"));
#endif
            if (InsertTable())
                return Query();
        }
    }

    return false;
}

bool SQLiteTbl::Close()
{
    if (m_is_open || m_pDb) {
        sqlite3_close(m_pDb);
        m_pDb = NULL;
        m_is_open = false;
        m_col_loaded = false;
        m_path = _T("");
    }

    return true;
}


bool SQLiteTbl::Compact()
{
    if (m_is_open && m_pDb && m_rowid_edit == 0 && !m_add) {
        ExeSQL(_T("VACUUM"));
    }

    return false;
}


bool SQLiteTbl::InsertTable()
{
    int item_count = ItemCount();

    if (m_is_open && m_pDb && m_pItems && item_count) {
        SQLiteString sql;
        sql.Format(_T("CREATE TABLE [%s] ("), TableName());

        for (int x=0; x < item_count; x++) {
            char sep = (x) ? ',' : ' ';
            SQLiteString ss;
            switch (m_pItems[x].type) {
            case MYDB_TYPE_TEXT:
                ss.Format(_T("%c[%s] TEXT"), sep, m_pItems[x].name);
                break;
            case MYDB_TYPE_INT:
                ss.Format(_T("%c[%s] INT"), sep, m_pItems[x].name);
                break;
            case MYDB_TYPE_DOUBLE:
                ss.Format(_T("%c[%s] DOUBLE"), sep, m_pItems[x].name);
                break;
            case MYDB_TYPE_BLOB:
                ss.Format(_T("%c[%s] LongBinary"), sep, m_pItems[x].name);
                break;
            case MYDB_TYPE_NONE:
	            break;
            }
            sql += ss;
        }
        sql += _T(")");

        ExeSQL(sql);

        return true;
    }

    return false;
}

bool SQLiteTbl::ExeSQL(const TCHAR * pSQL)
{
    if (m_is_open && m_pDb) {
        sqlite3_stmt* pStmt;

        const char *pTail;
        int i = sqlite3_prepare_v2(m_pDb, pSQL, -1, &pStmt, &pTail);
        if (i == SQLITE_OK) {
            i = sqlite3_step(pStmt);            
        }
        sqlite3_finalize(pStmt);
        
        return i == SQLITE_ROW || i == SQLITE_DONE;
    }
    return false;
}


bool SQLiteTbl::LoadColumnNumbers()
{
    int item_count = ItemCount();

    if (m_is_open && m_pDb && m_pItems && item_count) {
        sqlite3_stmt* pStmt;
        SQLiteString sql = GetSQL();

        const char *pTail;
        int i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
        if (i == SQLITE_CORRUPT) {
            return false;
        }

        if (i == SQLITE_OK) {
            m_col_count = sqlite3_column_count(pStmt); 
            
            for (int x=0; x < m_col_count; x++) {

                SQLiteString name = sqlite3_column_name(pStmt, x);
                for (int c=0; c < item_count; c++) {

                    if (name == m_pItems[c].name) {
                        m_pItems[c].item_col = x;
                        break;
                    }
                }
            }

            for (int c=0; c < item_count; c++) {
                if (m_pItems[c].item_col == -1) {
                    SQLiteString sql_add, sql_set;
                    
                    switch (m_pItems[c].type) {
                    case MYDB_TYPE_TEXT:
                        sql_add.Format(_T("ALTER TABLE [%s] ADD COLUMN [%s] TEXT"), TableName(), m_pItems[c].name);
                        sql_set.Format(_T("UPDATE %s SET [%s] = \"\""), TableName(), m_pItems[c].name);
                        break;
                    case MYDB_TYPE_INT:
                        sql_add.Format(_T("ALTER TABLE [%s] ADD COLUMN [%s] Integer"), TableName(), m_pItems[c].name);
                        sql_set.Format(_T("UPDATE %s SET [%s] = 0"), TableName(), m_pItems[c].name);
                        break;
                    case MYDB_TYPE_DOUBLE:
                        sql_add.Format(_T("ALTER TABLE [%s] ADD COLUMN [%s] Double"), TableName(), m_pItems[c].name);
                        sql_set.Format(_T("UPDATE %s SET [%s] = 0.0"), TableName(), m_pItems[c].name);
                        break;
                    case MYDB_TYPE_BLOB:
                        sql_add.Format(_T("ALTER TABLE [%s] ADD COLUMN [%s] LongBinary"), TableName(), m_pItems[c].name);
                        break;
		            case MYDB_TYPE_NONE: //do nothing
                        break;
                    }
                    if (!sql_add.IsEmpty()) {
                        ExeSQL(sql_add);
                    }
                    if (!sql_set.IsEmpty()) {
                        ExeSQL(sql_set);
                    }
                }
            }
            sqlite3_finalize(pStmt);

            m_col_loaded = true;
            return true;
        }
    }

    return false;
}

const TCHAR * SQLiteTbl::GetSQL(bool rowid_only, bool use_filter, bool use_sort)
{
    static SQLiteString sql;
    
    if (rowid_only)
        sql.Format(_T("SELECT ROWID FROM [%s]"), TableName());
    else
        sql.Format(_T("SELECT * FROM [%s]"), TableName());

    if (use_filter && !m_filter.IsEmpty()) {
        sql += _T(" WHERE ");
        sql += m_filter; 
    }

    if (use_sort && !m_sort.IsEmpty()) {
        sql += _T(" ORDER BY ");
        sql += m_sort; 
    }

    return sql;
}


bool SQLiteTbl::Query()
{
    SQLiteString sql;
    sqlite3_stmt* pStmt;

    if (m_is_open && m_pDb) {
        m_query_count = 0;

        if (!m_col_loaded) {
            if (!LoadColumnNumbers())
                return false;
        }

        m_row_count = ReadCount();
        m_row_pos = 1;

        if (m_pRowids) {
            delete [] m_pRowids;
            m_pRowids = NULL;
        }
        m_rowid_size = m_row_count + ROWID_GROW;
        m_pRowids = new int[m_rowid_size];
        memset(m_pRowids, 0, sizeof(m_pRowids));        

        if (m_row_count > 0) {
            sql = GetSQL(true);

	        const char *pTail;
            int i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
            if (i == SQLITE_OK) {
                i = sqlite3_step(pStmt);
                for (int r=0; r < m_row_count && i != SQLITE_DONE; r++) {
                    m_pRowids[r] = sqlite3_column_int(pStmt, 0);
                    m_query_count++;
                    i = sqlite3_step(pStmt);                      
                }
            }
            sqlite3_finalize(pStmt);

            Move(1);
        }
        return true;
    }

    return false;

}

int SQLiteTbl::ReadCount()
{
    int count = 0;
    SQLiteString sql;
    if (m_is_open || m_pDb) {
        sqlite3_stmt* pStmt;
        sql.Format(_T("SELECT COUNT (*) FROM [%s]"), TableName());
        if (!m_filter.IsEmpty()) {
            sql += _T(" WHERE ");
            sql += m_filter; 
        }

        const char *pTail;
        int i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
        if (i == SQLITE_OK) {
            i = sqlite3_step(pStmt); 
            if (i == SQLITE_ROW) {
                SQLiteString ss = (const char*)sqlite3_column_text(pStmt, 0);
                count = atoi(ss);
            }
        }
        sqlite3_finalize(pStmt);
    }
    return count;
}

void SQLiteTbl::InitItems()
{
    int item_count = ItemCount();
    SQLiteString* pStr;
    int* pInt;
    double* pDbl;
    SQLiteBlob8* pBlob;

    if (m_is_open && m_pDb && m_pItems) {
        for (int c =0; c < item_count; c++) {
            switch (m_pItems[c].type) {
            case MYDB_TYPE_TEXT:
                pStr = (SQLiteString*)m_pItems[c].pData;
                *pStr = _T("");
                break;
            case MYDB_TYPE_INT:
                pInt = (int*)m_pItems[c].pData;
                *pInt = 0;
                break;
            case MYDB_TYPE_DOUBLE:
                pDbl = (double*)m_pItems[c].pData;
                *pDbl = 0.0;
                break;
            case MYDB_TYPE_BLOB:
                pBlob = (SQLiteBlob8*)m_pItems[c].pData;
                pBlob->Clean();
                break;
            case MYDB_TYPE_NONE: //do nothing
                break;
            }
        }
    }
}

void SQLiteTbl::LoadItems(sqlite3_stmt* pStmt)
{
    int item_count = ItemCount();
    const TCHAR* pVal;
    SQLiteBlob8* pBlob;
    int size;

    if (m_is_open && m_pDb && m_pItems) {
        InitItems();

        for (int i = 0; i < item_count; i++) {
            if (m_pItems[i].pData) {
                switch (m_pItems[i].type) {
                case MYDB_TYPE_TEXT:
                    pVal = (const char*)sqlite3_column_text(pStmt, m_pItems[i].item_col);
                    *((SQLiteString*)m_pItems[i].pData) = (pVal) ? pVal : _T("");
                    break;
                case MYDB_TYPE_INT:
                    *((int*)m_pItems[i].pData) = sqlite3_column_int(pStmt, m_pItems[i].item_col); 
                    break;
                case MYDB_TYPE_DOUBLE:
                    *((double*)m_pItems[i].pData) = sqlite3_column_double(pStmt, m_pItems[i].item_col); 
                    break;
                case MYDB_TYPE_BLOB:
					const char *pcBinVal;
                    pBlob = (SQLiteBlob8*)m_pItems[i].pData;
                    pcBinVal = (const char*)sqlite3_column_blob(pStmt, m_pItems[i].item_col); 
                    size = sqlite3_column_bytes(pStmt, m_pItems[i].item_col);
                    if (pBlob && pcBinVal && size > 0)
                        pBlob->Set(pcBinVal, size);
                    break;
                case MYDB_TYPE_NONE:
                    break;
                }
            }
        }

        m_rowid_edit = 0;
        m_add = false;
    }
}


bool SQLiteTbl::Delete()
{
    if (m_is_open && m_pDb && m_rowid_edit == 0 && !m_add && m_row_pos <= m_row_count) {
        SQLiteString sql;
        int cur_rowid = m_pRowids[m_row_pos-1];
        sql.Format(_T("DELETE FROM [%s] WHERE ROWID = %d"), TableName(), cur_rowid);
        return ExeSQL(sql);
    }

    return false;
}


bool SQLiteTbl::Edit()
{
    if (m_is_open && m_pDb && m_pRowids && m_rowid_edit == 0 && m_row_count > 0 && !m_add) {
        m_rowid_edit = m_pRowids[m_row_pos-1]; 
        return true;
    }

    return false;
}

bool SQLiteTbl::AddNew()
{
    if (m_is_open && m_pDb && m_pItems && m_rowid_edit == 0 && !m_add) {
        //we don't really do anything for AddNew.  We just clear all the values in the table
        //Then when we call Update, I insert a new row in, and then set all the values
        InitItems();

        m_add = true;
        return true;
    }

    return false;
}


bool SQLiteTbl::Update()
{
    sqlite3_stmt* pStmt;
    SQLiteString ss, sql;
    SQLiteString* pStr;
    double* pDbl;
    int* pInt;
    SQLiteBlob8* pBlob;
    int size;

    int item_count = ItemCount();

    if (item_count < m_col_count && m_add) { //we have to do it the slow way
        //if the item count is less than the number of actual colums, we have to insert
        //a row with values first, then bind the blob data to it
        return SlowAdd();
    }

    //else they match, and we can use data binding for everything
    if (m_is_open && m_pDb && m_pItems && (m_rowid_edit > 0 || m_add)) {
        //if we are adding this record, go insert it first
        if (m_add) {
             sql.Format(_T("INSERT INTO [%s] VALUES("), TableName());
            //add all the binding identifiers
            for (int c = 0; c < item_count; c++) {
                sql += (c) ? _T(",?") : _T("?");
            }
            sql += _T(") ");
        }
        else {
            sql.Format(_T("UPDATE [%s] SET "), TableName()); 
            for (int c = 0; c < item_count; c++) {
                ss.Format(_T("%c[%s] = ?"), (c) ? ',' : ' ', m_pItems[c].name);
                sql += ss;
            }
            ss.Format(_T(" WHERE ROWID = %d"), m_rowid_edit);
            sql += ss;
        }

       const char *pTail;
       int i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
        if (i == SQLITE_OK) {  
            int bind_idx = 1;
            for (int c =0; c < item_count; c++) {
                switch (m_pItems[c].type) {
                case MYDB_TYPE_BLOB:
					const char *pcBinData;
                    pBlob = (SQLiteBlob8*)m_pItems[c].pData;
                    pcBinData = pBlob->GetData();
                    size = pBlob->GetSize();
                    sqlite3_bind_blob(pStmt, bind_idx++, pcBinData, size, SQLITE_STATIC);   
                    break;
                case MYDB_TYPE_TEXT:                        
                    pStr = (SQLiteString*)m_pItems[c].pData;
					size = pStr->GetLength();
                    sqlite3_bind_text(pStmt, bind_idx++, *pStr, size, SQLITE_STATIC);                            
                    break;
                case MYDB_TYPE_DOUBLE:                        
                    pDbl = (double*)m_pItems[c].pData;
                    sqlite3_bind_double(pStmt, bind_idx++, *pDbl);                            
                    break;
                case MYDB_TYPE_INT:                        
                    pInt = (int*)m_pItems[c].pData;
                    sqlite3_bind_int(pStmt, bind_idx++, *pInt);                            
                    break;
	            case MYDB_TYPE_NONE: //do nothing
                    break;
                }
            }

            sqlite3_step(pStmt);
            sqlite3_reset(pStmt);
            sqlite3_finalize(pStmt);
        }

        if (m_add) {
            //now go get the rowid of the newly inserted item
            m_rowid_edit = (int)sqlite3_last_insert_rowid(m_pDb);

            //add it to the rowid list
            AddRowid(m_rowid_edit); //this will increment the row count also
            m_row_pos = m_row_count; //the row position is pointed at the last added result
        }

        m_rowid_edit = 0; //we are done
        m_add = false;
        return true;
    }

    return false;
}

//this is the slower way to add.  It is required when the item list and the column count don't match
bool SQLiteTbl::SlowAdd()
{
    sqlite3_stmt* pStmt;
    SQLiteString ss, sql;
    SQLiteString* pStr;
    double* pDbl;
    int* pInt;
    SQLiteBlob8* pBlob;
    int size;

    int item_count = ItemCount();

    if (item_count < m_col_count && m_add) {
        sql.Format(_T("INSERT INTO [%s] ("), TableName());
        int add = 0;
        int c;
        for (c =0; c < item_count; c++) {
            switch (m_pItems[c].type) {
            case MYDB_TYPE_TEXT:
                pStr = (SQLiteString*)m_pItems[c].pData;
                ss.Format(_T("%c[%s]"), (add++) ? ',' : ' ', m_pItems[c].name);
                sql += ss; 
                break;
            case MYDB_TYPE_INT: 
                ss.Format(_T("%c[%s]"), (add++) ? ',' : ' ', m_pItems[c].name);
                sql += ss; 
                break;
	        case MYDB_TYPE_DOUBLE: //do nothing
	        case MYDB_TYPE_BLOB: //do nothing
	        case MYDB_TYPE_NONE: //do nothing
                break;
            }
        }
        sql += _T(") VALUES (");

        //now the values
        add = 0;
        for (c =0; c < item_count; c++) {
            switch (m_pItems[c].type) {
            case MYDB_TYPE_TEXT:
                pStr = (SQLiteString*)m_pItems[c].pData;
                ss.Format(_T("%c\"%s\""), (add++) ? ',' : ' ', *pStr);
                sql += ss;
                break;
            case MYDB_TYPE_INT:
                pInt = (int*)m_pItems[c].pData;
                ss.Format(_T("%c\"%d\""), (add++) ? ',' : ' ', *pInt);
                sql += ss;
                break;
	        case MYDB_TYPE_DOUBLE: //do nothing
	        case MYDB_TYPE_BLOB: //do nothing
	        case MYDB_TYPE_NONE: //do nothing
                break;
            }
        }
        sql += _T(")");

        const char *pTail;
        int i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
        if (i == SQLITE_OK) {
            sqlite3_step(pStmt);
        }
        sqlite3_finalize(pStmt);

        //now go get the rowid of the newly inserted item
        m_rowid_edit = (int)sqlite3_last_insert_rowid(m_pDb);

        //now bind the blob && double data
        for (c =0; c < item_count; c++) {
            switch (m_pItems[c].type) {
            case MYDB_TYPE_BLOB:
				const char * pcBinData;
                pBlob = (SQLiteBlob8*)m_pItems[c].pData;
                pcBinData = pBlob->GetData();
                size = pBlob->GetSize();
                if (size && pcBinData) {
                    sql.Format(_T("UPDATE [%s] SET [%s] = ? WHERE ROWID = %d"), TableName(), 
                            m_pItems[c].name, m_rowid_edit);
                    i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
                    if (i == SQLITE_OK) {
                        i = sqlite3_bind_blob(pStmt, 1, pcBinData, size, SQLITE_STATIC);
                        sqlite3_step(pStmt);
                    }
                    sqlite3_finalize(pStmt);
                }
                break;
            case MYDB_TYPE_DOUBLE:
                pDbl = (double*)m_pItems[c].pData;
                sql.Format(_T("UPDATE [%s] SET [%s] = ? WHERE ROWID = %d"), TableName(), 
                        m_pItems[c].name, m_rowid_edit);
                i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
                if (i == SQLITE_OK) {
                    i = sqlite3_bind_double(pStmt, 1, *pDbl);
                    sqlite3_step(pStmt);
                }
                sqlite3_finalize(pStmt);
                break;
            case MYDB_TYPE_TEXT:  //do nothing
            case MYDB_TYPE_INT:  //do nothing
            case MYDB_TYPE_NONE:  //do nothing
                break;
            }
        }

        //add it to the rowid list
        AddRowid(m_rowid_edit); //this will increment the row count also
        m_row_pos = m_row_count; //the row position is pointed at the last added result

        m_rowid_edit = 0; //we are done
        m_add = false;
        return true;
    }

    return false;
}


//SetOneItem will quickly set the value of one item in the table.  This is faster than
//using Edit/Update.  I created this to set the Deleted flag when deleting a group of records
bool SQLiteTbl::SetOneItem(const TCHAR * pItem, const TCHAR * pValue, int _rowid)
{
    if (m_is_open && m_pDb && m_pRowids && m_rowid_edit == 0 && m_row_count > 0 && !m_add) {
        sqlite3_stmt* pStmt;
        int rowid = (_rowid >= 0) ? _rowid : m_pRowids[m_row_pos-1];
        SQLiteString sql;
        sql.Format(_T("UPDATE [%s] SET [%s] = ? WHERE ROWID = %d"), TableName(), pItem, rowid);
        const char *pTail;
        int size = (int)strlen(pValue);
        int i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
        if (i == SQLITE_OK) {
            sqlite3_bind_text(pStmt, 1, pValue, size, SQLITE_STATIC); 
            sqlite3_step(pStmt);
        }
        sqlite3_finalize(pStmt);
          
        return true;
    }

    return false;
}

bool SQLiteTbl::IsBOF() 
{ 
    return !m_row_count || m_row_pos <= 1; 
} 


bool SQLiteTbl::IsEOF() 
{ 
    return !m_row_count || m_row_pos > m_row_count; 
}


bool SQLiteTbl::Move(int idx)
{
    if (m_is_open && m_pDb && m_pRowids && m_rowid_edit == 0 && !m_add) {  
        int index = (idx < 1) ? 1 : idx;
        if (idx <= 0) return false;

        if (m_row_count == 0)
            return true;  //nothing to do if it's empty, or if we are already at this record

        if (index <= m_row_count) {
            SQLiteString sql = GetSQL(false, false, false); //use the rowid only when moving around

            SQLiteString ss;
            int cur_rowid = m_pRowids[index-1];

            ss.Format(_T(" WHERE ROWID = %d"), cur_rowid);
            sql += ss;

            sqlite3_stmt* pStmt;
            const char *pTail;
            int i = sqlite3_prepare_v2(m_pDb, sql, -1, &pStmt, &pTail);
            if (i == SQLITE_OK) {
                sqlite3_step(pStmt);
            }                    

            LoadItems(pStmt);
            sqlite3_finalize(pStmt);
        }

        m_row_pos = index;

        return true;
    }

    return false;
}

bool SQLiteTbl::MoveFirst()
{
    if (m_is_open && m_pDb) {
        Move(1);
        return true;
    }

    return false;
}

bool SQLiteTbl::MoveLast()
{
    if (m_is_open && m_pDb) {
        Move(m_row_count);

        return true;
    }

    return false;
}

bool SQLiteTbl::MoveNext()
{
    if (m_is_open && m_pDb && m_row_pos <= m_row_count) {
        Move(++m_row_pos);

        return true;
    }

    return false;
}

bool SQLiteTbl::MovePrev()
{
    if (m_is_open && m_pDb && m_row_pos > 1) {
        Move(--m_row_pos);

        return true;
    }

    return false;
}

