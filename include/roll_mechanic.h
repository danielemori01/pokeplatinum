#ifndef POKEPLATINUM_ROLL_MECHANIC_H
#define POKEPLATINUM_ROLL_MECHANIC_H

#include "field_script_context.h"
#include "savedata.h"
#include "trainer_info.h"

BOOL StarterDraft_Execute(ScriptContext *ctx, int metLocation);
BOOL GymRoll_Execute(ScriptContext *ctx, int metLocation, int targetSlot, int level);
u8 LevelCap_Get(const TrainerInfo *info);

#endif // POKEPLATINUM_ROLL_MECHANIC_H
