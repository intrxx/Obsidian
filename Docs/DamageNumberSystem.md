# Obsidian Damage Number System

<a name="table-of-contents"></a>
## Class Structure Overview

> 1. [Backend](#backend) \
> &nbsp; 1.1 [ObsidianCommonAttributeSet](#ocommonattributeset) \
> &nbsp; 1.2 [ObsidianGameplayEffectContext](#ogameplayeffectcontext) \
> &nbsp; 1.3 [ObsidianDamageExecution](#odamageexec) \
> &nbsp;&nbsp; 1.3.1 [EnemyObsidianDamageExecution](#enemyobsidiandamageexec)
> 2. [User Interface](#ui) \
> &nbsp; 2.1 [ObsidianDamageNumberWidgetComp](#odamagenumberwidgetcomp) \
> &nbsp; 2.2 [ObsidianDamageNumber](#odamagenumber)
> 3. [The Rest](#rest) \
> &nbsp; 3.1 [ObsidianPlayerController](#obsidianpc) 

<a name="backend"></a>
## 1. Backend

<a name="ocommonattributeset"></a>
### 1.1 UObsidianCommonAttributeSet

Repo Links:
- [ObsidianCommonAttributeSet.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Attributes/ObsidianCommonAttributeSet.h)
- [ObsidianCommonAttributeSet.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Attributes/ObsidianCommonAttributeSet.cpp)

<a name="ogameplayeffectcontext"></a>
### 1.2 FObsidianGameplayEffectContext

Repo Links:
- [ObsidianGameplayEffect.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/ObsidianGameplayEffect.h)
- [ObsidianGameplayEffect.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/ObsidianGameplayEffect.cpp)

<a name="odamageexec"></a>
### 1.3 UObsidianDamageExecution

Repo Links:
- [ObsidianDamageExecution.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Executions/ObsidianDamageExecution.h)
- [ObsidianDamageExecution.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Executions/ObsidianDamageExecution.cpp)

<a name="enemyobsidiandamageexec"></a>
#### 1.3.1 UEnemyObsidianDamageExecution

Repo Links:
- [EnemyObsidianDamageExecution.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Executions/EnemyObsidianDamageExecution.h)
- [EnemyObsidianDamageExecution.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Executions/EnemyObsidianDamageExecution.cpp)

<a name="ui"></a>
## 2. User Interface

<a name="odamagenumberwidgetcomp"></a>
### 2.1 UObsidianDamageNumberWidgetComp

Repo Links:
- [ObsidianDamageNumberWidgetComp.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/DamageNumbers/ObsidianDamageNumberWidgetComp.h)
- [ObsidianDamageNumberWidgetComp.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/DamageNumbers/ObsidianDamageNumberWidgetComp.cpp)

<a name="odamagenumber"></a>
### 2.2 UObsidianDamageNumber

Repo Links:
- [ObsidianDamageNumber.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/DamageNumbers/ObsidianDamageNumber.h)
- [ObsidianDamageNumber.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/DamageNumbers/ObsidianDamageNumber.cpp)

<a name="rest"></a>
## 3. The Rest

<a name="obsidianpc"></a>
### 3.1 AObsidianPlayerController

Repo Links:
- [ObsidianPlayerController.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h)
- [ObsidianPlayerController.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp)