#include "linbak.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


void Linbak::ReadConfig(std::string sFileName) {
	using boost::property_tree::ptree;

	ptree theTree;
	read_xml(sFileName, theTree);
	m_sConfigPath = theTree.get<std::string>("config.client.winroot");
	m_ConfigPort = theTree.get<int>("config.system.port");
	m_CountWidth = theTree.get<short>("config.system.countwidth");
}