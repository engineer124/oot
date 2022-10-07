#ifndef SEQUENCE_H
#define SEQUENCE_H

typedef enum {
    /* 0x00 */ SEQ_ID_GENERAL_SFX,      // General Sound Effects
    /* 0x01 */ SEQ_ID_AMBIENCE,         // Environmental nature background sounds
    /* 0x02 */ SEQ_ID_FIELD_LOGIC,      // Hyrule Field
    /* 0x03 */ SEQ_ID_FIELD_INIT,       // Hyrule Field	Initial Segment From Loading Area
    /* 0x04 */ SEQ_ID_FIELD_DEFAULT_1,  // Hyrule Field	Moving Segment 1
    /* 0x05 */ SEQ_ID_FIELD_DEFAULT_2,  // Hyrule Field	Moving Segment 2
    /* 0x06 */ SEQ_ID_FIELD_DEFAULT_3,  // Hyrule Field	Moving Segment 3
    /* 0x07 */ SEQ_ID_FIELD_DEFAULT_4,  // Hyrule Field	Moving Segment 4
    /* 0x08 */ SEQ_ID_FIELD_DEFAULT_5,  // Hyrule Field	Moving Segment 5
    /* 0x09 */ SEQ_ID_FIELD_DEFAULT_6,  // Hyrule Field	Moving Segment 6
    /* 0x0A */ SEQ_ID_FIELD_DEFAULT_7,  // Hyrule Field	Moving Segment 7
    /* 0x0B */ SEQ_ID_FIELD_DEFAULT_8,  // Hyrule Field	Moving Segment 8
    /* 0x0C */ SEQ_ID_FIELD_DEFAULT_9,  // Hyrule Field	Moving Segment 9
    /* 0x0D */ SEQ_ID_FIELD_DEFAULT_A,  // Hyrule Field	Moving Segment 10
    /* 0x0E */ SEQ_ID_FIELD_DEFAULT_B,  // Hyrule Field	Moving Segment 11
    /* 0x0F */ SEQ_ID_FIELD_ENEMY_INIT, // Hyrule Field	Enemy Approaches
    /* 0x10 */ SEQ_ID_FIELD_ENEMY_1,    // Hyrule Field	Enemy Near Segment 1
    /* 0x11 */ SEQ_ID_FIELD_ENEMY_2,    // Hyrule Field	Enemy Near Segment 2
    /* 0x12 */ SEQ_ID_FIELD_ENEMY_3,    // Hyrule Field	Enemy Near Segment 3
    /* 0x13 */ SEQ_ID_FIELD_ENEMY_4,    // Hyrule Field	Enemy Near Segment 4
    /* 0x14 */ SEQ_ID_FIELD_STILL_1,    // Hyrule Field	Standing Still Segment 1
    /* 0x15 */ SEQ_ID_FIELD_STILL_2,    // Hyrule Field	Standing Still Segment 2
    /* 0x16 */ SEQ_ID_FIELD_STILL_3,    // Hyrule Field	Standing Still Segment 3
    /* 0x17 */ SEQ_ID_FIELD_STILL_4,    // Hyrule Field	Standing Still Segment 4
    /* 0x18 */ SEQ_ID_DUNGEON,          // Dodongo's Cavern
    /* 0x19 */ SEQ_ID_KAKARIKO_ADULT,   // Kakariko Village (Adult)
    /* 0x1A */ SEQ_ID_ENEMY,            // Battle
    /* 0x1B */ SEQ_ID_BOSS,             // Boss Battle "SEQ_ID_BOSS00"
    /* 0x1C */ SEQ_ID_INSIDE_DEKU_TREE, // Inside the Deku Tree "SEQ_ID_FAIRY_DUNGEON"
    /* 0x1D */ SEQ_ID_MARKET,           // Market
    /* 0x1E */ SEQ_ID_TITLE,            // Title Theme
    /* 0x1F */ SEQ_ID_LINK_HOUSE,       // House
    /* 0x20 */ SEQ_ID_GAME_OVER,        // Game Over
    /* 0x21 */ SEQ_ID_BOSS_CLEAR,       // Boss Clear
    /* 0x22 */ SEQ_ID_ITEM_GET,         // Obtain Item
    /* 0x23 */ SEQ_ID_OPENING_GANON,    // Enter Ganondorf
    /* 0x24 */ SEQ_ID_HEART_GET,        // Obtain Heart Container
    /* 0x25 */ SEQ_ID_OCA_LIGHT,        // Prelude of Light
    /* 0x26 */ SEQ_ID_JABU_JABU,        // Inside Jabu-Jabu's Belly "SEQ_ID_BUYO_DUNGEON"
    /* 0x27 */ SEQ_ID_KAKARIKO_KID,     // Kakariko Village (Child)
    /* 0x28 */ SEQ_ID_GREAT_FAIRY,      // Great Fairy's Fountain "SEQ_ID_GODESS"
    /* 0x29 */ SEQ_ID_ZELDA_THEME,      // Zelda's Theme "SEQ_ID_HIME"
    /* 0x2A */ SEQ_ID_FIRE_TEMPLE,      // Fire Temple "SEQ_ID_FIRE_DUNGEON"
    /* 0x2B */ SEQ_ID_OPEN_TRE_BOX,     // Open Treasure Chest
    /* 0x2C */ SEQ_ID_FOREST_TEMPLE,    // Forest Temple "SEQ_ID_FORST_DUNGEON"
    /* 0x2D */ SEQ_ID_COURTYARD,        // Hyrule Castle Courtyard "SEQ_ID_HIRAL_GARDEN"
    /* 0x2E */ SEQ_ID_GANON_TOWER,      // Ganondorf's Theme
    /* 0x2F */ SEQ_ID_LONLON,           // Lon Lon Ranch "SEQ_ID_RONRON"
    /* 0x30 */ SEQ_ID_GORON_CITY,       // Goron City "SEQ_ID_GORON"
    /* 0x31 */ SEQ_ID_FIELD_MORNING,    // Hyrule Field Morning Theme
    /* 0x32 */ SEQ_ID_SPIRITUAL_STONE,  // Spiritual Stone Get "SEQ_ID_SPIRIT_STONE"
    /* 0x33 */ SEQ_ID_OCA_BOLERO,       // Bolero of Fire "SEQ_ID_OCA_FLAME"
    /* 0x34 */ SEQ_ID_OCA_MINUET,       // Minuet of Forest "SEQ_ID_OCA_WIND"
    /* 0x35 */ SEQ_ID_OCA_SERENADE,     // Serenade of Water "SEQ_ID_OCA_WATER"
    /* 0x36 */ SEQ_ID_OCA_REQUIEM,      // Requiem of Spirit "SEQ_ID_OCA_SOUL"
    /* 0x37 */ SEQ_ID_OCA_NOCTURNE,     // Nocturne of Shadow "SEQ_ID_OCA_DARKNESS"
    /* 0x38 */ SEQ_ID_MINI_BOSS,        // Mini-Boss Battle "SEQ_ID_MIDDLE_BOSS"
    /* 0x39 */ SEQ_ID_SMALL_ITEM_GET,   // Obtain Small Item "SEQ_ID_S_ITEM_GET"
    /* 0x3A */ SEQ_ID_TEMPLE_OF_TIME,   // Temple of Time "SEQ_ID_SHRINE_OF_TIME"
    /* 0x3B */ SEQ_ID_EVENT_CLEAR,      // Escape from Lon Lon Ranch
    /* 0x3C */ SEQ_ID_KOKIRI,           // Kokiri Forest
    /* 0x3D */ SEQ_ID_OCA_FAIRY_GET,    // Obtain Fairy Ocarina "SEQ_ID_OCA_YOUSEI"
    /* 0x3E */ SEQ_ID_SARIA_THEME,      // Lost Woods "SEQ_ID_MAYOIMORI"
    /* 0x3F */ SEQ_ID_SPIRIT_TEMPLE,    // Spirit Temple "SEQ_ID_SOUL_DUNGEON"
    /* 0x40 */ SEQ_ID_HORSE,            // Horse Race
    /* 0x41 */ SEQ_ID_HORSE_GOAL,       // Horse Race Goal
    /* 0x42 */ SEQ_ID_INGO,             // Ingo's Theme
    /* 0x43 */ SEQ_ID_MEDALLION_GET,    // Obtain Medallion "SEQ_ID_MEDAL_GET"
    /* 0x44 */ SEQ_ID_OCA_SARIA,        // Ocarina Saria's Song
    /* 0x45 */ SEQ_ID_OCA_EPONA,        // Ocarina Epona's Song
    /* 0x46 */ SEQ_ID_OCA_ZELDA,        // Ocarina Zelda's Lullaby
    /* 0x47 */ SEQ_ID_OCA_SUNS,         // Ocarina Sun's Song "SEQ_ID_OCA_SUNMOON"
    /* 0x48 */ SEQ_ID_OCA_TIME,         // Ocarina Song of Time
    /* 0x49 */ SEQ_ID_OCA_STORM,        // Ocarina Song of Storms
    /* 0x4A */ SEQ_ID_NAVI_OPENING,     // Fairy Flying "SEQ_ID_NAVI"
    /* 0x4B */ SEQ_ID_DEKU_TREE_CS,     // Deku Tree "SEQ_ID_DEKUNOKI"
    /* 0x4C */ SEQ_ID_SONG_OF_STORMS,         // Windmill Hut "SEQ_ID_FUSHA"
    /* 0x4D */ SEQ_ID_HYRULE_CS,        // Legend of Hyrule "SEQ_ID_HIRAL_DEMO"
    /* 0x4E */ SEQ_ID_MINI_GAME,        // Shooting Gallery
    /* 0x4F */ SEQ_ID_SHEIK,            // Sheik's Theme "SEQ_ID_SEAK"
    /* 0x50 */ SEQ_ID_ZORA_DOMAIN,      // Zora's Domain "SEQ_ID_ZORA"
    /* 0x51 */ SEQ_ID_APPEAR,           // Enter Zelda
    /* 0x52 */ SEQ_ID_ADULT_LINK,       // Goodbye to Zelda
    /* 0x53 */ SEQ_ID_MASTER_SWORD,     // Master Sword
    /* 0x54 */ SEQ_ID_INTRO_GANON,
    /* 0x55 */ SEQ_ID_SHOP,             // Shop
    /* 0x56 */ SEQ_ID_CHAMBER_OF_SAGES, // Chamber of the Sages "SEQ_ID_KENJA"
    /* 0x57 */ SEQ_ID_FILE_SELECT,      // File Select
    /* 0x58 */ SEQ_ID_ICE_CAVERN,       // Ice Cavern "SEQ_ID_ICE_DUNGEON"
    /* 0x59 */ SEQ_ID_DOOR_OF_TIME,     // Open Door of Temple of Time "SEQ_ID_GATE_OPEN"
    /* 0x5A */ SEQ_ID_OWL,              // Kaepora Gaebora's Theme
    /* 0x5B */ SEQ_ID_SHADOW_TEMPLE,    // Shadow Temple "SEQ_ID_DARKNESS_DUNGEON"
    /* 0x5C */ SEQ_ID_WATER_TEMPLE,     // Water Temple "SEQ_ID_AQUA_DUNGEON"
    /* 0x5D */ SEQ_ID_BRIDGE_TO_GANONS, // Ganon's Castle Bridge "SEQ_ID_BRIDGE"
    /* 0x5E */ SEQ_ID_OCARINA_OF_TIME,  // Ocarina of Time "SEQ_ID_SARIA"
    /* 0x5F */ SEQ_ID_GERUDO_VALLEY,    // Gerudo Valley "SEQ_ID_GERUDO"
    /* 0x60 */ SEQ_ID_POTION_SHOP,      // Potion Shop "SEQ_ID_DRUGSTORE"
    /* 0x61 */ SEQ_ID_KOTAKE_KOUME,     // Kotake & Koume's Theme
    /* 0x62 */ SEQ_ID_ESCAPE,           // Escape from Ganon's Castle
    /* 0x63 */ SEQ_ID_UNDERGROUND,      // Ganon's Castle Under Ground
    /* 0x64 */ SEQ_ID_GANONDORF_BOSS,   // Ganondorf Battle
    /* 0x65 */ SEQ_ID_GANON_BOSS,       // Ganon Battle
    /* 0x66 */ SEQ_ID_END_DEMO,         // Seal of Six Sages
    /* 0x67 */ SEQ_ID_STAFF_1,          // End Credits I
    /* 0x68 */ SEQ_ID_STAFF_2,          // End Credits II
    /* 0x69 */ SEQ_ID_STAFF_3,          // End Credits III
    /* 0x6A */ SEQ_ID_STAFF_4,          // End Credits IV
    /* 0x6B */ SEQ_ID_FIRE_BOSS,        // King Dodongo & Volvagia Boss Battle "SEQ_ID_BOSS01"
    /* 0x6C */ SEQ_ID_TIMED_MINI_GAME,  // Mini-Game
    /* 0x6D */ SEQ_ID_CUTSCENE_EFFECTS, // A small collection of various cutscene sounds
    /* 0x7F */ SEQ_ID_NO_MUSIC = 0x7F,  // No bgm music is played
    /* 0x80 */ SEQ_ID_NATURE_SFX_RAIN = 0x80,  // Related to rain
    /* 0xFFFF */ SEQ_ID_DISABLED = 0xFFFF
} SeqId;

