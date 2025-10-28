#include "Pistol.h"

APistol::APistol()
{
	Damage = 25.0f;
	MaxMagazineAmmo = 15;
	CurrentAmmo = MaxMagazineAmmo;
	FireRate = 0.0f; // single-shot pistol
	WeaponType = EWeaponType::WT_Pistol;
}