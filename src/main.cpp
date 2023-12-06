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
#include <chrono>
#include <mutex>
#include <thread>
#include <fstream>
#include <memory>
#include <thread>
#include <vector>

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


std::string FormatNumericList(const std::string& numericList) {
    std::string result = "(";

    std::istringstream iss(numericList);
    std::string number;

    while (iss >> number) {
        result += number;

        // Dodaj przecinek, jeśli to nie ostatni element
        if (iss >> std::ws && iss.peek() != ')') {
            result += ',';
        }
    }

    result += ')';
    return result;
}

// Funkcja generująca napis XML
string GenerateConfigCmds(Configuration& Config) {
    std::ostringstream configCmds;
    configCmds << "Clear\n";

    auto nameIt = Config.ObjNames.begin();
    auto shiftIt = Config.ObjShift.begin();
    auto scaleIt = Config.ObjScale.begin();
    auto rotXYZIt = Config.ObjRotXYZ_deg.begin();
    auto transIt = Config.ObjTrans_m.begin();
    auto rgbIt = Config.ObjRGB.begin();

    while (nameIt != Config.ObjNames.end()) {
        configCmds << "AddObj Name=" << *nameIt << " RGB=" << FormatNumericList(*rgbIt)
                   << " Scale=" << FormatNumericList(*scaleIt)
                   << " Shift=" << FormatNumericList(*shiftIt)
                   << " RotXYZ_deg=" << FormatNumericList(*rotXYZIt)
                   << " Trans_m=" << FormatNumericList(*transIt) << "\n";

        ++nameIt;
        ++shiftIt;
        ++scaleIt;
        ++rotXYZIt;
        ++transIt;
        ++rgbIt;
    }


    string cos = configCmds.str();  
    return cos;
}

void ExecuteInterpreter(AbstractInterp4Command* interpreter, AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann) {
    interpreter->ExecCmd(rScn,sMobObjName, rComChann);
}
bool Exec(std::istream& IStrm4Cmds, Set4LibInterface& Interf, AbstractScene &rScn, AbstractComChannel& rComChann,  AbstractInterp4Command* &rInterp, const char *sMobObjName, const char *sConfigCmds,Configuration &Config) {
    std::string str;
    
    std::list<AbstractInterp4Command*> InterpList; // Otworz komunikacje
    if (openConnection(rComChann.GetSocket())) {
        std::cout << "Udało się połączyć z serwerem!" << std::endl;
    } else {
        std::cerr << "Nie udało się połączyć z serwerem!" << std::endl;
    }
        auto it1 = Config.ObjTrans_m.begin();
        auto it2 = Config.ObjRotXYZ_deg.begin();
        auto it3 = Config.ObjNames.begin();
                while (it1 != Config.ObjTrans_m.end()) {
                  AbstractMobileObj* obj = new AbstractMobileObj();
            std::istringstream ss1(*it1);
            std::istringstream ss2(*it2);
            std::istringstream ss3(*it3);

            Vector3D pos;
            double roll, pitch, yaw;
            string name;

            // Przypisanie wartości do zmiennych z obu list
            if ((ss1 >> pos[0] >> pos[1] >> pos[2]) && (ss2 >> roll >> pitch >> yaw)) {
              ss3 >> name;
            } else {
                // Błąd parsowania
                std::cerr << "Błąd parsowania dla stringów: " << *it1 << " i " << *it2 << std::endl;
            }

            ++it1;
            ++it2;
            ++it3;
                 obj->lockObj();
 obj->SetName(name);
 obj->SetPosition_m(pos);
 obj->SetAng_Roll_deg(roll);
 obj->SetAng_Pitch_deg(pitch);
 obj->SetAng_Yaw_deg(yaw);
 obj->unlockObj();
 rScn.AddMobileObj(obj);
        }

     send(rComChann.GetSocket() ,sConfigCmds);
    while (IStrm4Cmds >> str) {
        if (str == "Begin_Parallel_Actions") {
            break;
        }
    }
    // Dopóki nie napotkasz słowa End_Parallel_Actions
    while (IStrm4Cmds >> str) {
      cout << str << endl;
        if (str == "End_Parallel_Actions") {
              std::list<std::thread> List4Th;
      //        cout << "11111" << endl;
            if (InterpList.empty()){
              std::cerr << "Nie udało się utworzyć interpreterow" << endl;
              exit(-1);
            }

//cout << "22222" << endl;
for (auto& Interp : InterpList) {
    std::thread Th(ExecuteInterpreter, Interp, std::ref(rScn), sMobObjName, std::ref(rComChann));
    List4Th.push_back(std::move(Th));   
}

//cout << "33333" << endl;
  for (std::thread &rTh : List4Th) {
    if (rTh.joinable()) rTh.join();
    //sleep(1);
  }
  //sleep(5);
  cout << "4444" << endl;
  cout <<"============================"<< endl;
   //cout << InterpList.size() << endl;
  //cout << List4Th.size() << endl;

            List4Th.clear();
            InterpList.clear(); 
        }
AbstractInterp4Command* rInterp = nullptr;
        if(str == "Move" || str == "Set" || str == "Rotate" || str == "Pause"){
        

        // Tworzenie instancji interpretera w zależności od rodzaju akcji
        if (str == "Move") {
            rInterp = Interf.CreateCmd("libInterp4Move.so");
        } else if (str == "Set") {
            rInterp = Interf.CreateCmd("libInterp4Set.so");
        } else if (str == "Pause") {
            rInterp = Interf.CreateCmd("libInterp4Pause.so");
        } else if (str == "Rotate") {
            rInterp = Interf.CreateCmd("libInterp4Rotate.so");
        } else {
           // std::cerr << "Błąd! Nie udało się znaleźć odpowiedniej wtyczki" << std::endl;
        }

        if (rInterp) {
            // Wczytywanie parametrów polecenia
            if (rInterp->ReadParams(IStrm4Cmds)) {
                InterpList.push_back(rInterp);
                cout << "Parametry dla " << str << " załadowane" << endl;
            } else {
                std::cerr << "Nie udało się wczytać parametrów polecenia" << std::endl;
            }
        } else {
            std::cerr << "Błąd! Nie udało się utworzyć instancji interpretera" << std::endl;
        }
    }
    }
    

    return true;
}