typedef enum {
    /* 0 */ SEQ_PLAYER_BGM_MAIN,
    /* 1 */ SEQ_PLAYER_FANFARE,
    /* 2 */ SEQ_PLAYER_SFX,
    /* 3 */ SEQ_PLAYER_BGM_SUB,
    /* 4 */ SEQ_PLAYER_MAX
} SequencePlayerId;

typedef enum {
    /* 0 */ SEQ_MODE_DEFAULT,
    /* 1 */ SEQ_MODE_ENEMY,
    /* 2 */ SEQ_MODE_STILL, // Not moving or first-person view
    /* 3 */ SEQ_MODE_IGNORE
} SequenceMode;

typedef enum {
    /* 0x0 */ SEQ_CS_EFFECTS_SWORD_GLOW, // Master sword glow
    /* 0x1 */ SEQ_CS_EFFECTS_SHEIK_TRANSFORM, // Sheik's transformation to Zelda
    /* 0x2 */ SEQ_CS_EFFECTS_SAGE_SEAL, // Sages accumulating their power
    /* 0x3 */ SEQ_CS_EFFECTS_FARORE_MAGIC, // Farore's magic creating life
    /* 0x4 */ SEQ_CS_EFFECTS_NAYRU_MAGIC, // Nayru's magic establishing order
    /* 0x5 */ SEQ_CS_EFFECTS_DIN_MAGIC, // Din's building of the earth
    /* 0x6 */ SEQ_CS_EFFECTS_LAVA_ERUPT, // Lava erupting from Volvagia's pit
    /* 0x7 */ SEQ_CS_EFFECTS_BONGO_HURL_LINK, // Link screaming while attacked by invisible Bongo Bongo
    /* 0x8 */ SEQ_CS_EFFECTS_BONGO_HOVER, // Bongo Bongo hovering menacingly
    /* 0x9 */ SEQ_CS_EFFECTS_BONGO_EMERGES, // Bongo Bongo emerging from the well
    /* 0xA */ SEQ_CS_EFFECTS_TRIAL_WARP, // Warping from one of the trial barriers
    /* 0xB */ SEQ_CS_EFFECTS_TRIAL_DESTROY, // Destroying one of the trial barriers
    /* 0xC */ SEQ_CS_EFFECTS_DISPEL_BARRIER, // Dispelling the Tower barrier
    /* 0xD */ SEQ_CS_EFFECTS_TOWER_COLLAPSE, // Ganon's Tower's collapse
    /* 0xE */ SEQ_CS_EFFECTS_LINK_SCREAM, // Child Link screaming (unused)
    /* 0xF */ SEQ_CS_EFFECTS_RAINFALL // Rain with thunder effects
} SequenceCutsceneEffects;

typedef enum {
    /* 0x0 */ CHANNEL_IO_PORT_0,
    /* 0x1 */ CHANNEL_IO_PORT_1,
    /* 0x2 */ CHANNEL_IO_PORT_2,
    /* 0x3 */ CHANNEL_IO_PORT_3,
    /* 0x4 */ CHANNEL_IO_PORT_4,
    /* 0x5 */ CHANNEL_IO_PORT_5,
    /* 0x6 */ CHANNEL_IO_PORT_6,
    /* 0x7 */ CHANNEL_IO_PORT_7
} ChannelIOPort;

typedef enum {
    /* 0 */ VOL_SCALE_INDEX_BGM_MAIN,
    /* 1 */ VOL_SCALE_INDEX_FANFARE,
    /* 2 */ VOL_SCALE_INDEX_SFX,
    /* 3 */ VOL_SCALE_INDEX_BGM_SUB,
    /* 4 */ VOL_SCALE_INDEX_MAX
} VolumeScaleIndex; // May be worth using SequencePlayerId instead

typedef struct {
    /* 0x00 */ f32 volCur;
    /* 0x04 */ f32 volTarget;
    /* 0x08 */ f32 volStep;
    /* 0x0C */ u16 volTimer;
    /* 0x10 */ f32 freqScaleCur;
    /* 0x14 */ f32 freqScaleTarget;
    /* 0x18 */ f32 freqScaleStep;
    /* 0x1C */ u16 freqScaleTimer;
} ActiveSequenceChannelData; // size = 0x20

