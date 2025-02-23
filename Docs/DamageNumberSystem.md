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
> &nbsp; 2.2 [ObsidianDamageNumber](#odamagenumber) \
> &nbsp; 2.3 [ObsidianUITypes](#uitypes)
> 3. [The Rest](#rest) \
> &nbsp; 3.1 [ObsidianPlayerController](#obsidianpc) \
> &nbsp; 3.2 [DamageNumber CommonTextStyles](#dncommontextstyles)

<a name="backend"></a>
## 1. Backend

<a name="ocommonattributeset"></a>
### 1.1 UObsidianCommonAttributeSet

After changes to life in ```PostGameplayEffectExecute```, bools from ```FObsidianGameplayEffectContext``` are gathered and forwarded to ```ObsidianPlayerController```.

Repo Links:
- [ObsidianCommonAttributeSet.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Attributes/ObsidianCommonAttributeSet.h)
- [ObsidianCommonAttributeSet.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Attributes/ObsidianCommonAttributeSet.cpp)

<a name="ogameplayeffectcontext"></a>
### 1.2 FObsidianGameplayEffectContext

Custom ```GameplayEffectContext``` that contains bools like: ```bIsEvadedHit```, ```bIsSuppressedSpell``` or ```bIsTargetImmune``` which are set in ```ObsidianDamageExecution```.

Repo Links:
- [ObsidianGameplayEffect.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/ObsidianGameplayEffect.h)
- [ObsidianGameplayEffect.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/ObsidianGameplayEffect.cpp)

<a name="odamageexec"></a>
### 1.3 UObsidianDamageExecution

Damage Execution Calculation used in Obsidian, marks bools in ```FObsidianGameplayEffectContext``` when Evade, Block, Immune etc. occurs.

Repo Links:
- [ObsidianDamageExecution.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Executions/ObsidianDamageExecution.h)
- [ObsidianDamageExecution.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Executions/ObsidianDamageExecution.cpp)

<a name="enemyobsidiandamageexec"></a>
#### 1.3.1 UEnemyObsidianDamageExecution

Builds upon ```UObsidianDamageExecution```, adds logic for damaging Player Character. Might be removed/changed in the future.

Repo Links:
- [EnemyObsidianDamageExecution.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Executions/EnemyObsidianDamageExecution.h)
- [EnemyObsidianDamageExecution.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Executions/EnemyObsidianDamageExecution.cpp)

<a name="ui"></a>
## 2. User Interface

<a name="odamagenumberwidgetcomp"></a>
### 2.1 UObsidianDamageNumberWidgetComp

Widget Component attached to Target Actor (damaged Actor), this holds the ```UObsidianDamageNumber``` widget which is displayed.

Repo Links:
- [ObsidianDamageNumberWidgetComp.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/DamageNumbers/ObsidianDamageNumberWidgetComp.h)
- [ObsidianDamageNumberWidgetComp.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/DamageNumbers/ObsidianDamageNumberWidgetComp.cpp)

<a name="odamagenumber"></a>
### 2.2 UObsidianDamageNumber

Damage Number Widget, holds different ```UCommonTextBlockStyle```'s for managing the text based on received bools.

Repo Links:
- [ObsidianDamageNumber.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/DamageNumbers/ObsidianDamageNumber.h)
- [ObsidianDamageNumber.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/DamageNumbers/ObsidianDamageNumber.cpp)

<a name="uitypes"></a>
### 2.3 ObsidianUITypes

File that holds ```FObsidianDamageTextProps``` structure which is filled in ```UObsidianCommonAttributeSet``` and passed to ```UObsidianDamageNumber``` for initialization.

Repo Links:
- [ObsidianUITypes.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/ObsidianTypes/ObsidianUITypes.h)

<a name="rest"></a>
## 3. The Rest

<a name="obsidianpc"></a>
### 3.1 AObsidianPlayerController

Contains logic for attaching the ```UObsidianDamageNumberWidgetComp``` to the Target Actor and displaying the ```UObsidianDamageNumber``` Widget.

Repo Links:
- [ObsidianPlayerController.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h)
- [ObsidianPlayerController.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp)

<a name="dncommontextstyles"></a>
### 3.2 Damage Numbers CommonTextStyles

Bunch of CommonTextStyles used for displaying the Damage Number under different circumstances.

Repo Asset Location:
- ```/All/Game/Obsidian/UI/Styles/DamageNumbers```