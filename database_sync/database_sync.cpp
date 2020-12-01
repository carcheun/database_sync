// database_sync.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>
#include <filesystem>

#include "ReagentDBClient.h"

#include "SQLLiteTbls/ReagTbl.h"
#include "SQLLiteTbls/PATbl.h"

std::string SERVER = "";
namespace fs = std::experimental::filesystem;

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage) {
	int val = (int)(percentage * 100);
	int lpad = (int)(percentage * PBWIDTH);
	int rpad = PBWIDTH - lpad;
	printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
	fflush(stdout);
}

int SendPaEntries(ReagentDBClient rdb, std::vector<std::string> list_of_pa) {
	CPaTbl pa_tbl;
	std::vector<std::string> paths;
	paths.push_back("reagents");
	paths.push_back("api");
	paths.push_back("pa");
	paths.push_back("initial_sync");

	for (auto file_name : list_of_pa) {
		// get the SN
		std::cout << "Uploading " << file_name << std::endl;
		pa_tbl.Open(file_name.c_str());
		pa_tbl.Query();
		pa_tbl.MoveFirst();
		int nCnt = pa_tbl.GetCount();
		int arrIdx = 0;

		json::value arr_pa = json::value::array();

		for (int i = 0; i < nCnt; i++) {
			json::value data;
			std::wstring ws(pa_tbl.m_szFull.GetSize(), L' '); // Overestimate number of code points.
			ws.resize(std::mbstowcs(&ws[0], pa_tbl.m_szFull, pa_tbl.m_szFull.GetSize())); // Shrink to fit.
			data[U("fullname")] = json::value::string(ws);

			ws = std::wstring(pa_tbl.m_szAlias.GetSize(), L' '); // Overestimate number of code points.
			ws.resize(std::mbstowcs(&ws[0], pa_tbl.m_szAlias, pa_tbl.m_szAlias.GetSize())); // Shrink to fit.
			data[U("alias")] = json::value::string(ws);

			data[U("source")] = json::value::string(utility::conversions::to_utf16string(pa_tbl.m_szSource.GetData()));
			data[U("catalog")] = json::value::string(utility::conversions::to_utf16string(pa_tbl.m_szCat.GetData()));
			data[U("volume")] = pa_tbl.m_nVol;
			data[U("incub")] = pa_tbl.m_nIncub;
			data[U("ar")] = json::value::string(utility::conversions::to_utf16string(pa_tbl.m_szAR.GetData()));
			data[U("description")] = json::value::string(utility::conversions::to_utf16string(pa_tbl.m_szDesp.GetData()));
			data[U("factory")] = pa_tbl.m_nFact;
			data[U("date")] = json::value::string(utility::conversions::to_utf16string(rdb.ConvertClientDateToServerDateField(pa_tbl.m_nDate)
				+ "T" + rdb.ConvertClientTimeToServerTimeField(pa_tbl.m_nTime)));

			arr_pa[arrIdx++] = data;
			pa_tbl.MoveNext();
		}

		pa_tbl.Close();
		json::value ret = rdb.JPostGeneric(paths, arr_pa);
		if (ret.has_field(U("_error"))) {
			return -1;
		}
	}
	return 0;
}

