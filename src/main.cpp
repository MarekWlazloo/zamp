#include <iostream>
#include <dlfcn.h>
#include <cassert>
#include "AbstractInterp4Command.hh"
#include <cstdio>
#include <sstream>
#include <map>
#include <memory>
#include <iomanip>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <vector>
#include "AccessControl.hh"
#include "Port.hh"
#include "klient.cpp"
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include "xmlinterp.hh"
// asdwqe
#define LINE_SIZE 500
using namespace std;
using namespace xercesc;


bool ExecPreprocesor(const char *NazwaPliku, istringstream &IStrm4Cmds)
{
string Cmd4Preproc =  "cpp -P ";
char Line[LINE_SIZE];
ostringstream OTmpStrm;
Cmd4Preproc += *NazwaPliku;
FILE *pProc = popen(Cmd4Preproc.c_str(),"r");
if (!pProc) return false;
while (fgets(Line,LINE_SIZE,pProc)) {
OTmpStrm << Line;
}
IStrm4Cmds.str(OTmpStrm.str());
return pclose(pProc) == 0;
}

class LibInterface {
  
  void *pLibHnd = nullptr;
 // AbstractInterp4Command *(*pCreateCmd)(void) = nullptr;
  public:
  AbstractInterp4Command *(*pCreateCmd)(void) = nullptr;
  
  ~LibInterface() { if (pLibHnd) dlclose(pLibHnd);}


  bool init(const char *sFileName);
  AbstractInterp4Command *CreateCmd(){ return pCreateCmd(); }
};

bool LibInterface::init(const char *FileName){
   pLibHnd = dlopen(FileName,RTLD_LAZY);
   if (!pLibHnd) {
    cerr << "!!! Brak biblioteki: "<< FileName << endl;
    return false;
   }
  void *pFun = dlsym(pLibHnd,"CreateCmd");
  if (!pFun) {
    cerr << "!!! Nie znaleziono funkcji CreateCmd" << endl;
    return 1;
  }
  pCreateCmd = reinterpret_cast<AbstractInterp4Command* (*)(void)>(pFun);
   return true;
  
}

class Set4LibInterface{
private:
  std::map<std::string, std::shared_ptr<LibInterface>> Map;
public:
    Set4LibInterface(){
        Map["libInterp4Rotate.so"] = std::make_shared<LibInterface>();
        Map["libInterp4Move.so"] = std::make_shared<LibInterface>();
        Map["libInterp4Set.so"] = std::make_shared<LibInterface>();
        Map["libInterp4Pause.so"] = std::make_shared<LibInterface>();
    }

    void init(){
      const char* libraryNames[] = {"libInterp4Rotate.so", "libInterp4Move.so", "libInterp4Set.so", "libInterp4Pause.so"};
      for(int i = 0; i < 4; i++){
      auto lib = Map.find(libraryNames[i]);
      if (lib != Map.end()) {
      bool spr = lib->second->init(libraryNames[i]);
      if(!spr){
        cerr << " Nie udało się zainicjować biblioteki: "<< libraryNames[i] << endl;
      }
      } 
      else{
        cerr << "Nie znaleziono bilbioteki:" << libraryNames[i] << endl;
      }
      }
    }

    AbstractInterp4Command* CreateCmd(const char* libName) {
        auto lib = Map.find(libName);
        if (lib != Map.end()) {
            return lib->second->CreateCmd();
        }
        return nullptr;
    }
};


bool ExecActions(istream &IStrm4Cmds, AbstractInterp4Command* &rInterp, Set4LibInterface &Interf) {
    string str;

    while (IStrm4Cmds >> str) {
      rInterp = nullptr;
        if (str == "Move"){
            rInterp = Interf.CreateCmd("libInterp4Move.so");
        } else if (str == "Set"){
            rInterp = Interf.CreateCmd("libInterp4Set.so");
        } else if (str == "Pause"){
            rInterp = Interf.CreateCmd("libInterp4Pause.so");
        } else if (str == "Rotate"){
            rInterp = Interf.CreateCmd("libInterp4Rotate.so");
        }
        else{
          cerr << "Blad! Nie udało się znależć odpowiedniej wtyczki" << endl;
          return false;
        }

        if(rInterp){
            if(rInterp->ReadParams(IStrm4Cmds)){
               rInterp->PrintCmd();
              // if(rInterp->ExecCmd()){

              // } else{
              //   cerr << "Wykonanie polecena nie powidło się!" << endl;
              //   return false;
              // }
            } else{
              cerr << "Nie udało się wczytać parametrów polcenia" << endl;
              return false;
            }

        } else{
            cerr << "Błąd! Nie udało się utworzyć instancji interpretera" << endl;
            return false; 
        }
    }

    return true;
}

