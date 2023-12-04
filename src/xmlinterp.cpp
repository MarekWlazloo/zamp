#include <xercesc/util/PlatformUtils.hpp>
#include "xmlinterp.hh"
#include <cassert>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <algorithm>



using namespace std;

// std::list<int> destinationList = sourceList;
/*!
 * Konstruktor klasy. Tutaj należy zainicjalizować wszystkie
 * dodatkowe pola.
 */


/*!
 * Metoda wywoływana jest bezpośrednio przed rozpoczęciem
 * przetwarzana dokumentu XML.
 */
void XMLInterp4Config::startDocument()
{
  cout << "*** Rozpoczecie przetwarzania dokumentu XML." << endl;
}


/*!
 * Metoda wywoływana jest bezpośrednio po zakończeniu
 * przetwarzana dokumentu XML.
 */
void XMLInterp4Config::endDocument()
{
  cout << "=== Koniec przetwarzania dokumentu XML." << endl;
}





/*!
 * Analizuje atrybuty elementu XML \p "Lib" i odpowiednio je interpretuje.
 * \param[in] rAttrs - atrybuty elementu XML \p "Lib".
 */
void XMLInterp4Config::ProcessLibAttrs(const xercesc::Attributes  &rAttrs)
{
 if (rAttrs.getLength() != 1) {
      cerr << "Zla ilosc atrybutow dla \"Lib\"" << endl;
      exit(1);
 }

 char* sParamName = xercesc::XMLString::transcode(rAttrs.getQName(0));

 if (strcmp(sParamName,"Name")) {
      cerr << "Zla nazwa atrybutu dla Lib" << endl;
      exit(1);
 }         

 XMLSize_t  Size = 0;
 char* sLibName = xercesc::XMLString::transcode(rAttrs.getValue(Size));

 // rConfig.libs.push_back(sLibName);
char* copiedLibName = new char[strlen(sLibName) + 1];
strcpy(copiedLibName, sLibName);

rConfig.libs.push_back(copiedLibName);

 cout << "  Nazwa biblioteki: " << sLibName << endl;

 // Tu trzeba wpisać własny kod ...

 xercesc::XMLString::release(&sParamName);
 xercesc::XMLString::release(&sLibName);
}


/*!
 * Analizuje atrybuty. Sprawdza czy ich nazwy są poprawne. Jeśli tak,
 * to pobiera wartości atrybutów (w postaci napisów) i przekazuje ...
 * \param[in] rAttrs - atrybuty elementu XML \p "Cube".
 */
