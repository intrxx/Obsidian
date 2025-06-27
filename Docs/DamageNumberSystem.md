# Obsidian Damage Number System

[//]: ![DamageNumberSystemVid](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/DamageNumberSystem/DamageNumberSystem.mp4)

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
### 1.1 UObsidianCommonAttributeSet ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Attributes/ObsidianCommonAttributeSet.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Attributes/ObsidianCommonAttributeSet.cpp))

![AttributeSet](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/DamageNumberSystem/DamageNumberSystem_AttributeSet.jpg)

After changes to life in ```PostGameplayEffectExecute```, bools from ```FObsidianGameplayEffectContext``` are gathered and forwarded to ```ObsidianPlayerController```.

<a name="ogameplayeffectcontext"></a>
### 1.2 FObsidianGameplayEffectContext ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/ObsidianGameplayEffect.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/ObsidianGameplayEffect.cpp))

Custom ```GameplayEffectContext``` that contains bools like: ```bIsEvadedHit```, ```bIsSuppressedSpell``` or ```bIsTargetImmune``` which are set in ```ObsidianDamageExecution```.

<a name="odamageexec"></a>
### 1.3 UObsidianDamageExecution ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Executions/ObsidianDamageExecution.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Executions/ObsidianDamageExecution.cpp))

Damage Execution Calculation used in Obsidian, marks bools in ```FObsidianGameplayEffectContext``` when Evade, Block, Immune etc. occurs.

<a name="enemyobsidiandamageexec"></a>
#### 1.3.1 UEnemyObsidianDamageExecution ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/Executions/EnemyObsidianDamageExecution.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/Executions/EnemyObsidianDamageExecution.cpp))

Builds upon ```UObsidianDamageExecution```, adds logic for damaging Player Character. Might be removed/changed in the future.

<a name="ui"></a>
## 2. User Interface

<a name="odamagenumberwidgetcomp"></a>
### 2.1 UObsidianDamageNumberWidgetComp ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/DamageNumbers/ObsidianDamageNumberWidgetComp.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/DamageNumbers/ObsidianDamageNumberWidgetComp.cpp))

Widget Component attached to Target Actor (damaged Actor), this holds the ```UObsidianDamageNumber``` widget which is displayed.

<a name="odamagenumber"></a>
### 2.2 UObsidianDamageNumber ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/DamageNumbers/ObsidianDamageNumber.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/DamageNumbers/ObsidianDamageNumber.cpp))

[//]: ![Crit](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/DamageNumberSystem/DamageNumberSyetem_Crit.mp4)

Damage Number Widget, holds different ```UCommonTextBlockStyle```'s for managing the text based on received bools.

<a name="uitypes"></a>
### 2.3 ObsidianUITypes ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/ObsidianTypes/ObsidianUITypes.h))

File that holds ```FObsidianDamageTextProps``` structure which is filled in ```UObsidianCommonAttributeSet``` and passed to ```UObsidianDamageNumber``` for initialization.

<a name="rest"></a>
## 3. The Rest

<a name="obsidianpc"></a>
### 3.1 AObsidianPlayerController ([h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/Characters/Player/ObsidianPlayerController.h) | [cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/Characters/Player/ObsidianPlayerController.cpp))

Contains logic for attaching the ```UObsidianDamageNumberWidgetComp``` to the Target Actor and displaying the ```UObsidianDamageNumber``` Widget.

<a name="dncommontextstyles"></a>
### 3.2 Damage Numbers CommonTextStyles

![Sttyles](https://github.com/intrxx/Obsidian/blob/main/Docs/Images/DamageNumberSystem/DamageNumberSystem_Styles.jpg)

Bunch of CommonTextStyles used for displaying the Damage Number under different circumstances.

Repo Assets Location:
- ```/All/Game/Obsidian/UI/Styles/DamageNumbers```

