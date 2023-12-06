#ifndef ABSTRACTSCENE_HH
#define ABSTRACTSCENE_HH


#include "AbstractMobileObj.hh"
#include <memory>
#include <map>
#include <list>
#include <mutex>


/*!
 * \file 
 * \brief Zawiera definicję klasy abstrakcyjnej AbstractScene
 *
 *  Zawiera definicję klasy abstrakcyjnej AbstractScene.
 *  Wyznacza ona niezbędny interfejs klas pochodnych.
 */

 /*!
  * \brief Definiuje interfejs klasy modelującej scenę roboczą.
  *
  * Definiuje interfejs klasy modelującej scenę roboczą,
  * która zawiera kolekcję obiektów mobilnych.
  */
class AbstractScene {
std::list<AbstractMobileObj*>  mobileObjList;
mutex scnBlock;
public:
    virtual ~AbstractScene() {
        // Usuń obiekty, których zarządzasz, gdy obiekt AbstractScene jest niszczony.
        for (auto& obj : mobileObjList) {
            delete obj;
        }
        mobileObjList.clear();
    }

    void AddMobileObj(AbstractMobileObj* mobileObj) {
        mobileObjList.push_back(mobileObj);
    }

    AbstractMobileObj* FindMobileObject(const std::string& mobileObjectName) {
        for (auto& obj : mobileObjList) {
            if (obj->GetName() == mobileObjectName) {
                return obj;
            }
        }
        return nullptr;
    }
    void scnBlockfun(){scnBlock.lock();}
    void scnUnlockfun(){scnBlock.unlock();}
  };

#endif