/**/
void XMLInterp4Config::ProcessCubeAttrs(const xercesc::Attributes& rAttrs)
{
    if (rAttrs.getLength() < 1) {
        cerr << "Zla ilosc atrybutow dla \"Cube\"" << endl;
        exit(1);
    }

    XMLSize_t Index = 0;

    // Sprawdzamy, czy pierwszy atrybut to "Name"
    if (strcmp(xercesc::XMLString::transcode(rAttrs.getQName(Index)), "Name") != 0) {
        cerr << "Zly atrybut dla \"Cube\"" << endl;
        exit(1);
    }

    const char* sValue_Name = xercesc::XMLString::transcode(rAttrs.getValue(Index));
    rConfig.ObjNames.push_back(sValue_Name);

    // petla po atrybutach
    for (Index = 1; Index < rAttrs.getLength(); ++Index) {
        const char* sName_Attr = xercesc::XMLString::transcode(rAttrs.getQName(Index));
        const char* sValue_Attr = xercesc::XMLString::transcode(rAttrs.getValue(Index));

        // szukanie atrybutu i dodawanie do odpowiedniej listy
        if (strcmp(sName_Attr, "Shift") == 0) {
            rConfig.ObjShift.push_back(sValue_Attr);
        } else if (strcmp(sName_Attr, "Scale") == 0) {
            rConfig.ObjScale.push_back(sValue_Attr);
        } else if (strcmp(sName_Attr, "RotXYZ_deg") == 0) {
            rConfig.ObjRotXYZ_deg.push_back(sValue_Attr);
        } else if (strcmp(sName_Attr, "Trans_m") == 0) {
            rConfig.ObjTrans_m.push_back(sValue_Attr);
        } else if (strcmp(sName_Attr, "RGB") == 0) {
            rConfig.ObjRGB.push_back(sValue_Attr);
        }

        // Zwolnij zaalokowaną pamięć

    }

    // Zwolnij zaalokowaną pamięć
 
}
/*

void XMLInterp4Config::ProcessCubeAttrs(const xercesc::Attributes& rAttrs)
{
    if (rAttrs.getLength() < 1) {
        cerr << "Zla ilosc atrybutow dla \"Cube\"" << endl;
        exit(1);
    }

    char* sName_Name = xercesc::XMLString::transcode(rAttrs.getQName(0));
    XMLSize_t Index = 0;

    // Sprawdzamy, czy pierwszy atrybut to "Name"
    if (strcmp(sName_Name, "Name") != 0) {
        cerr << "Zly atrybut dla \"Cube\"" << endl;
        exit(1);
    }

    char* sValue_Name = xercesc::XMLString::transcode(rAttrs.getValue(Index));

    // obiekt name wsyetpuje na pewno
    char* name = new char[strlen(sValue_Name) + 1];
    strcpy(name, sValue_Name);
    rConfig.ObjNames.push_back(name);

    // petla po atrybutach
    for (Index = 1; Index < rAttrs.getLength(); ++Index) {
        char* sName_Attr = xercesc::XMLString::transcode(rAttrs.getQName(Index));
        char* sValue_Attr = xercesc::XMLString::transcode(rAttrs.getValue(Index));

      // szukanie atrybutu i dodawanie do odpowiedniej listy

        if (strcmp(sName_Attr, "Shift") == 0) {
            char* copy = new char[strlen(sValue_Attr) + 1];
            strcpy(copy, sValue_Attr);
            rConfig.ObjShift.push_back(copy);
        } else if (strcmp(sName_Attr, "Scale") == 0) {
            char* copy = new char[strlen(sValue_Attr) + 1];
            strcpy(copy, sValue_Attr);
            rConfig.ObjScale.push_back(copy);
        } else if (strcmp(sName_Attr, "RotXYZ_deg") == 0) {
            char* copy = new char[strlen(sValue_Attr) + 1];
            strcpy(copy, sValue_Attr);
            rConfig.ObjRotXYZ_deg.push_back(copy);
        } else if (strcmp(sName_Attr, "Trans_m") == 0) {
            char* copy = new char[strlen(sValue_Attr) + 1];
            strcpy(copy, sValue_Attr);
            rConfig.ObjTrans_m.push_back(copy);
        } else if (strcmp(sName_Attr, "RGB") == 0) {
            char* copy = new char[strlen(sValue_Attr) + 1];
            strcpy(copy, sValue_Attr);
            rConfig.ObjRGB.push_back(copy);
        }

        
        xercesc::XMLString::release(&sName_Attr);
        xercesc::XMLString::release(&sValue_Attr);
    }


    xercesc::XMLString::release(&sName_Name);
    xercesc::XMLString::release(&sValue_Name);
}
*/







/*!
 * Wykonuje operacje związane z wystąpieniem danego elementu XML.
 * W przypadku elementu \p "Action" będzie to utworzenie obiektu
 * reprezentującego odpowiednią działanie robota.
 * W przypadku elementu \p "Parameter" będzie to zapisanie parametrów
 * związanych z danym działaniem.
 * \param[in] rElemName - nazwa elementu XML.
 * \param[in] rAttrs - atrybuty napotkanego elementu XML.
 */
void XMLInterp4Config::WhenStartElement( const std::string           &rElemName,
		                         const xercesc::Attributes   &rAttrs
                                       )
{
  if (rElemName == "Lib") {
    ProcessLibAttrs(rAttrs);   return;   
  }

  if (rElemName == "Cube") {
    ProcessCubeAttrs(rAttrs);  return;
  }
}




/*!
 * Metoda jest wywoływana po napotkaniu nowego elementu XML, np.
 * gdy zostanie napotkany element:
   \verbatim
     <Action Name="Rotate">
   \endverbatim
 *  to poprzez parametr \e xscElemName jest dostęp do nazwy elementu
 *  tzn. \p Action, 
 *  \param[in] pURI - (Uniform Resource Identifier) jeżeli nie jest on wyspecyfikowany
 *                    poprzez użycie atrybutów \p xmlns (o ile jego użycie jest przewidziane w gramatyce,
 *                    w tym przykładzie nie jest to przewidziane), to będzie to napis pusty.
 *  \param[in] pLocalName -  umożliwia dostęp do nazwy elementu XML.
 *                 W podanym przykładzie nazwą elementu XML jest "Action".
 *  \param[in] pQName - pełna kwalifikowana nazwa. To ma znaczenie, gdy użyta jest przestrzeń nazwa.
 *                      Wówczas poprzez ten parametr można otrzymać nazwę elementu wraz z prefiksem
 *                      przestrzeni nazwa. Jeśli przestrzeń nazw nie jest użyta (taka jak w tym
 *                      przykładzie), to \p pQName i \p pLocalName dostaczają identyczne napisy.
 *  \param[in] rAttrs -  lista atrybutów danego symbolu XML.
 *                 W przykładzie pokazanym powyżej listę atrybutów
 *                 będą stanowiły napisy:
 */
/*
 * Te metode nalezy odpowiednio zdekomponowac!!!
 */
