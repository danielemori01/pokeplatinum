#include "location.h"

#include "generated/map_headers.h"

#include "field_overworld_state.h"
#include "savedata.h"

static const Location sPlayerStartLocation = {
    .mapId = MAP_HEADER_SANDGEM_TOWN_POKEMON_RESEARCH_LAB,
    .warpId = WARP_ID_NONE,
    .x = 7,
    .z = 15,
    .faceDirection = FACE_UP,
};

static const Location sPlayerFirstRespawnLocation = {
    .mapId = MAP_HEADER_TWINLEAF_TOWN,
    .warpId = WARP_ID_NONE,
    .x = 116,
    .z = 886,
    .faceDirection = FACE_DOWN,
};

void SetPlayerStartLocation(Location *outLocation)
{
    *outLocation = sPlayerStartLocation;
}

void SetPlayerFirstRespawnLocation(Location *outLocation)
{
    *outLocation = sPlayerFirstRespawnLocation;
}

void InitPlayerStartLocation(SaveData *saveData)
{
    SetPlayerStartLocation(FieldOverworldState_GetPlayerLocation(SaveData_GetFieldOverworldState(saveData)));
}