int SendReagentEntries(ReagentDBClient rdb, std::vector<std::string> list_of_reag) {
	CReagTbl pCreagTbl;
	std::vector<std::string> paths;
	paths.push_back("reagents");
	paths.push_back("api");
	paths.push_back("reagent");
	paths.push_back("sync_batch");

	for (auto file_name : list_of_reag) {
		int str_len = file_name.length();
		
		if (file_name.find(".dll") != std::string::npos) {
			continue;
		}

		std::cout << "\nUploading " << file_name << std::endl;

		std::string autostainer_sn = file_name.substr(6, str_len - 6 - 2);

		pCreagTbl.Open(file_name.c_str());
		pCreagTbl.Query();
		pCreagTbl.MoveFirst();
		int nCnt = pCreagTbl.GetCount();
		int send_cnt = 0;

		std::cout << nCnt << " entries\n";

		json::value arr_reagents = json::value::array();

		double progress = 0.0;
		printProgress(progress);

		for (int i = 0; i < nCnt; i++) {
			json::value data;
			std::wstring ws(pCreagTbl.m_szReagName.GetSize(), L' '); // Overestimate number of code points.
			ws.resize(std::mbstowcs(&ws[0], pCreagTbl.m_szReagName, pCreagTbl.m_szReagName.GetSize())); // Shrink to fit.

			data[U("autostainer_sn")] = json::value::string(utility::conversions::to_utf16string(autostainer_sn));
			data[U("reag_name")] = json::value::string(ws);
			data[U("reagent_sn")] = json::value::string(utility::conversions::to_utf16string(pCreagTbl.m_szSN.GetData()));
			data[U("catalog")] = json::value::string(utility::conversions::to_utf16string(pCreagTbl.m_szCatalog.GetData()));
			data[U("size")] = json::value::string(utility::conversions::to_utf16string(pCreagTbl.m_szBottleSize.GetData()));
			data[U("log")] = json::value::string(utility::conversions::to_utf16string(pCreagTbl.m_szLog.GetData()));
			data[U("vol_cur")] = pCreagTbl.m_nVolCur;
			data[U("vol")] = pCreagTbl.m_nVol;
			data[U("exp_date")] = json::value::string(utility::conversions::to_utf16string(rdb.ConvertClientDateToServerDateField(pCreagTbl.m_nExpDate)));
			data[U("mfg_date")] = json::value::string(utility::conversions::to_utf16string(rdb.ConvertClientDateToServerDateField(pCreagTbl.m_nMfgDate)));
			data[U("date")] = json::value::string(utility::conversions::to_utf16string(rdb.ConvertClientDateToServerDateField(pCreagTbl.m_nEditDate)
				+ "T" + rdb.ConvertClientTimeToServerTimeField(pCreagTbl.m_nUpdateTime)));
			data[U("r_type")] = json::value::string(utility::conversions::to_utf16string(pCreagTbl.m_szType.GetData()));
			data[U("factory")] = pCreagTbl.m_nFact;

			arr_reagents[send_cnt % 40] = data;
			send_cnt++;
			if (send_cnt % 40 == 0 || send_cnt == nCnt) {
				json::value ret = rdb.JPostGeneric(paths, arr_reagents);
				if (ret.has_field(U("_error"))) {
					return -1;
				}
				arr_reagents = json::value::array();

				progress = (double)(i+1) / (double)nCnt;
				printProgress(progress);
			}
			pCreagTbl.MoveNext();
		}

		pCreagTbl.Close();
	}

	return 0;
}

int CreatePA8D(ReagentDBClient rdb) {
	CPaTbl * pCPaTbl;
	CPaTbl PAFact;
	CPaTbl PAUser;
	std::vector<std::string> paths;
	paths.push_back("reagents");
	paths.push_back("api");
	paths.push_back("pa");

	json::value allPa = rdb.JGetGeneric(paths, std::map<std::string, std::string>());

	if (!allPa.is_array())
		return -1;

	//if (!PAFact.Open(PA_FACT))
	PAFact.Create(PA_FACT);
	//if (!PAUser.Open(PA_USER))
	PAUser.Create(PA_USER);

	for (auto data : allPa.as_array()) {
		if (data[U("is_factory")].as_bool())
			pCPaTbl = &PAFact;
		else
			pCPaTbl = &PAUser;

		pCPaTbl->Query();
		pCPaTbl->AddNew();

		std::wstring ws = data[U("fullname")].as_string();
		std::string ss(ws.size(), L' ');
		ws.resize(std::wcstombs(&ss[0], ws.c_str(), ws.size()));
		pCPaTbl->m_szFull = ss.c_str();

		ws = data[U("alias")].as_string();
		ss = std::string(ws.size(), L' ');
		ws.resize(std::wcstombs(&ss[0], ws.c_str(), ws.size()));
		pCPaTbl->m_szAlias = ss.c_str();

		pCPaTbl->m_szSource = utility::conversions::to_utf8string(
			data[U("source")].as_string()).c_str();
		pCPaTbl->m_szCat = utility::conversions::to_utf8string(
			data[U("catalog")].as_string()).c_str();
		pCPaTbl->m_nVol = data[U("volume")].as_integer();
		pCPaTbl->m_nIncub = data[U("incub")].as_integer();
		pCPaTbl->m_szAR = utility::conversions::to_utf8string(
			data[U("ar")].as_string()).c_str();
		pCPaTbl->m_szDesp = utility::conversions::to_utf8string(
			data[U("description")].as_string()).c_str();

		std::string s = utility::conversions::to_utf8string(data[U("date")].as_string());
		// slice string format to fit our format
		// "2020-09-03T20:25:14.724488Z",
		std::string date = s.substr(2, 2) + s.substr(5, 2) + s.substr(8, 2);
		std::string time = s.substr(11, 2) + s.substr(14, 2) + s.substr(17, 2);

		pCPaTbl->m_nDate = stoi(date);
		pCPaTbl->m_nTime = stoi(time);

		if (data[U("is_factory")].as_bool()) {
			pCPaTbl->m_nFact = 1;
		}
		pCPaTbl->Update();
	}

	PAFact.Close();
	PAUser.Close();

	return 0;
}

