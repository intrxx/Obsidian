# Game Effects Info System

<a name="table-of-contents"></a>
## Class Structure Overview

> 1. [Backend](#backend) \
> &nbsp; 1.1 [ObsidianAbilitySystemComponent](#abilitysystemcomp) \
> &nbsp; 1.2 [ObsidianEnemyOverlayBarComponent](#enemyoverlaybarcomp)
> 2. [User Interface](#ui) \
> &nbsp; 2.1 [MainOverlayWidgetController](#mainoverlaywidgetcontroller) \
> &nbsp; 2.2 [ObsidianUITypes](#uitypes) \
> &nbsp; 2.3 [ObsidianMainOverlay](#mainoverlay) \
> &nbsp;&nbsp; 2.3.1 [ObsidianEffectInfoBase](#effectinfobase) \
> &nbsp;&nbsp; 2.3.2 [ObsidianDurationalEffectInfo](#durrationaleffectinfo) \
> &nbsp;&nbsp; 2.3.3 [OStackingDurationalEffectInfo](#stackingdurationaleffectinfo) \
> &nbsp;&nbsp; 2.3.4 [ObsidianEffectDescription](#effectdesc) \
> &nbsp; 2.4 [ObsidianProgressGlobeBase](#progressglobebase) \
> &nbsp;&nbsp; 2.4.1 [ObsidianProgressGlobe_Health](#progressglobehealth) \
> &nbsp; 2.5 [ObsidianOverlayEnemyBar](#overlayenemybar) \
> &nbsp; 2.6 [ObsidianOverlayBossEnemyBar](#overlaybossenemybar)
> 3. [The Rest](#rest) \
> &nbsp; 3.1 [ObsidianGameplayTags](#gameplaytags) 

<a name="backend"></a>
## 1. Backend

<a name="abilitysystemcomp"></a>
### 1.1 UObsidianAbilitySystemComponent

Handles broadcasting data about any effects with proper GameplayTag setup.

```c++
OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &ThisClass::ClientOnEffectApplied);
```

Repo Links:
- [ObsidianAbilitySystemComponent.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/AbilitySystem/ObsidianAbilitySystemComponent.h)
- [ObsidianAbilitySystemComponent.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/AbilitySystem/ObsidianAbilitySystemComponent.cpp)

<a name="enemyoverlaybarcomp"></a>
### 1.2 UObsidianEnemyOverlayBarComponent

Receives and handles the data provided by ASC, bradcasting futher details to Enemy related UI.

Repo Links:
- [ObsidianEnemyOverlayBarComponent.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/CharacterComponents/ObsidianEnemyOverlayBarComponent.h)
- [ObsidianEnemyOverlayBarComponent.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/CharacterComponents/ObsidianEnemyOverlayBarComponent.cpp)

<a name="ui"></a>
## 2. User Interface

<a name="mainoverlaywidgetcontroller"></a>
### 2.1 UObsidianHeroWidgetControllerBase

Receives and handles the data provided by ASC, bradcasting futher details to Player related UI.

Repo Links:
- [MainOverlayWidgetController.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/WidgetControllers/MainOverlayWidgetController.h)
- [MainOverlayWidgetController.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/WidgetControllers/MainOverlayWidgetController.cpp)

<a name="uitypes"></a>
### 2.2 ObsidianUITypes

File that holds ```FObsidianEffectUIDataWidgetRow``` structure which is Table Row definition, this way designer can fill the details about specific effect which is then matched with its GameplayTag. I think about switching to DataAsset instead.

Repo Links:
- [ObsidianUITypes.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/ObsidianTypes/ObsidianUITypes.h)

<a name="mainoverlay"></a>
### 2.3 UObsidianMainOverlay

Center of UI that receives the data from ``MainOverlayWidgetController`` to handle it and spawn corresponding Effect Info widget or broadcast style changes to the ```ProgressGlobe```.

Repo Links:
- [ObsidianMainOverlay.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/ObsidianMainOverlay.h)
- [ObsidianMainOverlay.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/ObsidianMainOverlay.cpp)

<a name="effectinfobase"></a>
#### 2.3.1 UObsidianEffectInfoBase 

Spawned and added to Player's screen in ```UObsidianMainOverlay``` for basic effects.

Repo Links:
- [ObsidianEffectInfoBase.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/Subwidgets/ObsidianEffectInfoBase.h)
- [ObsidianEffectInfoBase.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/Subwidgets/ObsidianEffectInfoBase.cpp)

<a name="durrationaleffectinfo"></a>
#### 2.3.2 UObsidianDurationalEffectInfo 

Spawned and added to Player's screen in ```UObsidianMainOverlay``` for durational effects.

Repo Links:
- [ObsidianDurationalEffectInfo.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.h)
- [ObsidianDurationalEffectInfo.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/Subwidgets/ObsidianDurationalEffectInfo.cpp)

<a name="stackingdurationaleffectinfo"></a>
#### 2.3.3 UOStackingDurationalEffectInfo

Spawned and added to Player's screen in ```UObsidianMainOverlay``` for stacking durational effects.

Repo Links:
- [OStackingDurationalEffectInfo.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.h)
- [OStackingDurationalEffectInfo.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/Subwidgets/OStackingDurationalEffectInfo.cpp)

<a name="effectdesc"></a>
#### 2.3.4 UObsidianEffectDescription

Effect Description Widget spawned after howering over the Effect on the Player's screen.

Repo Links:
- [ObsidianEffectDescription.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/MainOverlay/Subwidgets/ObsidianEffectDescription.h)
- [ObsidianEffectDescription.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/MainOverlay/Subwidgets/ObsidianEffectDescription.cpp)

<a name="progressglobebase"></a>
### 2.4 UObsidianProgressGlobeBase

Base class for Progress Globes in Obsidian.

Repo Links:
- [ObsidianProgressGlobeBase.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.h)
- [ObsidianProgressGlobeBase.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobeBase.cpp)

<a name="progressglobehealth"></a>
#### 2.4.1 UObsidianProgressGlobe_Health

Player Health Globe, effects like poison, shock or chill are displayed on it.

Repo Links:
- [ObsidianProgressGlobe_Health.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.h)
- [ObsidianProgressGlobe_Health.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/ProgressBars/ProgressGlobe/ObsidianProgressGlobe_Health.cpp)

<a name="overlayenemybar"></a>
### 2.5 UObsidianOverlayEnemyBar

Regular Enemy Bar displayed on Player's Overlay, effects like poison, shock or chill are displayed on it.

Repo Links:
- [ObsidianOverlayEnemyBar.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/ProgressBars/UObsidianOverlayEnemyBar.h)
- [ObsidianOverlayEnemyBar.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/ProgressBars/UObsidianOverlayEnemyBar.cpp)

<a name="overlaybossenemybar"></a>
### 2.6 UObsidianOverlayBossEnemyBar

Boss Enemy Bar displayed on Player's Overlay, effects like poison, shock or chill are displayed on it.

Repo Links:
- [ObsidianOverlayBossEnemyBar.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Public/UI/ProgressBars/ObsidianOverlayBossEnemyBar.h)
- [ObsidianOverlayBossEnemyBar.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/Private/UI/ProgressBars/ObsidianOverlayBossEnemyBar.cpp)

<a name="rest"></a>
## 3. The Rest

<a name="overlaybossenemybar"></a>
### 3.1 ObsidianGameplayTags

Namespace containig all GameplayTags used in Obsidian, contains ```UI.EffectData``` tags that represent specific Effect that wish to be displayed on the UI. In order to be considered for broadcasting by ASC the effect needs to have ```UI.DataSpecifierTag``` tag.

Repo Links:
- [ObsidianGameplayTags.h](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/ObsidianGameplayTags.h)
- [ObsidianGameplayTags.cpp](https://github.com/intrxx/Obsidian/blob/main/Source/Obsidian/ObsidianGameplayTags.cpp)