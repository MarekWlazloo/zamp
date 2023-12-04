#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH
#include <string>
#include <xercesc/util/XMLString.hpp>
//#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/sax/Locator.hpp>

#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

#include <algorithm>
#include <iostream>
#include <list>
using namespace std;


class Configuration {
  //...
  public:
  std::list<std::string> libs;
  std::list<std::string>ObjNames; //
  std::list<std::string>ObjShift;//
  std::list<std::string> ObjScale;//
  std::list<std::string>ObjRotXYZ_deg;//
  std::list<std::string> ObjTrans_m;//
  std::list<std::string>ObjRGB;
};


#endif
