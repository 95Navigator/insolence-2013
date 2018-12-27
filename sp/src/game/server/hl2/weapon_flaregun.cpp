//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Flare gun (fffsssssssssss!!)
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "basehlcombatweapon.h"
#include "decals.h"
#include "soundenvelope.h"
#include "IEffects.h"
#include "engine/IEngineSound.h"
#include "weapon_flaregun.h"

#define	FLARE_LAUNCH_SPEED	1500

LINK_ENTITY_TO_CLASS( env_flare, CFlare );

BEGIN_DATADESC( CFlare )

	DEFINE_FIELD( m_pOwner,			FIELD_CLASSPTR ),
	DEFINE_FIELD( m_nBounces,		FIELD_INTEGER ),
	DEFINE_FIELD( m_flTimeBurnOut,	FIELD_TIME ),
	DEFINE_KEYFIELD( m_flScale,		FIELD_FLOAT, "scale" ),
	DEFINE_KEYFIELD( m_flDuration,	FIELD_FLOAT, "duration" ),
	DEFINE_FIELD( m_flNextDamage,	FIELD_TIME ),
	DEFINE_SOUNDPATCH( m_pBurnSound ),
	DEFINE_FIELD( m_bFading,		FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bLight,			FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bSmoke,			FIELD_BOOLEAN ),

	//Input functions
	DEFINE_INPUTFUNC( FIELD_FLOAT, "Start", InputStart ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "Die", InputDie ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Launch", InputLaunch),

	// Function Pointers
	DEFINE_FUNCTION( FlareTouch ),
	DEFINE_FUNCTION( FlareBurnTouch ),
	DEFINE_FUNCTION( FlareThink ),

END_DATADESC()

//Data-tables
IMPLEMENT_SERVERCLASS_ST( CFlare, DT_Flare )
	SendPropFloat( SENDINFO( m_flTimeBurnOut ), 0,	SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_flScale ), 0, SPROP_NOSCALE ),
	SendPropInt( SENDINFO( m_bLight ), 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_bSmoke ), 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

