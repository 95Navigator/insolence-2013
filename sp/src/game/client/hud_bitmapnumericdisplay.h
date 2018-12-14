//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HUD_BITMAPNUMERICDISPLAY_H
#define HUD_BITMAPNUMERICDISPLAY_H
#ifdef _WIN32
#pragma once
#endif

#include "hud_numericdisplay.h"

class CHudBitmapNumericDisplay : public vgui::Panel
{	
	DECLARE_CLASS_SIMPLE( CHudBitmapNumericDisplay, vgui::Panel );

public:
	CHudBitmapNumericDisplay(vgui::Panel *parent, const char *name);

	void SetDisplayValue(int value);
	void SetSecondaryValue(int value);
	void SetShouldDisplayValue(bool state);
	void SetShouldDisplaySecondaryValue(bool state);
	void SetLabelIcon(const char *a);
	void SetShouldDisplayLabelIcon(bool state);
	void SetNBackgroundNumbers(int n);
	void SetNBackgroundSecondaryNumbers(int n);

	virtual void Reset();

protected:
	// vgui overrides
	virtual void PaintBackground( void );
	virtual void Paint();
	virtual void PaintLabel(int xpos, int ypos, Color col);

	void PaintNumbers(int xpos, int ypos, int value, Color col, int numSigDigits);
	virtual void PaintNumbers(int xpos, int ypos, int value, Color col)
	{
		PaintNumbers(xpos, ypos, value, col, 1);
	}
	void PaintSecondaryNumbers(int xpos, int ypos, int value, Color col, int numSigDigits);
	virtual void PaintSecondaryNumbers(int xpos, int ypos, int value, Color col)
	{
		PaintSecondaryNumbers(xpos, ypos, value, col, 1);
	}
	void PaintBackgroundNumbers(int xpos, int ypos, Color col, int numdigits);
	void PaintBackgroundSecondaryNumbers(int xpos, int ypos, Color col, int numdigits);

	CPanelAnimationVar( float, m_flAlphaOverride, "Alpha", "255" );
	CPanelAnimationVar( Color, m_TextColor, "TextColor", "FgColor" );
	CPanelAnimationVar( float, m_flBlur, "Blur", "0" );

	CPanelAnimationVarAliasType( float, digit_xpos, "digit_xpos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit_ypos, "digit_ypos", "0", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit_height, "digit_height", "16", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit2_xpos, "digit2_xpos", "98", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit2_ypos, "digit2_ypos", "16", "proportional_float" );
	CPanelAnimationVarAliasType( float, digit2_height, "digit2_height", "8", "proportional_float" );

private:

	CHudTexture *m_pNumbers[10];
	CHudTexture *m_pSecondaryNumbers[10];
	CHudTexture *m_pBackgroundNumbers[2];
	CHudTexture *m_pBackgroundSecondaryNumbers[2];
	CHudTexture *m_pLabel;

	int m_iValue;
	int m_iSecondaryValue;
	char m_LabelIcon[16];
	bool m_bDisplayValue, m_bDisplaySecondaryValue, m_bDisplayLabelIcon;
	int m_iNBackgroundNumbers;
	int m_iNBackgroundSecondaryNumbers;
};

#endif //HUD_BITMAPNUMERICDISPLAY_H