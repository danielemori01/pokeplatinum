#include "roll_mechanic.h"
#include <nitro.h>
#include <string.h>
#include "pokemon.h"
#include "party.h"
#include "pc_boxes.h"
#include "savedata.h"
#include "heap.h"
#include "math_util.h"
#include "system.h"
#include "system_vars.h"
#include "unk_02054884.h"
#include "constants/items.h"
#include "generated/species.h"
#include "generated/battle_terrains.h"
#include "generated/text_banks.h"
#include "bg_window.h"
#include "field_message.h"
#include "font.h"
#include "list_menu.h"
#include "message.h"
#include "render_window.h"
#include "sound_playback.h"
#include "string_gf.h"
#include "string_template.h"
#include "sys_task.h"
#include "sys_task_manager.h"
#include "inlines.h"

#define DRAFT_POOL_SIZE 6
#define DRAFT_ROUNDS 7

typedef enum {
    DRAFT_STATE_PICK,
    DRAFT_STATE_SHOW_MESSAGE,
    DRAFT_STATE_WAIT_MESSAGE,
    DRAFT_STATE_ALL_DONE
} DraftState;

typedef struct {
    ScriptContext *ctx;
    DraftState state;
    int round;
    u16 currentPool[DRAFT_POOL_SIZE];
    u16 *selectedOptionPtr;
    SysTask *sysTask;
    Window menuWindow;
    Window messageWindow;
    ListMenu *listMenu;
    ListMenuTemplate menuTemplate;
    StringList choices[DRAFT_POOL_SIZE + 1];
    String *speciesNames[DRAFT_POOL_SIZE];
    MessageLoader *speciesNameLoader;
    int metLocation;
    u32 trainerID;
    TrainerInfo *trainerInfo;
    u8 printerID;
    u16 lastSelectedSpecies;
    BOOL done;
} DraftManager;