void XMLInterp4Config::startElement(  const   XMLCh* const            pURI,
                                      const   XMLCh* const            pLocalName,
                                      const   XMLCh* const            pQName,
				      const   xercesc::Attributes&    rAttrs
                                    )
{
  char* sElemName = xercesc::XMLString::transcode(pLocalName);
  cout << "+++ Poczatek elementu: "<< sElemName << endl;

  WhenStartElement(sElemName, rAttrs);

  xercesc::XMLString::release(&sElemName);
}




/*!
 * Metoda zostaje wywołana w momencie zakończenia danego elementu
 * XML, np. jeżeli w pliku jest wpis:
   \verbatim
     <Lib Name="Rotate">
     </Lib>
   \endverbatim
 * to metoda ta zostanie wywołana po napotkaniu znacznika:
 * \verbatim</Lib>\endverbatim
 * Jeżeli element XML ma formę skróconą, tzn.
   \verbatim
     <Parametr Name="Rotate"/>
   \endverbatim
 * to wówczas wywołana metoda wywołana zostanie w momencie
 * napotkania sekwencji "/>"
 *  \param[in] pURI - (Uniform Resource Identifier) jeżeli nie jest on wyspecyfikowany
 *                    poprzez użycie atrybutów \p xmlns (o ile jego użycie jest przewidziane w gramatyce,
 *                    w tym przykładzie nie jest to przewidziane), to będzie to napis pusty.
 *  \param[in] pLocalName -  umożliwia dostęp do nazwy elementu XML.
 *                 W podanym przykładzie nazwą elementu XML jest "Lib".
 *  \param[in] pQName - pełna kwalifikowana nazwa. To ma znaczenie, gdy użyta jest przestrzeń nazwa.
 *                      Wówczas poprzez ten parametr można otrzymać nazwę elementu wraz z prefiksem
 *                      przestrzeni nazwa. Jeśli przestrzeń nazw nie jest użyta (taka jak w tym
 *                      przykładzie), to \p pQName i \p pLocalName dostaczają identyczne napisy.
 *                      
 */
void XMLInterp4Config::endElement(  const   XMLCh* const    pURI,
                                    const   XMLCh* const    pLocalName,
                                    const   XMLCh* const    pQName
                                 )
{
   char* sURI =  xercesc::XMLString::transcode(pURI);
   char* sElemName = xercesc::XMLString::transcode(pLocalName);
   char* sQName =  xercesc::XMLString::transcode(pQName);
   cout << "   URI: " << sURI << endl;
   cout << " QName: " << sQName << endl;
   cout << "----- Koniec elementu: "<< sElemName << endl;

   xercesc::XMLString::release(&sElemName);
}




/*!
 * Metoda wywoływana jest, gdy napotkany zostanie błąd fatalny,
 * np.
  \verbatim
    <Action Name="Rotate">
    </Parametr>
  \endverbatim
 * \param[in] rException - zawiera informacje dotyczące błędu w dokumencie,
 *                         linii, kolumny itp.
 */
void XMLInterp4Config::fatalError(const xercesc::SAXParseException&  rException)
{
   char* sMessage = xercesc::XMLString::transcode(rException.getMessage());
   char* sSystemId = xercesc::XMLString::transcode(rException.getSystemId());

   cerr << "Blad fatalny! " << endl
        << "    Plik:  " << sSystemId << endl
        << "   Linia: " << rException.getLineNumber() << endl
        << " Kolumna: " << rException.getColumnNumber() << endl
        << " Informacja: " << sMessage 
        << endl;

   xercesc::XMLString::release(&sMessage);
   xercesc::XMLString::release(&sSystemId);
}

/*!
 * Metoda jest wywoływana, gdy napotkany zostanie błąd, który nie
 * jest traktowany jako fatalny. W ten sposób traktowane są błędy
 * występujące w opisie gramatyki dokumentu.
 * \param[in] rException - zawiera informacje dotyczące błędu. Informacje
 *                     te to opis błędu oraz numer linii, w której
 *                     wystąpił błąd.
 */
void XMLInterp4Config::error(const xercesc::SAXParseException&  rException)
{
  cerr << "Blad ..." << endl;

  /*
   * Tutaj należy wstawić odpowiedni kod. Tekst wyświetlany powyżej
   * jest tylko "atrapą".
   */
}


/*!
 * Metoda wywołana jest w przypadku błędów, które nie są fatalne
 * i mogą być traktowane jako ostrzeżenia.
 * \param[in] rException - zawiera informacje dotyczące błędu w dokumencie,
 *                         linii, kolumny itp.
 */
void XMLInterp4Config::warning(const xercesc::SAXParseException&  rException)  
{
  cerr << "Ostrzezenie ..." << endl;

  /*
   * Tutaj należy wstawić odpowiedni kod. Tekst wyświetlany powyżej
   * jest tylko "atrapą".
   */
}
