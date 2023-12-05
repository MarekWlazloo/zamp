#ifndef ABSTRACTSCENE_HH
#define ABSTRACTSCENE_HH


#include "AbstractMobileObj.hh"

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

public:
virtual ~AbstractScene() {}

 void AddMobileObj(std::shared_ptr<AbstractMobileObj> mobileObj) // TO MOZE BYC HUJOWE
{
    mobileObjMap.emplace(mobileObj->GetName(), mobileObj);
}

std::shared_ptr<AbstractMobileObj> FindMobileObject(const std::string &mobileObjectName) const
{
    return mobileObjMap.at(mobileObjectName);
}


private:
    std::map<std::string, std::shared_ptr<AbstractMobileObj>> mobileObjMap;
  };

#endif
