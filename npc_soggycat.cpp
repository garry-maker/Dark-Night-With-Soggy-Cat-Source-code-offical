#include "cbase.h"
#include "npc_soggycat.h"
#include "ai_basenpc.h"
#include "ai_motor.h"
#include "ai_navigator.h"
#include "ai_senses.h"
#include "player.h"
#include "ai_network.h"
#include "ai_node.h"
#include "mathlib/mathlib.h"

LINK_ENTITY_TO_CLASS(npc_soggycat, CNPCSoggyCat);

BEGIN_DATADESC(CNPCSoggyCat)
    DEFINE_FIELD(m_flLookStartTime, FIELD_TIME),
    DEFINE_FIELD(m_flLastLookCheck, FIELD_TIME),
    DEFINE_FIELD(m_bPlayerCurrentlyLooking, FIELD_BOOLEAN),
    DEFINE_FIELD(m_flLookThreshold, FIELD_FLOAT),
    DEFINE_FIELD(m_flLookForgiveTime, FIELD_FLOAT),
    DEFINE_FIELD(m_flLastTeleport, FIELD_TIME),
    DEFINE_FIELD(m_flNextDirectorThink, FIELD_TIME),
END_DATADESC()

CNPCSoggyCat::CNPCSoggyCat()
{
    m_flLookStartTime = 0.0f;
    m_flLastLookCheck = 0.0f;
    m_bPlayerCurrentlyLooking = false;

    m_flLookThreshold = 3.0f;
    m_flLookForgiveTime = 0.5f;
    m_flLastTeleport = 0.0f;

    m_flNextDirectorThink = 0.0f;
}

void CNPCSoggyCat::Precache()
{
    PrecacheModel("models/soggycat/soggycat.mdl");

    PrecacheScriptSound("SoggyCat.Glitch");
    PrecacheScriptSound("SoggyCat.Jumpscare");

    PrecacheMaterial("effects/soggycat_glitch");
    PrecacheMaterial("effects/soggycat_jumpscare");

    BaseClass::Precache();
}

void CNPCSoggyCat::Spawn()
{
    Precache();
    SetModel("models/soggycat/soggycat.mdl");

    SetHullType(HULL_HUMAN);
    SetHullSizeNormal();
    SetSolid(SOLID_BBOX);
    SetMoveType(MOVETYPE_STEP);

    SetHealth(9999);
    AddSolidFlags(FSOLID_NOT_STANDABLE);

    NPCInit();
    SetNavType(NAV_GROUND);

    SetThink(&CNPCSoggyCat::Think);
    SetNextThink(gpGlobals->curtime + 0.1f);
}

void CNPCSoggyCat::Think()
{
    CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
    if (!pPlayer)
    {
        SetNextThink(gpGlobals->curtime + 0.1f);
        return;
    }

    float flNow = gpGlobals->curtime;

    if (flNow > m_flNextDirectorThink)
    {
        DirectorThink(pPlayer);
        m_flNextDirectorThink = flNow + 1.0f;
    }

    UpdatePlayerLookState();

    SetNextThink(gpGlobals->curtime + 0.05f);
}

bool CNPCSoggyCat::IsPlayerLookingAtMe(CBasePlayer* pPlayer)
{
    Vector vecToMe = GetAbsOrigin() - pPlayer->EyePosition();
    Vector forward;
    pPlayer->EyeVectors(&forward);

    vecToMe.NormalizeInPlace();
    float flDot = DotProduct(forward, vecToMe);

    if (flDot < 0.85f)
        return false;

    trace_t tr;
    UTIL_TraceLine(pPlayer->EyePosition(), GetAbsOrigin(),
                   MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr);

    return (tr.m_pEnt == this);
}

void CNPCSoggyCat::UpdatePlayerLookState()
{
    CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
    if (!pPlayer)
        return;

    float flNow = gpGlobals->curtime;
    bool bLooking = IsPlayerLookingAtMe(pPlayer);

    if (bLooking)
    {
        if (!m_bPlayerCurrentlyLooking)
        {
            m_bPlayerCurrentlyLooking = true;
            m_flLookStartTime = flNow;
        }
        else
        {
            float flLookDuration = flNow - m_flLookStartTime;

            if (flLookDuration > 1.0f)
                TriggerGlitchEffect(pPlayer);

            if (flLookDuration > m_flLookThreshold)
                TriggerJumpscare(pPlayer);
        }
    }
    else
    {
        if (m_bPlayerCurrentlyLooking)
        {
            float flLookDuration = flNow - m_flLookStartTime;

            if (flLookDuration < m_flLookThreshold)
                TeleportAwayFromPlayer(pPlayer);

            m_bPlayerCurrentlyLooking = false;
            m_flLookStartTime = 0.0f;
        }

        if (flNow - m_flLastTeleport > 3.0f)
        {
            TeleportBehindPlayer(pPlayer);
            m_flLastTeleport = flNow;
        }
    }
}

void CNPCSoggyCat::TeleportBehindPlayer(CBasePlayer* pPlayer)
{
    Vector forward;
    pPlayer->EyeVectors(&forward);

    Vector pos = pPlayer->GetAbsOrigin() - forward * 120.0f;
    pos.z = pPlayer->GetAbsOrigin().z;

    Teleport(&pos, NULL, NULL);
}

void CNPCSoggyCat::TeleportAwayFromPlayer(CBasePlayer *pPlayer)
{
    if (!g_pBigAINet)
        return;

    int nodeCount = g_pBigAINet->NumNodes();

    if (nodeCount <= 0)
        return;

    // Essaie plusieurs nœuds aléatoires jusqu'à en trouver un assez éloigné.
    for (int i = 0; i < 32; i++)
    {
        int index = random->RandomInt(0, nodeCount - 1);

        CAI_Node *pNode = g_pBigAINet->GetNode(index);

        if (!pNode)
            continue;

        Vector pos = pNode->GetPosition(HULL_HUMAN);

        if ((pos - pPlayer->GetAbsOrigin()).LengthSqr() < Square(512.0f))
            continue;

        Teleport(&pos, NULL, NULL);
        return;
    }

    // Fallback : téléporte simplement loin du joueur.
    Vector forward;
    pPlayer->EyeVectors(&forward);

    Vector pos = pPlayer->GetAbsOrigin() - forward * 768.0f;
    Teleport(&pos, NULL, NULL);
}

void CNPCSoggyCat::TriggerGlitchEffect(CBasePlayer* pPlayer)
{
    pPlayer->EmitSound("SoggyCat.Glitch");
    engine->ClientCommand(pPlayer->edict(), "r_screenoverlay effects/soggycat_glitch\n");
}

void CNPCSoggyCat::TriggerJumpscare(CBasePlayer* pPlayer)
{
    pPlayer->EmitSound("SoggyCat.Jumpscare");
    engine->ClientCommand(pPlayer->edict(), "r_screenoverlay effects/soggycat_jumpscare\n");

    color32 black = {0, 0, 0, 255};
    UTIL_ScreenFade(pPlayer, black, 1.0f, 0.0f, FFADE_OUT);

    pPlayer->TakeDamage(CTakeDamageInfo(this, this, 9999, DMG_GENERIC));
}

void CNPCSoggyCat::DirectorThink(CBasePlayer* pPlayer)
{
    AI_NavGoal_t goal(pPlayer->GetAbsOrigin());
    GetNavigator()->SetGoal(goal);
}
