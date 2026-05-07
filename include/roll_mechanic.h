#ifndef POKEPLATINUM_ROLL_MECHANIC_H
#define POKEPLATINUM_ROLL_MECHANIC_H

#include "field_script_context.h"
#include "savedata.h"

BOOL StarterDraft_Execute(ScriptContext *ctx, int metLocation);
BOOL GymRoll_Execute(ScriptContext *ctx, int metLocation, int targetSlot, int level);

#endif // POKEPLATINUM_ROLL_MECHANIC_H
