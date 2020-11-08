#include "sync.h"

/* when reception of replacement file is finished close the stream and rename the temp file */
void Sync::EndFileUpdate()	{
	std::string sFullFileName(m_sPathRoot + m_sCurrentDirForUpdate + m_sCurrentFileForUpdate);
	m_TempFileStream.close();
	m_sCurrentFileForUpdate  = m_sCurrentDirForUpdate = "";
	m_RequestsOutstanding --;
	if(rename(m_szTempFileName,sFullFileName.c_str()))	{	//rename returns 0 for success
		int iErr = errno;
		std::string sMess("Failed to rename file ");
		sMess += m_szTempFileName;
		sMess += " to ";
		sMess += sFullFileName;
		sMess += " in EndFileUpdate, errno: ";
		sMess += std::to_string(iErr);
	//	throw std::runtime_error(sMess);
		Log(sMess);
	}
	
}
