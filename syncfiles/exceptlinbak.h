#include <exception>
#include <string>

struct ExceptLinbak: std::exception	{
	std::string m_sText;
	int m_iErrno;
	int m_iLineno;
	bool m_hasErrno;
public:
	ExceptLinbak(std::string text) throw() {m_sText = text;m_hasErrno = false;}
	ExceptLinbak(std::string text,int Errno,int Lineno = 0):m_sText(text),m_iErrno(Errno),m_iLineno(Lineno),m_hasErrno(true){}
	~ExceptLinbak() throw() {}
	const char* what() throw() {return m_sText.c_str();}
	const bool hasErrno(){return m_hasErrno;}
	int Errno(){return m_iErrno;}
};

void LinbakErrHand(ExceptLinbak *ex);	
