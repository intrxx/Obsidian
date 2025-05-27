#pragma once

// ~ Core
#include "DrawDebugHelpers.h"

// ~ Project

/** Prints a simple debug message to the screen. */
#define OB_PRINT(DebugMessage, ...) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString::Printf(TEXT(DebugMessage), __VA_ARGS__)); }

/**
 * Creates a debug sphere in the Center location.
 * Note: For tick use DEBUG_SPHERE_TICK.
 */
#define DEBUG_SPHERE(Center) if (GetWorld()) DrawDebugSphere(GetWorld(), Center, 20.0f, 16, FColor::Green, true);

/**
 * Creates a debug sphere in the Center location without persistent lines.
 */
#define DEBUG_SPHERE_TICK(Center) if (GetWorld()) DrawDebugSphere(GetWorld(), Center, 20.0f, 16, FColor::Blue, false);

/**
 * Creates a debug line from LineStart to LineEnd.
 * Note: For tick use DEBUG_LINE_TICK.
 */
#define DEBUG_LINE(LineStart, LineEnd) if (GetWorld()) DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Green, true)

/**
 * Creates a debug line from LineStart to LineEnd without persistent lines.
 */
#define DEBUG_LINE_TICK(LineStart, LineEnd) if (GetWorld()) DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Blue, false)