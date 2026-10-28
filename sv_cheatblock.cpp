#include "cbase.h"
#include "icvar.h"
#include "achievementmgr.h"

static ConVar* sv_cheats_var = nullptr;
static bool g_bCheatAchievementGiven = false;

// CHANGE THIS to your achievement ID
#define ACHIEVEMENT_NICETRY  13

static void SvCheatsChangeCallback( ConVar* var, const char* pOldValue, float flOldValue )
{
    // If someone tries to set sv_cheats to 1
    if ( var->GetBool() )
    {
        // Give achievement only once
        if (!g_bCheatAchievementGiven)
        {
            g_bCheatAchievementGiven = true;

            IAchievementMgr* pAchMgr = engine->GetAchievementMgr();
            if (pAchMgr)
            {
                pAchMgr->AwardAchievement(ACHIEVEMENT_NICETRY);
            }

            Msg("Nice try. Cheats are not allowed.\n");
        }

        // Force back to 0
        var->SetValue(0);
    }
}