int CreateReagD(ReagentDBClient rdb) {
	CReagTbl pCreagTbl;
	std::vector<std::string> paths;
	paths.push_back("reagents");
	paths.push_back("api");
	paths.push_back("reagent");

	json::value allReag = rdb.JGetGeneric(paths, std::map<std::string, std::string>());

	if (!allReag.is_array())
		return -1;

	// create reag8.d from all the returned information
	//if (!pCreagTbl.Open(REAG_FILE))
	pCreagTbl.Create(REAG_FILE);

	double progress = 1.0;
	double total = allReag.size();

	for (auto data : allReag.as_array()) {
		if (data[U("catalog")].is_null()) {
			data[U("catalog")] = json::value::string(U(""));
		}
		pCreagTbl.AddNew();

		int serverDate;
		int serverTime;
		rdb.ConvertServerDateTimeFieldToClientDateTime(serverDate, serverTime,
			utility::conversions::to_utf8string(data[U("date")].as_string()));

		pCreagTbl.m_szSN = utility::conversions::to_utf8string(data[U("reagent_sn")].as_string()).c_str();

		std::wstring ws = data[U("reag_name")].as_string();
		std::string ss(ws.size(), L' ');
		ws.resize(std::wcstombs(&ss[0], ws.c_str(), ws.size()));
		pCreagTbl.m_szReagName = ss.c_str();

		pCreagTbl.m_szCatalog = utility::conversions::to_utf8string(data[U("catalog")].as_string()).c_str();
		pCreagTbl.m_szLog = utility::conversions::to_utf8string(data[U("log")].as_string()).c_str();

		pCreagTbl.m_nVol = data[U("vol")].as_integer();
		pCreagTbl.m_nVolCur = data[U("vol_cur")].as_integer();
		pCreagTbl.m_nSequence = data[U("sequence")].as_integer();
		pCreagTbl.m_nMfgDate = rdb.ConvertServerDateFieldToClientDate(
			utility::conversions::to_utf8string(data[U("mfg_date")].as_string()));
		pCreagTbl.m_nExpDate = rdb.ConvertServerDateFieldToClientDate(
			utility::conversions::to_utf8string(data[U("exp_date")].as_string()));
		pCreagTbl.m_nEditDate = serverDate;
		pCreagTbl.m_nUpdateDate = serverDate;
		pCreagTbl.m_nUpdateTime = serverTime;
		pCreagTbl.m_nFact = data[U("factory")].as_bool();
		pCreagTbl.m_szType = utility::conversions::to_utf8string(data[U("r_type")].as_string()).c_str();
		pCreagTbl.m_szBottleSize = utility::conversions::to_utf8string(data[U("size")].as_string()).c_str();

		pCreagTbl.Update();
		pCreagTbl.SetFilter("");
		pCreagTbl.Query();

		printProgress(progress / total);
		progress++;
	}

	pCreagTbl.Close();
	return 0;
}

int main()
{
	std::vector<std::string> list_of_pa;
	std::vector<std::string> list_of_reag;

	// get list of stuff from current directroy
	for (const auto & entry : fs::directory_iterator(fs::current_path())) {
		// if the path contains REAGENT_SN_bla
		// PA_SN
		std::string str_filename = entry.path().filename().generic_string();
		
		if (str_filename.find("PA") != std::string::npos) {
			//list_of_pa.push_back(entry.path().generic_string());
			list_of_pa.push_back(str_filename);
		}
		else if (str_filename.find("Reag") != std::string::npos) {
			//list_of_reag.push_back(entry.path().generic_string());
			list_of_reag.push_back(str_filename);
		}
		else if (str_filename.find("settings.txt") != std::string::npos) {
			std::ifstream setting_file;
			setting_file.open(entry.path().generic_string());
			if (setting_file.is_open()) {
				getline(setting_file, SERVER);
			}
			setting_file.close();
			std::cout << "Setting server as: " << SERVER << std::endl;
		}
	}

	ReagentDBClient rdb = ReagentDBClient(SERVER);

	std::cout << "Starting upload sequence...\n";

	if (SendPaEntries(rdb, list_of_pa) != 0) {
		std::cout << "\nError with server or data! Aborting PA Sync!";
		exit(-1);
	}
	
	if (SendReagentEntries(rdb, list_of_reag) != 0) {
		std::cout << "\nError with server or data! Aborting Reagent Sync!";
		exit(-1);
	}

	std::cout << "\nStarting download sequence...\n";

	if (CreatePA8D(rdb) != 0) {
		std::cout << "\nError with server or data! Aborting Create PA8.d!";
		exit(-1);
	}

	if (CreateReagD(rdb) != 0) {
		std::cout << "\nError with server or data! Aborting Create Reag8.d!";
		exit(-1);
	}


	return 0;
}
