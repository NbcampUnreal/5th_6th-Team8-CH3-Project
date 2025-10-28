#include "Rifle.h"

ARifle::ARifle()
{
	Damage = 30.0f;
	MaxMagazineAmmo = 30;
	CurrentAmmo = MaxMagazineAmmo;
	FireRate = 600.0f;
	WeaponType = EWeaponType::WT_Rifle;
}