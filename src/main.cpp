#include <iostream>
#include <dlfcn.h>
#include <cassert>
#include "AbstractInterp4Command.hh"
#include <cstdio>
#include <sstream>
#include <map>
#include <memory>
#define LINE_SIZE 500

using namespace std;

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

class Set4LibInterface {
private:
  std::map<std::string, std::shared_ptr<LibInterface>> Map;
public:
    Set4LibInterface() {
        Map["libInterp4Rotate.so"] = std::make_shared<LibInterface>();
        Map["libInterp4Move.so"] = std::make_shared<LibInterface>();
        Map["libInterp4Set.so"] = std::make_shared<LibInterface>();
        Map["libInterp4Pause.so"] = std::make_shared<LibInterface>();

        // Inicjowanie wszystkich bibliotek automatycznie

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
  //  AbstractInterp4Command *rInterp;
    while (IStrm4Cmds >> str) {
        if (str == "Move") {
            rInterp = Interf.CreateCmd("libInterp4Move.so");
            rInterp->ReadParams(IStrm4Cmds);
            rInterp->PrintCmd();
        } else if (str == "Set") {
            rInterp = Interf.CreateCmd("libInterp4Set.so");
            rInterp->ReadParams(IStrm4Cmds);
            rInterp->PrintCmd();
        } else if (str == "Pause") {
            rInterp = Interf.CreateCmd("libInterp4Pause.so");
            rInterp->ReadParams(IStrm4Cmds);
            rInterp->PrintCmd();
        } else if (str == "Rotate") {
            rInterp = Interf.CreateCmd("libInterp4Rotate.so");
            rInterp->ReadParams(IStrm4Cmds);
            rInterp->PrintCmd();
        }
        else{
          cerr << "Blad!!" << endl;
        }
    }

    return true;
}

int main(int argc, char **argv)
{
  istringstream IStrm4Cmds;

  if(argc < 2){
    cerr << "ERROR" << endl;
    return 1;
  }

  if(!ExecPreprocesor(argv[1], IStrm4Cmds)){
    cerr << "ERROR" << endl;
    return 2;
  }

  Set4LibInterface Libs;
  Libs.init();
  AbstractInterp4Command *pCmdS = nullptr;

  //cout << IStrm4Cmds.str() << endl;
  //LibInterface MoveCmdInterf;
  //AbstractInterp4Command *pCmdS = Libs.CreateCmd("libInterp4Pause.so");

  //AbstractInterp4Command *pCmdS;
  ExecActions(IStrm4Cmds,pCmdS, Libs);

 // if(!ExecActions(IStrm4Cmds,*pCmd)){
 //  cerr << "ERROR" << endl;
 //  return 3;
 // }

}