typedef struct {
    /* 0x000 */ f32 volCur;
    /* 0x004 */ f32 volTarget;
    /* 0x008 */ f32 volStep;
    /* 0x00C */ u16 volTimer;
    /* 0x00E */ u8 volScales[VOL_SCALE_INDEX_MAX];
    /* 0x012 */ u8 volFadeTimer;
    /* 0x013 */ u8 fadeVolUpdate;
    /* 0x014 */ u32 tempoCmd;
    /* 0x018 */ u16 tempoOriginal; // stores the original tempo before modifying it (to reset back to)
    /* 0x01C */ f32 tempoCur;
    /* 0x020 */ f32 tempoTarget;
    /* 0x024 */ f32 tempoStep;
    /* 0x028 */ u16 tempoTimer;
    /* 0x02C */ u32 setupCmd[8]; // a queue of cmds to execute once the player is disabled
    /* 0x04C */ u8 setupCmdTimer; // only execute setup commands when the timer is at 0.
    /* 0x04D */ u8 setupCmdNum; // number of setup commands requested once the player is disabled
    /* 0x04E */ u8 setupFadeTimer;
    /* 0x050 */ ActiveSequenceChannelData channelData[16];
    /* 0x250 */ u16 freqScaleChannelFlags;
    /* 0x252 */ u16 volChannelFlags;
    /* 0x254 */ u16 seqId; // active seqId currently playing. Resets when sequence stops
    /* 0x256 */ u16 prevSeqId; // last seqId played on a player. Does not reset when sequence stops
    /* 0x258 */ u16 channelPortMask;
    /* 0x25C */ u32 startSeqCmd; // This name comes from MM
    /* 0x260 */ u8 isWaitingForFonts; // This name comes from MM
} ActiveSequence; // size = 0x264

