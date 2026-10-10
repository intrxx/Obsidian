// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project

/**
 * Every Log Category of the project, named ObLog[SystemName].
 *
 * All of them compile up to All and show Log and above by default, the Verbose and VeryVerbose flow logs
 * can be enabled per Category with the "Log ObLog[SystemName] Verbose" console command, the -LogCmds="ObLog[SystemName] Verbose"
 * command line argument or in the [Core.Log] section of DefaultEngine.ini.
 */

/**
 * ---- General ----
 */

OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogGeneral, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogDebugMenu, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogSaveSystem, Log, All);

/**
 * ---- Gameplay ----
 */

OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogAbilitySystem, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogAttributes, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogDamage, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogCombat, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogAI, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogCharacter, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogInput, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogInteraction, Log, All);

/**
 * ---- Items ----
 */

OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogItems, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogItemContainer, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogItemManager, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogInventory, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogEquipment, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogPlayerStash, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogItemDrop, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogItemData, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogItemLabels, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogAffixes, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogCrafting, Log, All);

/**
 * ---- User Interface ----
 */

OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogUI, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogUIItems, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogUIMainOverlay, Log, All);
OBSIDIAN_API DECLARE_LOG_CATEGORY_EXTERN(ObLogUICharacterStatus, Log, All);
