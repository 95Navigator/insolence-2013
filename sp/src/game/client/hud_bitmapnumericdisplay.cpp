//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud_bitmapnumericdisplay.h"
#include "iclientmode.h"

#include <Color.h>
#include <KeyValues.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CHudBitmapNumericDisplay::CHudBitmapNumericDisplay(vgui::Panel *parent, const char *name) : vgui::Panel(parent, name)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_iValue = 0;
	m_iSecondaryValue = 0;
	m_bDisplayValue = true;
	m_bDisplaySecondaryValue = false;
	memset( m_pNumbers, 0, 10*sizeof(CHudTexture *) );
	memset( m_pSecondaryNumbers, 0, 10*sizeof(CHudTexture *) );
	memset( m_pBackgroundNumbers, 0, 2*sizeof(CHudTexture *) );
	memset( m_pBackgroundSecondaryNumbers, 0, 2*sizeof(CHudTexture *) );
}

//-----------------------------------------------------------------------------
// Purpose: Resets values on restore/new map
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::Reset()
{
	m_flBlur = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetDisplayValue(int value)
{
	m_iValue = value;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetSecondaryValue(int value)
{
	m_iSecondaryValue = value;
}

//-----------------------------------------------------------------------------
// Purpose: renders the vgui panel
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::Paint()
{
	float alpha = m_flAlphaOverride / 255;
	Color fgColor = GetFgColor();
	fgColor[3] *= alpha;
	SetFgColor( fgColor );

	if (m_bDisplayValue)
	{
		// draw our background numbers
		PaintBackgroundNumbers(digit_xpos, digit_ypos, GetFgColor(), b_digit_n);

		// draw our numbers
		PaintNumbers(digit_xpos, digit_ypos, m_iValue, GetFgColor());

		// draw the overbright blur
		for (float fl = m_flBlur; fl > 0.0f; fl -= 1.0f)
		{
			if (fl >= 1.0f)
			{
				PaintNumbers(digit_xpos, digit_ypos, m_iValue, GetFgColor());
			}
			else
			{
				// draw a percentage of the last one
				Color col = GetFgColor();
				col[3] *= fl;
				PaintNumbers(digit_xpos, digit_ypos, m_iValue, col);
			}
		}
	}

	// total ammo
	if (m_bDisplaySecondaryValue)
	{
		PaintBackgroundSecondaryNumbers(digit2_xpos, digit2_ypos, GetFgColor(), b_digit2_n);
		PaintSecondaryNumbers(digit2_xpos, digit2_ypos, m_iSecondaryValue, GetFgColor());
	}
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetShouldDisplayValue(bool state)
{
	m_bDisplayValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::SetShouldDisplaySecondaryValue(bool state)
{
	m_bDisplaySecondaryValue = state;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintBackground( void )
{
	int alpha = m_flAlphaOverride / 255;
	Color bgColor = GetBgColor();
	bgColor[3] *= alpha;
	SetBgColor( bgColor );

	BaseClass::PaintBackground();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintNumbers(int xpos, int ypos, int value, Color col, int numSigDigits )
{
	if( !m_pNumbers[0] )
	{
		int i;
		char a[16];

		for( i=0;i<10;i++ )
		{
			sprintf( a, "number_%d", i );

			m_pNumbers[i] = gHUD.GetIcon( a );
		}

		if( !m_pNumbers[0] )
			return;
	}

	if( value > 100000 )
	{
		value = 99999;
	}

	int pos = 10000;

	float scale = ( digit_height / (float)m_pNumbers[0]->Height());

	int digit;
	Color color = GetFgColor();
	int width = m_pNumbers[0]->Width() * scale;
	int height = m_pNumbers[0]->Height() * scale;
	bool bStart = false;

	//right align to xpos

	int numdigits = 1;

	int x = pos;
	while( x >= 10 )
	{
		if( value >= x )
			numdigits++;

		x /= 10;
	}

	if( numdigits < numSigDigits )
		numdigits = numSigDigits;

	xpos -= numdigits * width;

	//draw the digits
	while( pos >= 1 )
	{
		digit = value / pos;
		value = value % pos;
		
		if( bStart || digit > 0 || pos <= pow(10.0f,numSigDigits-1) )
		{
			bStart = true;
			m_pNumbers[digit]->DrawSelf( xpos, ypos, width, height, col );
			xpos += width;
		}		

		pos /= 10;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintSecondaryNumbers(int xpos, int ypos, int value, Color col, int numSigDigits )
{
	if( !m_pSecondaryNumbers[0] )
	{
		int i;
		char a[16];

		for( i=0;i<10;i++ )
		{
			sprintf( a, "sec_number_%d", i );

			m_pSecondaryNumbers[i] = gHUD.GetIcon( a );
		}

		if( !m_pSecondaryNumbers[0] )
			return;
	}

	if( value > 100000 )
	{
		value = 99999;
	}

	int pos = 10000;

	float scale = ( digit2_height / (float)m_pSecondaryNumbers[0]->Height());

	int digit;
	Color color = GetFgColor();
	int width = m_pSecondaryNumbers[0]->Width() * scale;
	int height = m_pSecondaryNumbers[0]->Height() * scale;
	bool bStart = false;

	//right align to xpos

	int numdigits = 1;

	int x = pos;
	while( x >= 10 )
	{
		if( value >= x )
			numdigits++;

		x /= 10;
	}

	if( numdigits < numSigDigits )
		numdigits = numSigDigits;

	xpos -= numdigits * width;

	//draw the digits
	while( pos >= 1 )
	{
		digit = value / pos;
		value = value % pos;
		
		if( bStart || digit > 0 || pos <= pow(10.0f,numSigDigits-1) )
		{
			bStart = true;
			m_pSecondaryNumbers[digit]->DrawSelf( xpos, ypos, width, height, col );
			xpos += width;
		}		

		pos /= 10;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintBackgroundNumbers(int xpos, int ypos, Color col, int numdigits )
{
	if( !m_pBackgroundNumbers[0] )
	{
		int i;
		char a[16];

		for( i=0;i<2;i++ )
		{
			sprintf( a, "b_number_%d", i );

			m_pBackgroundNumbers[i] = gHUD.GetIcon( a );
		}

		if( !m_pBackgroundNumbers[0] )
			return;
	}

	float scale = ( digit_height / (float)m_pBackgroundNumbers[0]->Height());

	int digit = 0;
	Color color = GetFgColor();
	int width = m_pBackgroundNumbers[0]->Width() * scale;
	int height = m_pBackgroundNumbers[0]->Height() * scale;
	int i;

	//right align to xpos

	xpos -= numdigits * width;

	//draw the digits
	for ( i=0;i<numdigits;i++ )
	{
		m_pBackgroundNumbers[digit]->DrawSelf( xpos, ypos, width, height, col );
		xpos += width;

		digit = !digit;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBitmapNumericDisplay::PaintBackgroundSecondaryNumbers(int xpos, int ypos, Color col, int numdigits )
{
	if( !m_pBackgroundSecondaryNumbers[0] )
	{
		int i;
		char a[16];

		for( i=0;i<2;i++ )
		{
			sprintf( a, "b_sec_number_%d", i );

			m_pBackgroundSecondaryNumbers[i] = gHUD.GetIcon( a );
		}

		if( !m_pBackgroundSecondaryNumbers[0] )
			return;
	}

	float scale = ( digit2_height / (float)m_pBackgroundSecondaryNumbers[0]->Height());

	int digit = 0;
	Color color = GetFgColor();
	int width = m_pBackgroundSecondaryNumbers[0]->Width() * scale;
	int height = m_pBackgroundSecondaryNumbers[0]->Height() * scale;
	int i;

	//right align to xpos

	xpos -= numdigits * width;

	//draw the digits
	for ( i=0;i<numdigits;i++ ) 
	{
		m_pBackgroundSecondaryNumbers[digit]->DrawSelf( xpos, ypos, width, height, col );
		xpos += width;

		digit = !digit;
	}
}