typedef enum {
    /* 0x0 */ AMBIENCE_CHANNEL_STREAM_0,
    /* 0x1 */ AMBIENCE_CHANNEL_CRITTER_0,
    /* 0x2 */ AMBIENCE_CHANNEL_CRITTER_1,
    /* 0x3 */ AMBIENCE_CHANNEL_CRITTER_2,
    /* 0x4 */ AMBIENCE_CHANNEL_CRITTER_3,
    /* 0x5 */ AMBIENCE_CHANNEL_CRITTER_4,
    /* 0x6 */ AMBIENCE_CHANNEL_CRITTER_5,
    /* 0x7 */ AMBIENCE_CHANNEL_CRITTER_6,
    /* 0x8 */ AMBIENCE_CHANNEL_CRITTER_7,
    /* 0xC */ AMBIENCE_CHANNEL_STREAM_1 = 12,
    /* 0xD */ AMBIENCE_CHANNEL_UNK,
    /* 0xE */ AMBIENCE_CHANNEL_RAIN,
    /* 0xF */ AMBIENCE_CHANNEL_LIGHTNING
} AmbienceChannelIndex; // seqPlayerIndex = 0 (Overlaps with main bgm)

typedef enum {
    /* 0x00 */ AMBIENCE_ID_GENERAL_NIGHT,
    /* 0x01 */ AMBIENCE_ID_MARKET_ENTRANCE,
    /* 0x02 */ AMBIENCE_ID_KAKARIKO_REGION,
    /* 0x03 */ AMBIENCE_ID_MARKET_RUINS,
    /* 0x04 */ AMBIENCE_ID_KOKIRI_REGION,
    /* 0x05 */ AMBIENCE_ID_MARKET_NIGHT,
    /* 0x06 */ AMBIENCE_ID_06,
    /* 0x07 */ AMBIENCE_ID_GANONS_LAIR,
    /* 0x08 */ AMBIENCE_ID_08,
    /* 0x09 */ AMBIENCE_ID_09,
    /* 0x0A */ AMBIENCE_ID_WASTELAND,
    /* 0x0B */ AMBIENCE_ID_COLOSSUS,
    /* 0x0C */ AMBIENCE_ID_DEATH_MOUNTAIN_TRAIL,
    /* 0x0D */ AMBIENCE_ID_0D,
    /* 0x0E */ AMBIENCE_ID_0E,
    /* 0x0F */ AMBIENCE_ID_0F,
    /* 0x10 */ AMBIENCE_ID_10,
    /* 0x11 */ AMBIENCE_ID_11,
    /* 0x12 */ AMBIENCE_ID_12,
    /* 0x13 */ AMBIENCE_ID_NONE,
    /* 0xFF */ AMBIENCE_ID_DISABLED = 0xFF
} NatureAmbienceId;