int main(int argc, char **argv)
{
  /*thread Th1(Fun4Thread, "prog1.cmd");
  thread Th2(Fun4Thread, "prog2.cmd");
  Th1.join();
  Th2.join();*/

  istringstream IStrm4Cmds;
  Configuration Config;
  Set4LibInterface Libs;

  Libs.init();

  if(argc < 2){
    cerr << "ERROR" << endl;
    return 1;
  }

  if(!ExecPreprocesor(argv[1], IStrm4Cmds)){
    cerr << "ERROR" << endl;
    return 2;
  }


    if (!ReadFile("config/config.xml", Config)) {
        cout << "Błąd przetwarzania pliku XML." << endl;
        return 1;
    }
    string str = GenerateConfigCmds(Config);
std::vector<char> Buffer(str.begin(), str.end());
Buffer.push_back('\0');
const char *sConfigCmds = Buffer.data();
cout << sConfigCmds << endl;

  AbstractInterp4Command *pCmdS = nullptr;
  AbstractScene               Scn;
  int socket;
  AbstractComChannel comChannel(socket);
  const char* sMobObjName = "cosik";

   Exec(IStrm4Cmds, Libs, Scn, comChannel, pCmdS, sMobObjName, sConfigCmds, Config);


    return 0;
/*
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

*/
}
/*
Diagram od bkr
czytaj slowo do napotkania -->begin parralel actions
czytaj dalej, JESLI nic nie ma to błąd
ogolnie to czytaj az do END parallel actions
PO BEGIN ->>>>>> po drodze diagram 1 od
uwtorz instacje
dodaj do kolekcji instancje
wczytaj parametry polecenia
wykonaj polecenie w osobnym watku 
ROBISZ TO DO CZASU NAPOTKANIA NA end_parallel_actions
gdy napotkasz na end parallel to poczekaj na wykonanie wszystkich wątków i usun kolekcje interpreterow

*/