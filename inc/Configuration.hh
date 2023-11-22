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


class Configuration {
  //...
  public:
  std::list<char*> libs;
  std::list<char*> ObjNames;
  std::list<char*> ObjShift;
  std::list<char*> ObjScale;
  std::list<char*> ObjRotXYZ_deg;
  std::list<char*> ObjTrans_m;
  std::list<char*> ObjRGB;
};


#endif