typedef enum {
    /* 0x00 */ AMBIENCE_STREAM_RUSHING_WATER,
    /* 0x01 */ AMBIENCE_STREAM_HOWLING_WIND,
    /* 0x02 */ AMBIENCE_STREAM_SCREECHING_WIND,
    /* 0x03 */ AMBIENCE_STREAM_SCREECHING_WIND_ALT1
} NatureStreamId;

typedef enum {
    /* 0x00 */ AMBIENCE_CRITTER_BIRD_CHIRP_1,
    /* 0x01 */ AMBIENCE_CRITTER_TAP,
    /* 0x02 */ AMBIENCE_CRITTER_BIRD_CHIRP_2,
    /* 0x03 */ AMBIENCE_CRITTER_BIRD_CHIRP_1_ALT1,
    /* 0x04 */ AMBIENCE_CRITTER_CRICKETS,
    /* 0x05 */ AMBIENCE_CRITTER_BIRD_CHIRP_1_ALT2,
    /* 0x06 */ AMBIENCE_CRITTER_LOUD_CHIRPING,
    /* 0x07 */ AMBIENCE_CRITTER_BIRD_CHIRP_1_ALT3,
    /* 0x08 */ AMBIENCE_CRITTER_BIRD_CHIRP_1_ALT4,
    /* 0x09 */ AMBIENCE_CRITTER_CROWS_CAWS,
    /* 0x0A */ AMBIENCE_CRITTER_SMALL_BIRD_CHIRPS,
    /* 0x0B */ AMBIENCE_CRITTER_BIRD_SCREECH,
    /* 0x0C */ AMBIENCE_CRITTER_BIRD_SONG,
    /* 0x0D */ AMBIENCE_CRITTER_OWL_HOOT,
    /* 0x0E */ AMBIENCE_CRITTER_HAWK_SCREECH,
    /* 0x0F */ AMBIENCE_CRITTER_BIRD_CALL,
    /* 0x10 */ AMBIENCE_CRITTER_CAWING_BIRD,
    /* 0x11 */ AMBIENCE_CRITTER_CUCCO_CROWS,
    /* 0x12 */ AMBIENCE_CRITTER_BIRD_CHIRP_2_ALT1,
    /* 0x13 */ AMBIENCE_CRITTER_BIRD_CHIRP_1_ALT5
} NatureAmimalId;

#define AMBIENCE_IO_CRITTER_0_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_0_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_0_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_0_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_0, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_1_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_1_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_1_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_1_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_1, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_2_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_2_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_2_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_2_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_2, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_3_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_3_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_3_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_3_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_3, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_4_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_4_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_4_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_4_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_4, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_5_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_5_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_5_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_5_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_5, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_CRITTER_6_TYPE(type)        AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_CRITTER_6_BEND_PITCH(bend)  AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_3, bend
#define AMBIENCE_IO_CRITTER_6_NUM_LAYERS(num)   AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_4, num
#define AMBIENCE_IO_CRITTER_6_PORT5(reverb)     AMBIENCE_CHANNEL_CRITTER_6, CHANNEL_IO_PORT_5, reverb

#define AMBIENCE_IO_STREAM_0_TYPE(type)         AMBIENCE_CHANNEL_STREAM_0, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_STREAM_0_PORT3(data)        AMBIENCE_CHANNEL_STREAM_0, CHANNEL_IO_PORT_3, data
#define AMBIENCE_IO_STREAM_0_PORT4(data)        AMBIENCE_CHANNEL_STREAM_0, CHANNEL_IO_PORT_4, data

#define AMBIENCE_IO_STREAM_1_TYPE(type)         AMBIENCE_CHANNEL_STREAM_1, CHANNEL_IO_PORT_2, type
#define AMBIENCE_IO_STREAM_1_PORT3(data)        AMBIENCE_CHANNEL_STREAM_1, CHANNEL_IO_PORT_3, data
#define AMBIENCE_IO_STREAM_1_PORT4(data)        AMBIENCE_CHANNEL_STREAM_1, CHANNEL_IO_PORT_4, data

#define AMBIENCE_IO_ENTRIES_END 0xFF

#endif
