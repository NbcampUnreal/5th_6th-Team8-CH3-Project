#include "Rifle.h"

ARifle::ARifle()
{
	Damage = 30.0f;
	MaxMagazineAmmo = 30;
	FireRate = 600.0f;

	WeaponType = EWeaponType::WT_Rifle; // �߰�: ������ Ÿ�� ���
}