#include <iostream>
#include "Interp4Move.hh"
#include <sstream>
#include <thread>


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Move"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Move::CreateCmd();
}


/*!
 *
 */
Interp4Move::Interp4Move()
{}


/*!
 *
 */
void Interp4Move::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() <<" " << ObjName << " "<< vel <<" "<< dist <<endl;
  cout <<endl;
}


/*!
 *
 */
const char* Interp4Move::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Move::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{
  std::stringstream ss;
AbstractMobileObj* obj =  rScn.FindMobileObject(ObjName);
if (obj == nullptr){
  cerr << " Nie udało się znaleźć obiektu!" << endl;
}
 obj->lockObj();
 Vector3D currPose = obj->GetPositoin_m();
Vector3D trans_vec;
    double rot_vec[3] = {obj->GetAng_Roll_deg() * 3.14 / 180, obj->GetAng_Pitch_deg() * 3.14 / 180, obj->GetAng_Yaw_deg() * 3.14 / 180};
    trans_vec[0] = (cos(rot_vec[0]) * sin(rot_vec[1]) * cos(rot_vec[2]) + sin(rot_vec[0]) * sin(rot_vec[2])) * dist;
    trans_vec[1] = (cos(rot_vec[0]) * sin(rot_vec[1]) * sin(rot_vec[2]) - sin(rot_vec[0]) * cos(rot_vec[2])) * dist;
    trans_vec[2] = cos(rot_vec[0]) * cos(rot_vec[1]) * dist;
    Vector3D newPose;

    for (int i = 0; i < 100; ++i)
    {
      std::stringstream ss;
        newPose[0] = currPose[0] + (trans_vec[0] / 100.0) * (double)(i);
        newPose[1] = currPose[1] + (trans_vec[1] / 100.0) * (double)(i);
        newPose[2] = currPose[2] + (trans_vec[2] / 100.0) * (double)(i);
        // obj->lockObj();
        obj->SetPosition_m(newPose);
        // obj->unlockObj();
        Vector3D pos = obj->GetPositoin_m();
        rComChann.LockAccess();
 ss << "UpdateObj "<< "Name=" << ObjName << " Trans_m=" << "(" << pos[0] << "," << pos[1] << "," << pos[2] << ")" 
 << " RotXYZ_deg=("<< obj->GetAng_Roll_deg() << "," << obj->GetAng_Pitch_deg()  << "," << obj->GetAng_Yaw_deg()  << ")" << "\n";
 cout << ss.str().c_str() << endl;
         send(rComChann.GetSocket() ,ss.str().c_str());
        rComChann.UnlockAccess();
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 / vel)));
    }

  return true;
}


/*!
 *
 */
bool Interp4Move::ReadParams(std::istream& Strm_CmdsList)
{
  Strm_CmdsList >> ObjName >> vel >> dist;
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Move::CreateCmd()
{
  return new Interp4Move();
}


/*!
 *
 */
void Interp4Move::PrintSyntax() const
{
  cout << "   Move  NazwaObiektu  Szybkosc[m/s]  DlugoscDrogi[m]" << endl;
}
