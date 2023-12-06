#include <iostream>
#include "Interp4Set.hh"
#include <sstream>


using std::cout;
using std::endl;


extern "C" {
  AbstractInterp4Command* CreateCmd(void);
  const char* GetCmdName() { return "Set"; }
}




/*!
 * \brief
 *
 *
 */
AbstractInterp4Command* CreateCmd(void)
{
  return Interp4Set::CreateCmd();
}


/*!
 *
 */
Interp4Set::Interp4Set()
{}


/*!
 *
 */
void Interp4Set::PrintCmd() const
{
  /*
   *  Tu trzeba napisać odpowiednio zmodyfikować kod poniżej.
   */
  cout << GetCmdName() << " " << ObjName  << " " << x << " " << y <<" " << z <<" " << angle_OX <<" " << angle_OY <<" " << angle_OZ << endl;
  cout <<endl;
}


/*!
 *
 */
const char* Interp4Set::GetCmdName() const
{
  return ::GetCmdName();
}


/*!
 *
 */
bool Interp4Set::ExecCmd( AbstractScene      &rScn, 
                           const char         *sMobObjName,
			   AbstractComChannel &rComChann
			 )
{

//std::shared_ptr<AbstractMobileObj> obj = std::make_shared<AbstractMobileObj>();
AbstractMobileObj* obj =  rScn.FindMobileObject(ObjName);
if (obj == nullptr){
  cerr << " Nie udało się znaleźć obiektu!" << endl;
}
int socket;
 std::stringstream ss;
 Vector3D currentPositon = obj->GetPositoin_m();
 Vector3D Position;
 int roll = obj->GetAng_Roll_deg();
 int pitch = obj->GetAng_Pitch_deg();
 int yaw = obj->GetAng_Yaw_deg();
 Position[0] = x + currentPositon[0];
 Position[1] = y + currentPositon[1];
 Position[2] = z + currentPositon[2];
 obj->lockObj();
 //obj->SetName(ObjName);
 obj->SetPosition_m(Position);
 obj->SetAng_Roll_deg(angle_OX + roll);
 obj->SetAng_Pitch_deg(angle_OY + pitch);
 obj->SetAng_Yaw_deg(angle_OZ + yaw);
 obj->unlockObj();
 //rScn.scnBlockfun();
 // rScn.AddMobileObj(obj);
 //rScn.scnUnlockfun();
 // AddObj Name=Podstawa1 RotXYZ_deg=(0,-45,20) Trans_m=(-1,3,0)\n";
rComChann.LockAccess();
 ss << "AddObj "<< "Name=" << ObjName << " Trans_m=" << "(" << Position[0] << "," << Position[1] << "," << Position[2] << ")" 
 << " RotXYZ_deg=("<< angle_OX << "," << angle_OY << "," << angle_OZ << ")" << "\n";
 //const char* c = "AddObj Name=Podstawa1 RotXYZ_deg=(0,-45,20) Trans_m=(-1,3,0)\n";
 cout << ss.str().c_str() << endl;
 send(rComChann.GetSocket() ,ss.str().c_str());
 rComChann.UnlockAccess();
 return true;

}


/*!
 *
 */
bool Interp4Set::ReadParams(std::istream& Strm_CmdsList)
{
  Strm_CmdsList >> ObjName >> x >> y >> z >> angle_OX >> angle_OY >> angle_OZ;
  return true;
}


/*!
 *
 */
AbstractInterp4Command* Interp4Set::CreateCmd()
{
  return new Interp4Set();
}


/*!
 *
 */
void Interp4Set::PrintSyntax() const
{
  cout << "   Set  nazwa_obiektu  wsp_x  wsp_y wsp_z kat_OX kat_OY kat_OZ " << endl;
}