bool ReadFile(const char* sFileName, Configuration &rConfig)
{
   try {
            XMLPlatformUtils::Initialize();
   }
   catch (const XMLException& toCatch) {
            char* message = XMLString::transcode(toCatch.getMessage());
            cerr << "Error during initialization! :\n";
            cerr << "Exception message is: \n"
                 << message << "\n";
            XMLString::release(&message);
            return 1;
   }

   SAX2XMLReader* pParser = XMLReaderFactory::createXMLReader();

   pParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
   pParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
   pParser->setFeature(XMLUni::fgXercesDynamic, false);
   pParser->setFeature(XMLUni::fgXercesSchema, true);
   pParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);

   pParser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);

   DefaultHandler* pHandler = new XMLInterp4Config(rConfig);
   pParser->setContentHandler(pHandler);
   pParser->setErrorHandler(pHandler);

   try {
     
     if (!pParser->loadGrammar("config/config.xsd",
                              xercesc::Grammar::SchemaGrammarType,true)) {
       cerr << "!!! Plik grammar/actions.xsd, '" << endl
            << "!!! ktory zawiera opis gramatyki, nie moze zostac wczytany."
            << endl;
       return false;
     }
     pParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse,true);
     pParser->parse(sFileName);
   }
   catch (const XMLException& Exception) {
            char* sMessage = XMLString::transcode(Exception.getMessage());
            cerr << "Informacja o wyjatku: \n"
                 << "   " << sMessage << "\n";
            XMLString::release(&sMessage);
            return false;
   }
   catch (const SAXParseException& Exception) {
            char* sMessage = XMLString::transcode(Exception.getMessage());
            char* sSystemId = xercesc::XMLString::transcode(Exception.getSystemId());

            cerr << "Blad! " << endl
                 << "    Plik:  " << sSystemId << endl
                 << "   Linia: " << Exception.getLineNumber() << endl
                 << " Kolumna: " << Exception.getColumnNumber() << endl
                 << " Informacja: " << sMessage 
                 << endl;

            XMLString::release(&sMessage);
            XMLString::release(&sSystemId);
            return false;
   }
   catch (...) {
            cout << "Zgloszony zostal nieoczekiwany wyjatek!\n" ;
            return false;
   }

   delete pParser;
   delete pHandler;
   return true;
}

