#include "sync.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


void Sync::ReadConfig(std::string sFileName) {
	using boost::property_tree::ptree;

	ptree theTree;
	m_sConfigPath = "";
	m_ConfigPort = 23001;
	m_CountWidth = 4;

	try	{
		read_xml(sFileName.c_str(), theTree);
		m_sConfigPath = theTree.get<std::string>("config.server.linroot");
		m_ConfigPort = theTree.get<int>("config.system.port");
		m_CountWidth = theTree.get<short>("config.system.countwidth");
	}
	catch(boost::property_tree::xml_parser_error ex)	{
		std::cerr << ex.what() << '\n';
	}
}
