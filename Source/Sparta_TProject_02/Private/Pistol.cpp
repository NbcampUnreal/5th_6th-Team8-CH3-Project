#include "Pistol.h"

APistol::APistol()
{
	Damage = 25.0f;
	MaxMagazineAmmo = 15;
	CurrentAmmo = MaxMagazineAmmo;
	FireRate = 0.0f;
	WeaponType = EWeaponType::WT_Pistol;
}