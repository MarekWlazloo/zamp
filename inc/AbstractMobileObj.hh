#ifndef ABSGTRACTMOBILEOBJ_HH
#define ABSGTRACTMOBILEOBJ_HH


/*!
 * \file 
 * \brief Zawiera definicję klasy abstrakcyjnej AbstractMobileObj
 *
 *  Zawiera definicję klasy abstrakcyjnej AbstractMobileObj.
 *  Wyznacza ona niezbędny interfejs klas pochodnych.
 */

#include <string>
#include "Vector3D.hh"
#include <mutex>

   /*!
    * \brief Definiuje interfejs dla obiektów mobilnych.
    *
    * Definiuje interfejs dla obiektów mobilnych.
    * Zakładamy, że przód obiektu jest wskazywany przez strzałkę osi OX.
    * Nazwy metod są obowiązujące.
    */
    class AbstractMobileObj {
      double ang_yaw_deg = 0; // z 
      double and_pich_deg = 0; // y
      double ang_roll_deg = 0; //x 
      Vector3D pos;
      string name;
      mutex objLock;
     public:

       virtual ~AbstractMobileObj() {}
      
       /*!
        * \brief Udostępnia wartość kąta \e roll.
        *
        *  Udostępnia wartość kąta \e pitch reprezentuje rotację
        *  zgodnie z ruchem wskazówek zegara wokół osi \e OX.
        *  \return Wartość kąta \e roll wyrażona w stopniach.
        */
       virtual double GetAng_Roll_deg() { return ang_roll_deg; }
       /*!
        * \brief Udostępnia wartość kąta \e yaw.
        *
        *  Udostępnia wartość kąta \e pitch reprezentuje rotację
        *  zgodnie z ruchem wskazówek zegara wokół osi \e OY.
        *  \return Wartość kąta \e pitch wyrażona w stopniach.
        */
       virtual double GetAng_Pitch_deg() const = 0; { return and_pich_deg; }
       /*!
        * \brief Udostępnia wartość kąta \e yaw.
        *
        *  Udostępnia wartość kąta \e yaw reprezentuje rotację
        *  zgodnie z ruchem wskazówek zegara wokół osi \e OZ.
        *  \return Wartość kąta \e yaw wyrażona w stopniach.
        */
       virtual double GetAng_Yaw_deg() const { return ang_yaw_deg; }

       /*!
        * \brief Zmienia wartość kąta \e roll.
        *
        *  Zmienia wartość kąta \e roll.
        *  \param[in] Ang_Roll_deg - nowa wartość kąta \e roll wyrażona w stopniach.
        */
       virtual void SetAng_Roll_deg(double Ang_Roll_deg) { ang_roll_deg = Ang_Roll_deg; }
       /*!
        * \brief Zmienia wartość kąta \e pitch.
        *
        *  Zmienia wartość kąta \e pitch.
        *  \param[in] Ang_Pitch_deg - nowa wartość kąta \e pitch wyrażona w stopniach.
        */
       virtual void SetAng_Pitch_deg(double Ang_Pitch_deg) { and_pich_deg = Ang_Pitch_deg; }
       /*!
        * \brief Zmienia wartość kąta \e yaw.
        *
        *  Zmienia wartość kąta \e yaw.
        *  \param[in] Ang_Yaw_deg - nowa wartość kąta \e yaw wyrażona w stopniach.
        */
       virtual void SetAng_Yaw_deg(double Ang_Yaw_deg) { ang_yaw_deg = Ang_Yaw_deg; }

       /*!
        * \brief Udostępnia współrzędne aktualnej pozycji obiektu.
        *
        * Udostępnia współrzędne aktualnej pozycji obiektu
        * \return Współrzędne aktualnej pozycji obiektu. Przyjmuje się,
        *         że współrzędne wyrażone są w metrach.
        */
       virtual const Vector3D & GetPositoin_m() {return pos;}
       /*!
        * \brief Zmienia współrzędne aktualnej pozycji obiektu.
        *
        * Zmienia współrzędne aktualnej pozycji obiektu.
        * \param[in] rPos - nowe współrzędne obiektu. Przyjmuje się,
        *         że współrzędne wyrażone są w metrach.
        */
       virtual void SetPosition_m(const Vector3D &rPos) {pos = rPos;}

       /*!
        * \brief Zmienia nazwę obiektu.
        *
        *  Zmienia nazwę obiektu, która go identyfikuje.
        *  \param[in]  sName - nowa nazwa obiektu.
        */
        virtual void SetName(const char* sName) = 0;
       /*!
        * \brief Udostępnia nazwę obiektu.
        *  Udostępnia nazwę identyfikującą obiekt.
        *  \return Nazwa obiektu.
        */
        virtual const std::string & GetName() {return name;}

        virtual void lockObj() {objLock.lock();}

       virtual void unlockObj() {objLock.unlock();}
    };



#endif
