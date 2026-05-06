# Project Design — Full Vision

> This document captures the complete long-term design for the hack.
> CLAUDE.md describes only what is actively being built (currently v1).
> When a feature graduates from "planned" to "in development", move its
> spec from this file into CLAUDE.md.

## Core Concept

A challenge ROM hack of Pokémon Platinum (US, rev 1) where:
- The player never catches Pokémon — every team member is acquired via a "roll" mechanic.
- Death is permanent; fainted Pokémon are removed from play forever.
- Combat resource management is restricted to held items only.
- The economy is reshaped around free Rare Candies and trainer money.

## Pokémon Pool (used by all rolls)

- Source: full National Dex (493 species).
- Rules: base-form only OR single-stage species. No fully-evolved forms in the pool.
- Excluded: legendaries, mythicals, shinies.
- Once acquired, normal evolution is allowed via standard gameplay.

## Roll Mechanic (universal primitive)

A "roll" presents the player with **one pool of 6 species** drawn from the Pokémon Pool.

- 2 species are banned.
- The player picks 1 of the remaining 4.
- If a Pokemon is already in the active team it can't appear in a roll.

### When a roll happens

1. **Initial draft (Planned):**
   Sequential rolls at game start, replacing the starter briefcase. Player builds their starting party from these rolls.

2. **Fast Forward / Intro Skip (Done):**
   Bypasses introductory sequence. Player spawns in Rowan's Lab. Starter selection and basic items given immediately.

3. **Post-gym roll (In Development):**
   After each of the 8 gyms, player gets one roll.

4. **In-game trade (Planned):**
   NPC trades are replaced by rolls.

5. **Forced gifts and event captures (Planned):**
   Replaced by rolls.

## Permadeath (Done)

- When a Pokémon's HP reaches 0 in battle, it is marked as permanently dead.
- Dead Pokémon are moved to a hidden "locked" PC box (Box 18) after the battle ends.
- `Party_HealAllMembers` (and similar effects) skip dead Pokémon.
- Revive items are either removed or made non-functional for the player.
- **Implemented in:** `src/pokemon.c`, `src/battle/battle_script.c`, `src/encounter.c`, `src/item_use_pokemon.c`.

## Catching & Wild Encounters (Done)

- Poké Balls (all variants) are removed from the game entirely. Not in shops, not as field items, not from NPCs.
- Wild battles still occur normally for XP.

## Combat Bag Restriction (Done)

- The player's bag menu is disabled inside battles.
- Held items continue to function normally.

## Economy (Done)

- Every Poké Mart sells Rare Candies for 0 ¥.
- Rare Candies are unsellable.

## HM Overhaul (Planned)

- HMs are replaced with Key Items (e.g., "Machete" for Cut).
- Field obstacles check for Key Items instead of moves.

## Open Questions / Decide Before Implementing

- **Roll level:** What level does a rolled Pokémon arrive at?
- **Party size > 6:** How to handle the initial 7-mon draft given the 6-mon party limit?
- **HM moves themselves:** Repurpose or remove?

## Known Bugs / Future Fixes

- **Player House Cutscene (Fixed):** Entering the house in Twinleaf Town triggered an out-of-bounds movement script. Resolved by adjusting `VAR_PLAYER_HOUSE_STATE` in the lab script.

## Versioning Plan

- **v1 (Done):** Permadeath + Fast Forward / Intro Skip + Poké Ball removal + 7-mon Initial Draft.
- **v2 (Done):** Post-gym rolls + Combat Bag Disable + free Rare Candies (un-sellable).
- **v3:** HM Overhaul + event/gift replacement.
- **v4:** Visible Memorial UI, stat-based ban algorithm, balance pass.
an algorithm, balance pass.