Class_T CFlare::Classify( void )
{
	return CLASS_FLARE; 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CFlare::CFlare( void )
{
	m_flScale		= 1.0f;
	m_nBounces		= 0;
	m_bFading		= false;
	m_bLight		= true;
	m_bSmoke		= true;
	m_flNextDamage	= gpGlobals->curtime;
	m_lifeState		= LIFE_ALIVE;
	m_iHealth		= 100;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::Precache( void )
{
	PrecacheModel("models/weapons/flare.mdl" );

	PrecacheScriptSound( "Weapon_FlareGun.Burn" );

  	// FIXME: needed to precache the fire model.  Shouldn't have to do this.
  	UTIL_PrecacheOther( "_firesmoke" );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &restore - 
// Output : int
//-----------------------------------------------------------------------------
int CFlare::Restore( IRestore &restore )
{
	int result = BaseClass::Restore( restore );

	if ( m_spawnflags & SF_FLARE_NO_DLIGHT )
	{
		m_bLight = false;
	}

	if ( m_spawnflags & SF_FLARE_NO_SMOKE )
	{
		m_bSmoke = false;
	}

	CPASAttenuationFilter filter( this );
	m_pBurnSound = CSoundEnvelopeController::GetController().SoundCreate( filter, entindex(), CHAN_WEAPON, "weapons/flaregun/burn.wav", 2.0f );

	if ( ( m_flTimeBurnOut != -1.0f ) && ( m_flTimeBurnOut > gpGlobals->curtime ) )
	{
		CSoundEnvelopeController::GetController().Play( m_pBurnSound, 0.0f, 60 );
		CSoundEnvelopeController::GetController().SoundChangeVolume( m_pBurnSound, 0.8f, 2.0f );
		CSoundEnvelopeController::GetController().SoundChangePitch( m_pBurnSound, 100, 2.0f );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::Spawn( void )
{
	Precache();

	SetModel( "models/weapons/flare.mdl" );

	UTIL_SetSize( this, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ) );

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_SOLID );

	SetMoveType( MOVETYPE_NONE );
	SetFriction( 0.6f );
	SetGravity(0.5f);
	m_flTimeBurnOut = gpGlobals->curtime + 30;

	AddEffects(EF_NOSHADOW|EF_NORECEIVESHADOW);

	if ( m_spawnflags & SF_FLARE_NO_DLIGHT )
	{
		m_bLight = false;
	}

	if ( m_spawnflags & SF_FLARE_NO_SMOKE )
	{
		m_bSmoke = false;
	}

	if ( m_spawnflags & SF_FLARE_INFINITE )
	{
		m_flTimeBurnOut = -1.0f;
	}

	if ( m_spawnflags & SF_FLARE_START_OFF )
	{
		AddEffects( EF_NODRAW );
	}

	AddFlag( FL_OBJECT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::Activate( void )
{
	BaseClass::Activate();

	// Start the burning sound if we're already on
	if ( ( m_spawnflags & SF_FLARE_START_OFF ) == false )
	{
		StartBurnSound();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::StartBurnSound( void )
{
	if ( m_pBurnSound == NULL )
	{
		CPASAttenuationFilter filter( this );
		m_pBurnSound = CSoundEnvelopeController::GetController().SoundCreate( 
			filter, entindex(), CHAN_WEAPON, "weapons/flaregun/burn.wav", 3.0f );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : vecOrigin - 
//			vecAngles - 
//			*pOwner - 
// Output : CFlare
//-----------------------------------------------------------------------------
CFlare *CFlare::Create( Vector vecOrigin, QAngle vecAngles, CBaseEntity *pOwner, float lifetime )
{
	CFlare *pFlare = (CFlare *) CreateEntityByName( "env_flare" );

	if ( pFlare == NULL )
		return NULL;

	UTIL_SetOrigin( pFlare, vecOrigin );

	pFlare->SetLocalAngles( vecAngles );
	pFlare->Spawn();
	pFlare->SetTouch( &CFlare::FlareTouch );
	pFlare->SetThink( &CFlare::FlareThink );
	
	//Start up the flare
	pFlare->Start( lifetime );

	//Don't start sparking immediately
	pFlare->SetNextThink( gpGlobals->curtime + 0.5f );

	//Burn out time
	pFlare->m_flTimeBurnOut = gpGlobals->curtime + lifetime;

	pFlare->RemoveSolidFlags( FSOLID_NOT_SOLID );
	pFlare->AddSolidFlags( FSOLID_NOT_STANDABLE );

	pFlare->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );

	pFlare->SetOwnerEntity( pOwner );
	pFlare->m_pOwner	= pOwner;

	return pFlare;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
unsigned int CFlare::PhysicsSolidMaskForEntity( void ) const
{
	return MASK_NPCSOLID;

/*
	if ( GetFlags() & FL_NPC )
	{
	}
	else if ( IsPlayer() )
	{
		return MASK_PLAYERSOLID;
	}
	return MASK_OPAQUE;
*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::FlareThink( void )
{
	float	deltaTime = ( m_flTimeBurnOut - gpGlobals->curtime );

	if ( m_flTimeBurnOut != -1.0f )
	{
		//Fading away
		if ( ( deltaTime <= FLARE_DECAY_TIME ) && ( m_bFading == false ) )
		{
			m_bFading = true;
			CSoundEnvelopeController::GetController().SoundChangePitch( m_pBurnSound, 60, deltaTime );
			CSoundEnvelopeController::GetController().SoundFadeOut( m_pBurnSound, deltaTime );
		}

		//Burned out
		if ( m_flTimeBurnOut < gpGlobals->curtime )
		{
			CSoundEnvelopeController::GetController().SoundDestroy( m_pBurnSound );
			m_pBurnSound = NULL;
			UTIL_Remove( this );
			return;
		}
	}
	
	//Act differently underwater
	if ( GetWaterLevel() > 1 )
	{
		UTIL_Bubbles( GetAbsOrigin() + Vector( -2, -2, -2 ), GetAbsOrigin() + Vector( 2, 2, 2 ), 1 );
		m_bSmoke = false;
	}
	else
	{
		//Shoot sparks
		if ( random->RandomInt( 0, 8 ) == 1 )
		{
			g_pEffects->Sparks( GetAbsOrigin() );
		}
	}

	//Next update
	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CFlare::FlareBurnTouch( CBaseEntity *pOther )
{
	if ( pOther && pOther->m_takedamage && ( m_flNextDamage < gpGlobals->curtime ) )
	{
		pOther->TakeDamage( CTakeDamageInfo( this, m_pOwner, 1, (DMG_BULLET|DMG_BURN) ) );
		m_flNextDamage = gpGlobals->curtime + 1.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CFlare::FlareTouch( CBaseEntity *pOther )
{
	Assert( pOther );
	if ( !pOther->IsSolid() )
		return;

	if ( ( m_nBounces < 10 ) && ( GetWaterLevel() < 1 ) )
	{
		// Throw some real chunks here
		g_pEffects->Sparks( GetAbsOrigin() );
	}

	//If the flare hit a person or NPC, do damage here.
	if ( pOther && pOther->m_takedamage )
	{
		/*
			The Flare is the iRifle round right now. No damage, just ignite. (sjb)

		//Damage is a function of how fast the flare is flying.
		int iDamage = GetAbsVelocity().Length() / 50.0f;

		if ( iDamage < 5 )
		{
			//Clamp minimum damage
			iDamage = 5;
		}

		//Use m_pOwner, not GetOwnerEntity()
		pOther->TakeDamage( CTakeDamageInfo( this, m_pOwner, iDamage, (DMG_BULLET|DMG_BURN) ) );
		m_flNextDamage = gpGlobals->curtime + 1.0f;
		*/

		CBaseCombatCharacter *pBCC;
		pBCC = pOther->MyCombatCharacterPointer();
		
		if( pBCC )
		{
			if( !pBCC->IsOnFire() )
			{
				pOther->MyCombatCharacterPointer()->Ignite( 100.0f );
			}
		}

		Vector vecNewVelocity = GetAbsVelocity();
		vecNewVelocity	*= 0.1f;
		SetAbsVelocity( vecNewVelocity );

		SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
		SetGravity(1.0f);


		Die( 0.5 );

		return;
	}
	else
	{
		// hit the world, check the material type here, see if the flare should stick.
		trace_t tr;
		tr = CBaseEntity::GetTouchTrace();

		//Only do this on the first bounce
		if ( m_nBounces == 0 )
		{
			surfacedata_t *pdata = physprops->GetSurfaceData( tr.surface.surfaceProps );	

			if ( pdata != NULL )
			{
				//Only embed into concrete and wood (jdw: too obscure for players?)
				//if ( ( pdata->gameMaterial == 'C' ) || ( pdata->gameMaterial == 'W' ) )
				{
					Vector	impactDir = ( tr.endpos - tr.startpos );
					VectorNormalize( impactDir );

					float	surfDot = tr.plane.normal.Dot( impactDir );

					//Do not stick to ceilings or on shallow impacts
					if ( ( tr.plane.normal.z > -0.5f ) && ( surfDot < -0.9f ) )
					{
						RemoveSolidFlags( FSOLID_NOT_SOLID );
						AddSolidFlags( FSOLID_TRIGGER );
						UTIL_SetOrigin( this, tr.endpos + ( tr.plane.normal * 2.0f ) );
						SetAbsVelocity( vec3_origin );
						SetMoveType( MOVETYPE_NONE );
						
						SetTouch( &CFlare::FlareBurnTouch );
						
						int index = decalsystem->GetDecalIndexForName( "SmallScorch" );
						if ( index >= 0 )
						{
							CBroadcastRecipientFilter filter;
							te->Decal( filter, 0.0, &tr.endpos, &tr.startpos, ENTINDEX( tr.m_pEnt ), tr.hitbox, index );
						}
						
						CPASAttenuationFilter filter2( this, "Flare.Touch" );
						EmitSound( filter2, entindex(), "Flare.Touch" );

						return;
					}
				}
			}
		}

		//Scorch decal
		if ( GetAbsVelocity().LengthSqr() > (250*250) )
		{
			int index = decalsystem->GetDecalIndexForName( "FadingScorch" );
			if ( index >= 0 )
			{
				CBroadcastRecipientFilter filter;
				te->Decal( filter, 0.0, &tr.endpos, &tr.startpos, ENTINDEX( tr.m_pEnt ), tr.hitbox, index );
			}
		}

		// Change our flight characteristics
		SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
		SetGravity(0.8f);
		
		m_nBounces++;

		//After the first bounce, smacking into whoever fired the flare is fair game
		SetOwnerEntity( this );	

		// Slow down
		Vector vecNewVelocity = GetAbsVelocity();
		vecNewVelocity.x *= 0.8f;
		vecNewVelocity.y *= 0.8f;
		SetAbsVelocity( vecNewVelocity );

		//Stopped?
		if ( GetAbsVelocity().Length() < 64.0f )
		{
			SetAbsVelocity( vec3_origin );
			SetMoveType( MOVETYPE_NONE );
			RemoveSolidFlags( FSOLID_NOT_SOLID );
			AddSolidFlags( FSOLID_TRIGGER );
			SetTouch( &CFlare::FlareBurnTouch );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::Start( float lifeTime )
{
	StartBurnSound();

	if ( m_pBurnSound != NULL )
	{		
		CSoundEnvelopeController::GetController().Play( m_pBurnSound, 0.0f, 60 );
		CSoundEnvelopeController::GetController().SoundChangeVolume( m_pBurnSound, 0.8f, 2.0f );
		CSoundEnvelopeController::GetController().SoundChangePitch( m_pBurnSound, 100, 2.0f );
	}

	m_flTimeBurnOut = gpGlobals->curtime + lifeTime;

	RemoveEffects( EF_NODRAW );

	SetThink( &CFlare::FlareThink );
	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::Die( float fadeTime )
{
	m_flTimeBurnOut = gpGlobals->curtime + fadeTime;

	SetThink( &CFlare::FlareThink );
	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlare::Launch( const Vector &direction, float speed )
{
	// Make sure we're visible
	Start( 8.0f );

	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );

	// Punch our velocity towards our facing
	SetAbsVelocity( direction * FLARE_LAUNCH_SPEED );

	SetGravity( 1.0f );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CFlare::InputStart( inputdata_t &inputdata )
{
	Start( inputdata.value.Float() );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CFlare::InputDie( inputdata_t &inputdata )
{
	Die( inputdata.value.Float() );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CFlare::InputLaunch( inputdata_t &inputdata )
{
	Vector	direction;
	AngleVectors( GetAbsAngles(), &direction );

	Launch( direction, FLARE_LAUNCH_SPEED );
}

IMPLEMENT_SERVERCLASS_ST(CFlaregun, DT_Flaregun)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_flaregun, CFlaregun );
PRECACHE_WEAPON_REGISTER( weapon_flaregun );

//-----------------------------------------------------------------------------
// Purpose: Precache
//-----------------------------------------------------------------------------
void CFlaregun::Precache( void )
{
	BaseClass::Precache();

	UTIL_PrecacheOther( "env_flare" );
}

//-----------------------------------------------------------------------------
// Purpose: Main attack
//-----------------------------------------------------------------------------
void CFlaregun::PrimaryAttack( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	if ( m_iClip1 <= 0 )
	{
		SendWeaponAnim( ACT_VM_DRYFIRE );
		pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
		return;
	}

	m_iClip1 = m_iClip1 - 1;

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );
	pOwner->m_flNextAttack = gpGlobals->curtime + 1;

	CFlare *pFlare = CFlare::Create( pOwner->Weapon_ShootPosition(), pOwner->EyeAngles(), pOwner, FLARE_DURATION );

	if ( pFlare == NULL )
		return;

	Vector forward;
	pOwner->EyeVectors( &forward );

	pFlare->SetAbsVelocity( forward * 1500 );

	WeaponSound( SINGLE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CFlaregun::SecondaryAttack( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	
	if ( pOwner == NULL )
		return;

	if ( m_iClip1 <= 0 )
	{
		SendWeaponAnim( ACT_VM_DRYFIRE );
		pOwner->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
		return;
	}

	m_iClip1 = m_iClip1 - 1;

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );
	pOwner->m_flNextAttack = gpGlobals->curtime + 1;

	CFlare *pFlare = CFlare::Create( pOwner->Weapon_ShootPosition(), pOwner->EyeAngles(), pOwner, FLARE_DURATION );

	if ( pFlare == NULL )
		return;

	Vector forward;
	pOwner->EyeVectors( &forward );

	pFlare->SetAbsVelocity( forward * 500 );
	pFlare->SetGravity(1.0f);
	pFlare->SetFriction( 0.85f );
	pFlare->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );

	WeaponSound( SINGLE );
}

