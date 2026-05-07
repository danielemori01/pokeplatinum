#include "scrcmd_item.h"

#include <nitro.h>
#include <string.h>

#include "constants/heap.h"
#include "constants/items.h"

#include "field/field_system.h"

#include "bag.h"
#include "field_script_context.h"
#include "inlines.h"
#include "item.h"
#include "unk_0205DFC4.h"

BOOL ScrCmd_AddItem(ScriptContext *ctx)
{
    FieldSystem *fieldSystem = ctx->fieldSystem;
    u16 item = ScriptContext_GetVar(ctx);
    u16 count = ScriptContext_GetVar(ctx);
    u16 *destVar = ScriptContext_GetVarPointer(ctx);

    Bag *bag = SaveData_GetBag(fieldSystem->saveData);
    *destVar = Bag_TryAddItem(bag, item, count, HEAP_ID_FIELD1);

    if (item >= ITEM_HM01 && item <= ITEM_HM08) {
        Bag_TryAddItem(bag, ITEM_TM93 + (item - ITEM_HM01), 1, HEAP_ID_FIELD1);
    }

    return FALSE;
}

BOOL ScrCmd_RemoveItem(ScriptContext *ctx)
{
    FieldSystem *fieldSystem = ctx->fieldSystem;
    u16 item = ScriptContext_GetVar(ctx);
    u16 count = ScriptContext_GetVar(ctx);
    u16 *destVar = ScriptContext_GetVarPointer(ctx);

    *destVar = Bag_TryRemoveItem(SaveData_GetBag(fieldSystem->saveData), item, count, HEAP_ID_FIELD1);
    return FALSE;
}

BOOL ScrCmd_CanFitItem(ScriptContext *ctx)
{
    FieldSystem *fieldSystem = ctx->fieldSystem;
    u16 item = ScriptContext_GetVar(ctx);
    u16 count = ScriptContext_GetVar(ctx);
    u16 *destVar = ScriptContext_GetVarPointer(ctx);

    *destVar = Bag_CanFitItem(SaveData_GetBag(fieldSystem->saveData), item, count, HEAP_ID_FIELD1);
    return FALSE;
}

BOOL ScrCmd_CheckItem(ScriptContext *ctx)
{
    FieldSystem *fieldSystem = ctx->fieldSystem;
    u16 item = ScriptContext_GetVar(ctx);
    u16 count = ScriptContext_GetVar(ctx);
    u16 *destVar = ScriptContext_GetVarPointer(ctx);

    *destVar = Bag_CanRemoveItem(SaveData_GetBag(fieldSystem->saveData), item, count, HEAP_ID_FIELD2);
    return FALSE;
}

BOOL ScrCmd_GetItemQuantity(ScriptContext *ctx)
{
    FieldSystem *fieldSystem = ctx->fieldSystem;
    u16 item = ScriptContext_GetVar(ctx);
    u16 *destVar = ScriptContext_GetVarPointer(ctx);

    *destVar = Bag_GetItemQuantity(SaveData_GetBag(fieldSystem->saveData), item, HEAP_ID_FIELD2);
    return FALSE;
}

BOOL ScrCmd_IsItemTMHM(ScriptContext *ctx)
{
    FieldSystem *fieldSystem = ctx->fieldSystem;
    u16 item = ScriptContext_GetVar(ctx);
    u16 *destVar = ScriptContext_GetVarPointer(ctx);

    *destVar = Item_IsTMHM(item);
    return FALSE;
}

BOOL ScrCmd_GetItemPocket(ScriptContext *ctx)
{
    FieldSystem *fieldSystem = ctx->fieldSystem;
    u16 item = ScriptContext_GetVar(ctx);
    u16 *pocket = ScriptContext_GetVarPointer(ctx);

    *pocket = (u16)Item_LoadParam(item, ITEM_PARAM_FIELD_POCKET, HEAP_ID_FIELD2);
    return FALSE;
}

BOOL ScrCmd_Dummy081(ScriptContext *ctx)
{
    return FALSE;
}

BOOL ScrCmd_Dummy082(ScriptContext *ctx)
{
    return FALSE;
}
