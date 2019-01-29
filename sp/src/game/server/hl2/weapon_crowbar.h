//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef WEAPON_CROWBAR_H
#define WEAPON_CROWBAR_H

#include "basebludgeonweapon.h"

#if defined( _WIN32 )
#pragma once
#endif

#ifdef HL2MP
#error weapon_crowbar.h must not be included in hl2mp. The windows compiler will use the wrong class elsewhere if it is.
#endif

//-----------------------------------------------------------------------------
// CWeaponCrowbar
//-----------------------------------------------------------------------------

class CWeaponCrowbar : public CBaseCombatWeapon
{
	DECLARE_CLASS( CWeaponCrowbar, CBaseCombatWeapon );
public:

	CWeaponCrowbar();

	void			Precache( void );
	virtual void	ItemPostFrame( void );
	void			PrimaryAttack( void );

public:
	trace_t		m_traceHit;
	Activity	m_nHitActivity;

private:
	virtual void		Swing( void );
	virtual	void		Hit( void );
	virtual	void		ImpactEffect( void );
	virtual	void		ImpactSound( bool isWorld );
	virtual Activity	ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner );

public:
	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
};

#endif // WEAPON_CROWBAR_H