// Comprehensive base-form pool (Gen 1-4, excluding legendaries/mythicals)
static const u16 sFullStarterPool[] = {
    // Gen 1
    SPECIES_BULBASAUR, SPECIES_CHARMANDER, SPECIES_SQUIRTLE, SPECIES_CATERPIE, SPECIES_WEEDLE,
    SPECIES_PIDGEY, SPECIES_RATTATA, SPECIES_SPEAROW, SPECIES_EKANS, SPECIES_PIKACHU,
    SPECIES_SANDSHREW, SPECIES_NIDORAN_F, SPECIES_NIDORAN_M, SPECIES_VULPIX, SPECIES_ZUBAT,
    SPECIES_ODDISH, SPECIES_PARAS, SPECIES_VENONAT, SPECIES_DIGLETT, SPECIES_MEOWTH,
    SPECIES_PSYDUCK, SPECIES_MANKEY, SPECIES_GROWLITHE, SPECIES_POLIWAG, SPECIES_ABRA,
    SPECIES_MACHOP, SPECIES_BELLSPROUT, SPECIES_TENTACOOL, SPECIES_GEODUDE, SPECIES_PONYTA,
    SPECIES_SLOWPOKE, SPECIES_MAGNEMITE, SPECIES_FARFETCHD, SPECIES_DODUO, SPECIES_SEEL,
    SPECIES_GRIMER, SPECIES_SHELLDER, SPECIES_GASTLY, SPECIES_ONIX, SPECIES_DROWZEE,
    SPECIES_KRABBY, SPECIES_VOLTORB, SPECIES_EXEGGCUTE, SPECIES_CUBONE, SPECIES_KOFFING,
    SPECIES_RHYHORN, SPECIES_HORSEA, SPECIES_GOLDEEN, SPECIES_STARYU, SPECIES_SCYTHER,
    SPECIES_PINSIR, SPECIES_TAUROS, SPECIES_MAGIKARP, SPECIES_LAPRAS, SPECIES_DITTO,
    SPECIES_EEVEE, SPECIES_PORYGON, SPECIES_OMANYTE, SPECIES_KABUTO, SPECIES_AERODACTYL,
    SPECIES_DRATINI,
    // Gen 2
    SPECIES_CHIKORITA, SPECIES_CYNDAQUIL, SPECIES_TOTODILE, SPECIES_SENTRET, SPECIES_HOOTHOOT,
    SPECIES_LEDYBA, SPECIES_SPINARAK, SPECIES_CHINCHOU, SPECIES_PICHU, SPECIES_CLEFFA,
    SPECIES_IGGLYBUFF, SPECIES_TOGEPI, SPECIES_NATU, SPECIES_MAREEP, SPECIES_MARILL,
    SPECIES_SUDOWOODO, SPECIES_HOPPIP, SPECIES_AIPOM, SPECIES_SUNKERN, SPECIES_YANMA,
    SPECIES_WOOPER, SPECIES_MURKROW, SPECIES_MISDREAVUS, SPECIES_GIRAFARIG, SPECIES_PINECO,
    SPECIES_DUNSPARCE, SPECIES_GLIGAR, SPECIES_SNUBBULL, SPECIES_QWILFISH, SPECIES_SHUCKLE,
    SPECIES_HERACROSS, SPECIES_SNEASEL, SPECIES_TEDDIURSA, SPECIES_SLUGMA, SPECIES_SWINUB,
    SPECIES_CORSOLA, SPECIES_REMORAID, SPECIES_DELIBIRD, SPECIES_MANTINE, SPECIES_SKARMORY,
    SPECIES_HOUNDOUR, SPECIES_PHANPY, SPECIES_STANTLER, SPECIES_SMEARGLE, SPECIES_MILTANK,
    SPECIES_LARVITAR,
    // Gen 3
    SPECIES_TREECKO, SPECIES_TORCHIC, SPECIES_MUDKIP, SPECIES_POOCHYENA, SPECIES_ZIGZAGOON,
    SPECIES_WURMPLE, SPECIES_LOTAD, SPECIES_SEEDOT, SPECIES_TAILLOW, SPECIES_WINGULL,
    SPECIES_RALTS, SPECIES_SURSKIT, SPECIES_SHROOMISH, SPECIES_SLAKOTH, SPECIES_NINCADA,
    SPECIES_WHISMUR, SPECIES_MAKUHITA, SPECIES_AZURILL, SPECIES_NOSEPASS, SPECIES_SKITTY,
    SPECIES_SABLEYE, SPECIES_MAWILE, SPECIES_ARON, SPECIES_MEDITITE, SPECIES_ELECTRIKE,
    SPECIES_PLUSLE, SPECIES_MINUN, SPECIES_VOLBEAT, SPECIES_ILLUMISE, SPECIES_ROSELIA,
    SPECIES_GULPIN, SPECIES_CARVANHA, SPECIES_WAILMER, SPECIES_NUMEL, SPECIES_TORKOAL,
    SPECIES_SPOINK, SPECIES_SPINDA, SPECIES_TRAPINCH, SPECIES_CACNEA, SPECIES_SWABLU,
    SPECIES_ZANGOOSE, SPECIES_SEVIPER, SPECIES_LUNATONE, SPECIES_SOLROCK, SPECIES_BARBOACH,
    SPECIES_CORPHISH, SPECIES_BALTOY, SPECIES_LILEEP, SPECIES_ANORITH, SPECIES_FEEBAS,
    SPECIES_CASTFORM, SPECIES_KECLEON, SPECIES_SHUPPET, SPECIES_DUSKULL, SPECIES_TROPIUS,
    SPECIES_CHIMECHO, SPECIES_ABSOL, SPECIES_WYNAUT, SPECIES_SNORUNT, SPECIES_SPHEAL,
    SPECIES_CLAMPERL, SPECIES_RELICANTH, SPECIES_LUVDISC, SPECIES_BAGON, SPECIES_BELDUM,
    // Gen 4
    SPECIES_TURTWIG, SPECIES_CHIMCHAR, SPECIES_PIPLUP, SPECIES_STARLY, SPECIES_BIDOOF,
    SPECIES_KRICKETOT, SPECIES_SHINX, SPECIES_BUDEW, SPECIES_BURMY, SPECIES_COMBEE,
    SPECIES_PACHIRISU, SPECIES_BUIZEL, SPECIES_CHERUBI, SPECIES_SHELLOS, SPECIES_DRIFLOON,
    SPECIES_BUNEARY, SPECIES_GLAMEOW, SPECIES_CHINGLING, SPECIES_STUNKY, SPECIES_BRONZOR,
    SPECIES_BONSLY, SPECIES_MIME_JR, SPECIES_HAPPINY, SPECIES_CHATOT, SPECIES_SPIRITOMB,
    SPECIES_GIBLE, SPECIES_MUNCHLAX, SPECIES_RIOLU, SPECIES_HIPPOPOTAS, SPECIES_SKORUPI,
    SPECIES_CROAGUNK, SPECIES_CARNIVINE, SPECIES_FINNEON, SPECIES_MANTYKE, SPECIES_SNOVER
};

