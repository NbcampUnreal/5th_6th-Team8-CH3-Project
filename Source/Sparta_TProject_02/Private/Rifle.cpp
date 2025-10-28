#include "Rifle.h"

ARifle::ARifle()
{
	Damage = 30.0f;
	MaxMagazineAmmo = 30;
	FireRate = 600.0f;

	WeaponType = EWeaponType::WT_Rifle; // 추가: 라이플 타입 명시
}