int main(int argc, char **argv)
{
    Configuration Config;

  Set4LibInterface Libs;
  Libs.init();

    if (!ReadFile("config/config.xml", Config)) {
        cout << "Błąd przetwarzania pliku XML." << endl;
        return 1;
    }

           for (const auto& lib : Config.libs)
        {
            cout << "  " << lib << endl;
        }
        cout << "NAZWY" << endl;
                for (const auto& lib : Config.ObjNames)
        {
            cout << "  " << lib << endl;
        }
        cout << "RGB" << endl;
                for (const auto& lib : Config.ObjRGB)
        {

            cout << "  " << lib << endl;
        }
        cout << "ROTXYZ_DEG" << endl;
                for (const auto& lib : Config.ObjRotXYZ_deg)
        {
          
            cout << "  " << lib << endl;
        }
        cout << "SCALE" << endl;
                for (const auto& lib : Config.ObjScale)
        {
          
            cout << "  " << lib << endl;
        }
        cout << "SHIFT" << endl;
                for (const auto& lib : Config.ObjShift)
        {
          
            cout << "  " << lib << endl;
        }
        cout << "TRANS" << endl;
               for (const auto& lib : Config.ObjTrans_m)
        {
            cout << "  " << lib << endl;
        }


  /*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
  
  cout << "Port: " << PORT << endl;
  Scene               Scn;
  int                 Socket4Sending;   

  if (!OpenConnection(Socket4Sending)) return 1;
  
  Sender   ClientSender(Socket4Sending,&Scn);
  //  thread   Thread4Sending(Fun_Sender, Socket4Sending, &ClientSender);

  thread   Thread4Sending(Fun_CommunicationThread,&ClientSender);
  const char *sConfigCmds =
"Clear\n"
"AddObj Name=Podstawa1 RGB=(20,200,200) Scale=(4,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,20) Trans_m=(-1,3,0)\n"
"AddObj Name=Podstawa1.Ramie1 RGB=(200,0,0) Scale=(3,3,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(4,0,0)\n"
"AddObj Name=Podstawa1.Ramie1.Ramie2 RGB=(100,200,0) Scale=(2,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(3,0,0)\n"       
"AddObj Name=Podstawa2 RGB=(20,200,200) Scale=(4,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(-1,-3,0)\n"
"AddObj Name=Podstawa2.Ramie1 RGB=(200,0,0) Scale=(3,3,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(4,0,0)\n"
"AddObj Name=Podstawa2.Ramie1.Ramie2 RGB=(100,200,0) Scale=(2,2,1) Shift=(0.5,0,0) RotXYZ_deg=(0,-45,0) Trans_m=(3,0,0)\n";


  cout << "Konfiguracja:" << endl;
  cout << sConfigCmds << endl;
  
  Send(Socket4Sending,sConfigCmds);
  

  cout << "Akcja:" << endl;    
  for (GeomObject &rObj : Scn._Container4Objects) {
    usleep(20000);
    ChangeState(Scn);
    Scn.MarkChange();
    usleep(100000);
  }
  usleep(100000);

  //-------------------------------------
  // Należy pamiętać o zamknięciu połączenia.
  // Bez tego serwer nie będzie reagował na
  // nowe połączenia.
  //
  cout << "Close\n" << endl; // To tylko, aby pokazac wysylana instrukcje
  Send(Socket4Sending,"Close\n");
  ClientSender.CancelCountinueLooping();
  Thread4Sending.join();
  close(Socket4Sending);


/*#######################################################################################*/

  /*
  istringstream IStrm4Cmds;

  if(argc < 2){
    cerr << "ERROR" << endl;
    return 1;
  }

  if(!ExecPreprocesor(argv[1], IStrm4Cmds)){
    cerr << "ERROR" << endl;
    return 2;
  }
  cout << "Zawartość pliku przetworzonego przez preprocessor" << endl;
  cout << endl;
  cout << IStrm4Cmds.str() << endl;

  AbstractInterp4Command *pCmdS = nullptr;

  //cout << IStrm4Cmds.str() << endl;
  //LibInterface MoveCmdInterf;
  //AbstractInterp4Command *pCmdS = Libs.CreateCmd("libInterp4Pause.so");

  //AbstractInterp4Command *pCmdS;
  cout << "Wczytanie parametrów z pliku do odpowiednich wtyczek, wyświetlenie wczytanych parametrów" << endl;
  cout << endl;
  ExecActions(IStrm4Cmds,pCmdS, Libs);

 // if(!ExecActions(IStrm4Cmds,*pCmd)){
 //  cerr << "ERROR" << endl;
 //  return 3;
 // }
*/
}
/*
TWORZENIE LISTY Z NAZWAMI BILBIOTEK,
TWORZENIE LISTY NAPISÓW (POLECENIA E PLIKU XML) I UDOSTEPNIANIE ICH KOLEJNYM KLASOM
ZAPAMIETYWANIE OBJEKTOW W JEDNYM NAPISIE (STRING) --> NASTEPNIE ADD

SERWER KOMPILUJE SIE Z QT5, Z QT6 ALE JEST PROBLEM

wczytywanie bibliotek podanych w pliku konfiguracyjnych,

*/
