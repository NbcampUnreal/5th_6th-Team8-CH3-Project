#include "Rifle.h"

ARifle::ARifle()
{
	// 라이플에 맞는 스탯으로 조정합니다 (예시 값)
	Damage = 30.0f;
	MaxMagazineAmmo = 30;
	FireRate = 600.0f; // 분당 600발 (자동 연사)
}