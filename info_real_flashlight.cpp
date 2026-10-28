#include "cbase.h"
#include "player.h"

class CInfoRealFlashlight : public CBaseEntity
{
public:
    DECLARE_CLASS(CInfoRealFlashlight, CBaseEntity);
    DECLARE_DATADESC();

    void Spawn() override
    {
        BaseClass::Spawn();

        SetThink(&CInfoRealFlashlight::Think);
        SetNextThink(gpGlobals->curtime + 0.1f);
    }

    void Think()
    {
        CBasePlayer *pPlayer = UTIL_GetLocalPlayer();

        if (!pPlayer)
        {
            SetNextThink(gpGlobals->curtime + 0.1f);
            return;
        }

        if (!m_hLight)
            CreateLight();

        if (m_hLight)
        {
            m_hLight->SetAbsOrigin(pPlayer->EyePosition());
            m_hLight->SetAbsAngles(pPlayer->EyeAngles());
        }

        SetNextThink(gpGlobals->curtime + 0.01f);
    }

    void CreateLight()
    {
        CBaseEntity *pLight = CreateEntityByName("light_dynamic");

        if (!pLight)
            return;

        pLight->KeyValue("brightness", "3");
        pLight->KeyValue("distance", "400");
        pLight->KeyValue("style", "0");
        pLight->KeyValue("color", "255 255 220");

        pLight->Spawn();

        m_hLight = pLight;
    }

private:
    EHANDLE m_hLight;
};

LINK_ENTITY_TO_CLASS(info_real_flashlight, CInfoRealFlashlight);

BEGIN_DATADESC(CInfoRealFlashlight)
    DEFINE_FIELD(m_hLight, FIELD_EHANDLE),
END_DATADESC()