static void DraftManager_UpdateMenu(DraftManager *dm);
static void DraftManager_Delete(DraftManager *dm);
static void SysTask_DraftCallback(SysTask *task, void *data);
static BOOL DraftIsDone(ScriptContext *ctx);

static void DraftManager_GeneratePool(DraftManager *dm) {
    int poolSize = sizeof(sFullStarterPool) / sizeof(u16);
    Party *party = SaveData_GetParty(dm->ctx->fieldSystem->saveData);
    
    for (int i = 0; i < DRAFT_POOL_SIZE; i++) {
        u16 species = sFullStarterPool[LCRNG_Next() % poolSize];
        
        // Ensure no repeats in current pool or party
        BOOL repeat = FALSE;
        for (int j = 0; j < i; j++) {
            if (dm->currentPool[j] == species) {
                repeat = TRUE;
                break;
            }
        }
        if (repeat || (party && Party_HasSpecies(party, species))) {
            i--;
            continue;
        }
        
        dm->currentPool[i] = species;
    }
}

static DraftManager *DraftManager_New(ScriptContext *ctx, int metLocation) {
    DraftManager *dm = Heap_Alloc(HEAP_ID_FIELD1, sizeof(DraftManager));
    memset(dm, 0, sizeof(DraftManager));
    
    dm->ctx = ctx;
    dm->metLocation = metLocation;
    dm->round = 0;
    dm->state = DRAFT_STATE_PICK;
    dm->trainerInfo = SaveData_GetTrainerInfo(ctx->fieldSystem->saveData);
    dm->trainerID = TrainerInfo_ID(dm->trainerInfo);
    
    dm->speciesNameLoader = MessageLoader_Init(MSG_LOADER_PRELOAD_ENTIRE_BANK, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_SPECIES_NAME, HEAP_ID_FIELD1);
    
    for (int i = 0; i < DRAFT_POOL_SIZE; i++) {
        dm->speciesNames[i] = String_Init(32 * 2, HEAP_ID_FIELD1);
    }
    
    DraftManager_GeneratePool(dm);
    
    // Menu window
    Window_Add(ctx->fieldSystem->bgConfig, &dm->menuWindow, 3, 20, 1, 11, DRAFT_POOL_SIZE * 2, 13, 1);
    LoadStandardWindowGraphics(ctx->fieldSystem->bgConfig, 3, 1024 - (18 + 12) - 9, 11, STANDARD_WINDOW_SYSTEM, HEAP_ID_FIELD1);
    Window_FillTilemap(&dm->menuWindow, 15);
    Window_DrawStandardFrame(&dm->menuWindow, TRUE, 1024 - (18 + 12) - 9, 11);

    // Message window
    Window_Add(ctx->fieldSystem->bgConfig, &dm->messageWindow, 3, 2, 19, 27, 4, 13, 1 + (11 * 12));
    
    DraftManager_UpdateMenu(dm);
    
    dm->sysTask = SysTask_Start(SysTask_DraftCallback, dm, 0);
    dm->done = FALSE;

    ctx->data[0] = (u32)dm;
    ScriptContext_Pause(ctx, DraftIsDone);

    return dm;
}

