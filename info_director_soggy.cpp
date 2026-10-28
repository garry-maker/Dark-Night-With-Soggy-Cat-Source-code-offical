#include "cbase.h"
#include "info_director_soggy.h"
#include "npc_soggycat.h"

#include "ai_network.h"
#include "ai_node.h"
#include "ai_initutils.h"

LINK_ENTITY_TO_CLASS(info_director_soggy, CInfoDirectorSoggy);

BEGIN_DATADESC(CInfoDirectorSoggy)
    DEFINE_FIELD(m_flSpawnDelay, FIELD_FLOAT),
    DEFINE_FIELD(m_flNextSpawnCheck, FIELD_TIME),
    DEFINE_FIELD(m_hSoggyCat, FIELD_EHANDLE),
END_DATADESC()

void CInfoDirectorSoggy::Spawn()
{
    BaseClass::Spawn();

    m_flSpawnDelay = 2.0f;
    m_flNextSpawnCheck = gpGlobals->curtime + m_flSpawnDelay;

    SetThink(&CInfoDirectorSoggy::Think);
    SetNextThink(gpGlobals->curtime + 0.1f);
}

void CInfoDirectorSoggy::Think()
{
    // Wait until the AI network is available.
    if (!g_pBigAINet || g_pBigAINet->NumNodes() <= 0)
    {
        SetNextThink(gpGlobals->curtime + 1.0f);
        return;
    }

    // Respawn the cat if necessary.
    if (!m_hSoggyCat || !m_hSoggyCat->IsAlive())
    {
        if (gpGlobals->curtime >= m_flNextSpawnCheck)
        {
            SpawnSoggyCat();
            m_flNextSpawnCheck = gpGlobals->curtime + 5.0f;
        }
    }

    SetNextThink(gpGlobals->curtime + 1.0f);
}

void CInfoDirectorSoggy::SpawnSoggyCat()
{
    if (!g_pBigAINet)
        return;

    int nodeCount = g_pBigAINet->NumNodes();

    if (nodeCount <= 0)
        return;

    // Try several random nodes.
    for (int i = 0; i < 32; i++)
    {
        int nodeIndex = random->RandomInt(0, nodeCount - 1);

        Vector pos = g_pBigAINet->GetNodePosition(HULL_HUMAN, nodeIndex);

        CNPCSoggyCat *pCat =
            dynamic_cast<CNPCSoggyCat *>(CreateEntityByName("npc_soggycat"));

        if (!pCat)
            return;

        pCat->SetAbsOrigin(pos);
        pCat->SetAbsAngles(QAngle(0, random->RandomFloat(0, 360), 0));
        pCat->Spawn();

        m_hSoggyCat = pCat;
        return;
    }
}

void CInfoDirectorSoggy::RemoveExistingSoggyCat()
{
    if (m_hSoggyCat)
    {
        UTIL_Remove(m_hSoggyCat);
        m_hSoggyCat = NULL;
    }
}
