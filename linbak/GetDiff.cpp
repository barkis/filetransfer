//GetDiff.cpp
#include "sync.h"	//includes string
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

vector<string> Sync::GetDiff()	{

	vector<string> vRetVal;
	vector<string> vIntersection;

	map<string,string> mapWinFileDate, mapLinFileDate;
	back_insert_iterator<map<string,string>>mapInsIter(mapWinFileDate);
	back_insert_iterator<vector<string>> outIter(vRetVal);
	back_insert_iterator<vector<string>> outIntersectIter(vIntersection);

	//add filename-date pairs to maps
	for(auto iter = m_vWinFiles.begin(),iter2 = m_vWinDates.begin();iter != m_vWinFiles.end();++iter,++iter2)	{
		mapWinFileDate.insert(make_pair(*iter,*iter2));
	}
	for(auto iter = m_vLinFiles.begin(),iter2 = m_vLinDates.begin();iter != m_vLinFiles.end();++iter,++iter2)	{
		mapLinFileDate.insert(make_pair(*iter,*iter2));
	}

	//sort vectors to allow use of set algorithms
	sort(m_vWinFiles.begin(),m_vWinFiles.end());
	sort(m_vLinFiles.begin(),m_vLinFiles.end());

	//Get list of files on client but not server to vRetVal
	set_difference(m_vWinFiles.begin(),m_vWinFiles.end(),m_vLinFiles.begin(),m_vLinFiles.end(),outIter);
	if(vRetVal.size())	{
		Log("Files not on server: ");
		for(std::string s: vRetVal)	{
			Log(s);
		}
		Log("");
	}

	//Get list of files on both to vIntersection
	set_intersection(m_vWinFiles.begin(),m_vWinFiles.end(),m_vLinFiles.begin(),m_vLinFiles.end(),outIntersectIter);

	int iRetValSize = vRetVal.size();
	//Append files on both where date on Linux is earlier to vRetVal
	// Note copy happens if pred returns false
	auto pred = [&mapWinFileDate,&mapLinFileDate](string sFileName)->bool {return mapWinFileDate[sFileName] <= mapLinFileDate[sFileName];};
	auto newEnd = remove_copy_if(vIntersection.begin(),vIntersection.end(),outIter,pred);
	if(vRetVal.size() - iRetValSize)	{
		Log("Out of date files");
		for(int i=iRetValSize;i<vRetVal.size();++i)	{
			Log(vRetVal[i]);
		}
		Log("");
	}
	return vRetVal;
}