static void DraftManager_UpdateMenu(DraftManager *dm) {
    if (dm->listMenu) {
        ListMenu_Free(dm->listMenu, NULL, NULL);
    }
    
    for (int i = 0; i < DRAFT_POOL_SIZE; i++) {
        MessageLoader_GetString(dm->speciesNameLoader, dm->currentPool[i], dm->speciesNames[i]);
        
        dm->choices[i].entry = (const void *)dm->speciesNames[i];
        dm->choices[i].index = i;
    }
    dm->choices[DRAFT_POOL_SIZE].entry = NULL;
    dm->choices[DRAFT_POOL_SIZE].index = MENU_CANCEL;
    
    dm->menuTemplate.choices = dm->choices;
    dm->menuTemplate.cursorCallback = NULL;
    dm->menuTemplate.printCallback = NULL;
    dm->menuTemplate.window = &dm->menuWindow;
    dm->menuTemplate.count = DRAFT_POOL_SIZE;
    dm->menuTemplate.maxDisplay = DRAFT_POOL_SIZE;
    dm->menuTemplate.headerXOffset = 1;
    dm->menuTemplate.textXOffset = 12;
    dm->menuTemplate.cursorXOffset = 2;
    dm->menuTemplate.yOffset = 1;
    dm->menuTemplate.textColorFg = 1;
    dm->menuTemplate.textColorBg = 15;
    dm->menuTemplate.textColorShadow = 2;
    dm->menuTemplate.letterSpacing = 0;
    dm->menuTemplate.lineSpacing = 0;
    dm->menuTemplate.pagerMode = PAGER_MODE_LEFT_RIGHT_PAD;
    dm->menuTemplate.fontID = FONT_SYSTEM;
    dm->menuTemplate.cursorType = 0;
    dm->menuTemplate.parent = (void *)dm;
    
    dm->listMenu = ListMenu_New(&dm->menuTemplate, 0, 0, HEAP_ID_FIELD1);
}

static void DraftManager_AddPokemon(DraftManager *dm, u16 species) {
    SaveData *saveData = dm->ctx->fieldSystem->saveData;
    Party *party = SaveData_GetParty(saveData);
    PCBoxes *pcBoxes = SaveData_GetPCBoxes(saveData);
    Pokedex *pokedex = SaveData_GetPokedex(saveData);
    
    if (dm->round == 0) {
        VarsFlags *varsFlags = SaveData_GetVarsFlags(saveData);
        if (varsFlags != NULL) {
            SystemVars_SetPlayerStarter(varsFlags, species);
        }
    }

    Pokemon *mon = Pokemon_New(HEAP_ID_FIELD2);
    Pokemon_Init(mon);
    Pokemon_InitWith(mon, species, 5, INIT_IVS_RANDOM, FALSE, 0, OTID_SET, dm->trainerID);
    Pokemon_SetCatchData(mon, dm->trainerInfo, ITEM_POKE_BALL, dm->metLocation, TERRAIN_MAX, HEAP_ID_FIELD2);
    
    if (dm->round < 6) {
        Party_AddPokemon(party, mon);
    } else {
        PCBoxes_TryStoreBoxMon(pcBoxes, (BoxPokemon *)mon);
    }
    
    Pokedex_Capture(pokedex, mon);
    Pokedex_ObtainNationalDex(pokedex);
    Heap_Free(mon);
}

