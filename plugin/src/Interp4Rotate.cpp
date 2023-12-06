#include <iostream>
#include "Interp4Rotate.hh"
#include <sstream>
#include <thread>


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Rotate"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Rotate::CreateCmd();
}


/*!
 *
 */
Interp4Rotate::Interp4Rotate()
{}


/*!
 *
 */
void Interp4Rotate::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() <<" "<< ObjName <<" "<< AxisName <<" "<< AngularSpeed <<" "<< RotationAngle <<  endl;
  cout <<endl;
}


/*!
 *
 */
const char* Interp4Rotate::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Rotate::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{
AbstractMobileObj* obj =  rScn.FindMobileObject(ObjName);
if (obj == nullptr){
  cerr << " Nie udało się znaleźć obiektu!" << endl;
}
//cout << "UDALO SIE 1" << endl;
int socket;
 std::stringstream ss;


   
    Vector3D rot_vec, new_rot, result_rot;
    double phi, ksi, theta;
     obj->lockObj();
    if(AxisName == "OX"){
    double x = obj->GetAng_Roll_deg() + RotationAngle;
    obj->SetAng_Roll_deg(x);
    }
    if(AxisName == "OY"){
    double x = obj->GetAng_Pitch_deg() + RotationAngle;
    obj->SetAng_Pitch_deg(x);  
    }
    if(AxisName == "OZ"){
    double x = obj->GetAng_Yaw_deg() + RotationAngle;
    obj->SetAng_Yaw_deg(x);    
    }
    rot_vec[0] = obj->GetAng_Roll_deg();
    rot_vec[1] = obj->GetAng_Pitch_deg();
    rot_vec[2] = obj->GetAng_Yaw_deg();
    phi = rot_vec[0] * 3.14 / 180;
    ksi = rot_vec[1] * 3.14 / 180;
    theta = rot_vec[2] * 3.14 / 180;
    new_rot[0] = (cos(phi) * sin(ksi) * cos(theta) + sin(phi) * sin(theta)) * RotationAngle;
    new_rot[1] = (cos(phi) * sin(ksi) * sin(theta) - sin(phi) * cos(theta)) * RotationAngle;
    new_rot[2] = cos(phi) * cos(ksi) * RotationAngle;
    int NUM_OF_STEPS = 100;
        for (int i = 0; i < NUM_OF_STEPS; ++i)
    {
        result_rot[0] = rot_vec[0] + (new_rot[0] / 100.0) * (double)(i);
        result_rot[1] = rot_vec[1] + (new_rot[1] / 100.0) * (double)(i);
        result_rot[2] = rot_vec[2] + (new_rot[2] / 100.0) * (double)(i);

        obj->SetAng_Roll_deg(result_rot[0]);
        obj->SetAng_Pitch_deg(result_rot[1]);
        obj->SetAng_Yaw_deg(result_rot[2]);

        Vector3D pos = obj->GetPositoin_m();
        
        rComChann.LockAccess();
        std::stringstream ss;
 ss << "UpdateObj "<< "Name=" << ObjName << " Trans_m=" << "(" << pos[0] << "," << pos[1] << "," << pos[2] << ")" 
 << " RotXYZ_deg=("<< obj->GetAng_Roll_deg() << "," << obj->GetAng_Pitch_deg()  << "," << obj->GetAng_Yaw_deg()  << ")" << "\n";
 cout << ss.str().c_str() << endl;
         send(rComChann.GetSocket() ,ss.str().c_str());
 rComChann.UnlockAccess();
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000/AngularSpeed)));
    }
    obj->unlockObj();
 return true;

}


/*!
 *
 */
bool Interp4Rotate::ReadParams(std::istream& Strm_CmdsList)
{
  Strm_CmdsList >> ObjName >> AxisName >> AngularSpeed >> RotationAngle;
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Rotate::CreateCmd()
{
  return new Interp4Rotate();
}


/*!
 *
 */
void Interp4Rotate::PrintSyntax() const
{
  cout << "   Rotate nazwa_obiektu nazwa_osi szybkosc_katowa kat_obrotu" << endl;
}