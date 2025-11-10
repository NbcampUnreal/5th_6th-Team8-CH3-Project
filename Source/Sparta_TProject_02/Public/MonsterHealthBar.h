// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHealthBar.generated.h"

/*
* 이 클래스는 몬스터 HP UI를 제어하기 위한 인터페이스만 정의
* 실제 디자인과 구현은 블루프린트 위젯에서 담당
*/
UCLASS()
class SPARTA_TPROJECT_02_API UMonsterHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 체력바의 퍼센트(%)를 업데이트 하도록 블루프린트에 요청하는 함수
	// 'Event Update Health Bar' 라는 이벤트 노드로 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateHealthBar(float HealthRatio);

	// 받은 데미지 값을 텍스트로 표시하고 팝업 애니메이션을 실행하도록 블루프린트에 요청하는 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void PlayDamageText(float ActualDamage);
};