static void SysTask_DraftCallback(SysTask *task, void *data) {
    DraftManager *dm = (DraftManager *)data;
    
    switch (dm->state) {
    case DRAFT_STATE_PICK:
    {
        u32 input = ListMenu_ProcessInput(dm->listMenu);
        if ((s32)input < 0) {
            // MENU_NOTHING_CHOSEN (-1): no selection yet; MENU_CANCEL (-2): ignore B press
            return;
        }

        int index = (int)input;
        if (index >= DRAFT_POOL_SIZE) {
            return;
        }
        dm->lastSelectedSpecies = dm->currentPool[index];
        Sound_PlayEffect(SEQ_SE_CONFIRM);
        
        DraftManager_AddPokemon(dm, dm->lastSelectedSpecies);
        dm->state = DRAFT_STATE_SHOW_MESSAGE;
        break;
    }
    case DRAFT_STATE_SHOW_MESSAGE:
    {
        Window_FillTilemap(&dm->messageWindow, 15);
        Window_DrawStandardFrame(&dm->messageWindow, TRUE, 1024 - (18 + 12) - 9, 11);

        String *msg = String_Init(128, HEAP_ID_FIELD1);
        StringTemplate *tempTemplate = StringTemplate_Default(HEAP_ID_FIELD1);
        StringTemplate_SetSpeciesNameWithArticleByID(tempTemplate, 0, dm->lastSelectedSpecies);
        StringTemplate_SetSpeciesNameWithArticleByID(tempTemplate, 1, dm->lastSelectedSpecies);
        
        MessageLoader *loader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, NARC_INDEX_MSGDATA__PL_MSG, TEXT_BANK_BATTLE_STRINGS, HEAP_ID_FIELD1);
        
        if (dm->round < 6) {
            // ID 867: "Gotcha! {STRVAR_1 1, 0, 0} was caught!"
            MessageLoader_GetString(loader, 867, msg);
        } else {
            // ID 869: "{STRVAR_1 1, 0, 0} was sent to someone's PC."
            MessageLoader_GetString(loader, 869, msg);
        }
        
        String *formatted = String_Init(128, HEAP_ID_FIELD1);
        StringTemplate_Format(tempTemplate, formatted, msg);
        
        dm->printerID = FieldMessage_Print(&dm->messageWindow, formatted, SaveData_GetOptions(dm->ctx->fieldSystem->saveData), TRUE);
        
        String_Free(msg);
        String_Free(formatted);
        MessageLoader_Free(loader);
        StringTemplate_Free(tempTemplate);
        dm->state = DRAFT_STATE_WAIT_MESSAGE;
        break;
    }
    case DRAFT_STATE_WAIT_MESSAGE:
        if (!FieldMessage_FinishedPrinting(dm->printerID)) {
            return;
        }
        
        if (gSystem.pressedKeys & (PAD_BUTTON_A | PAD_BUTTON_B)) {
            Window_EraseStandardFrame(&dm->messageWindow, FALSE);
            Window_FillTilemap(&dm->messageWindow, 0);
            dm->round++;
            if (dm->round < DRAFT_ROUNDS) {
                dm->state = DRAFT_STATE_PICK;
                DraftManager_GeneratePool(dm);
                DraftManager_UpdateMenu(dm);
            } else {
                dm->state = DRAFT_STATE_ALL_DONE;
            }
        }
        break;
    case DRAFT_STATE_ALL_DONE:
        DraftManager_Delete(dm);
        break;
    }
}

static void DraftManager_Delete(DraftManager *dm) {
    ListMenu_Free(dm->listMenu, NULL, NULL);
    dm->listMenu = NULL;
    Window_EraseStandardFrame(&dm->menuWindow, FALSE);
    Window_Remove(&dm->menuWindow);
    Window_Remove(&dm->messageWindow);

    for (int i = 0; i < DRAFT_POOL_SIZE; i++) {
        String_Free(dm->speciesNames[i]);
    }
    MessageLoader_Free(dm->speciesNameLoader);

    SysTask_Done(dm->sysTask);
    dm->sysTask = NULL;

    dm->done = TRUE;
    // DraftIsDone (the ScriptContext resume callback) will free dm and resume the script
}

static BOOL DraftIsDone(ScriptContext *ctx) {
    DraftManager *dm = (DraftManager *)ctx->data[0];
    if (dm == NULL || dm->done) {
        Heap_Free(dm);
        ctx->data[0] = 0;
        return TRUE;
    }
    return FALSE;
}

BOOL StarterDraft_Execute(ScriptContext *ctx, int metLocation) {
    DraftManager_New(ctx, metLocation);
    return TRUE;
}
