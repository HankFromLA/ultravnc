/////////////////////////////////////////////////////////////////////////////
//  Copyright (C) 2002-2013 UltraVNC Team Members. All Rights Reserved.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
//  USA.
//
// If the source code for the program is not available from the place from
// which you received this file, check 
// http://www.uvnc.com/
//
////////////////////////////////////////////////////////////////////////////

// System headers
#include <assert.h>
#include "stdhdrs.h"

// Custom headers
//#include <WinAble.h>
#include <omnithread.h>
#include "winvnc.h"
#include "vnchooks/VNCHooks.h"
#include "vncserver.h"
#include "vnckeymap.h"
#include "rfbRegion.h"
#include "rfbRect.h"
#include "vncdesktop.h"
#include "vncservice.h"
// Modif rdv@2002 - v1.1.x - videodriver
#include "vncOSVersion.h"

#include "mmsystem.h" // sf@2002
#include "TextChat.h" // sf@2002
#include "vncdesktopthread.h"
#include "common/win32_helpers.h"

void 
vncDesktop::Checkmonitors()
{
	nr_monitors=GetNrMonitors();
	DEVMODE devMode;

	if (nr_monitors>0) {
		GetPrimaryDevice();
		devMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(mymonitor[0].device, ENUM_CURRENT_SETTINGS, &devMode);
		mymonitor[0].offsetx=devMode.dmPosition.x;
		mymonitor[0].offsety=devMode.dmPosition.y;
		mymonitor[0].Width=devMode.dmPelsWidth;
		mymonitor[0].Height=devMode.dmPelsHeight;
		mymonitor[0].Depth=devMode.dmBitsPerPel;
	}
	if (nr_monitors>1) {
		GetSecondaryDevice();
		devMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(mymonitor[1].device, ENUM_CURRENT_SETTINGS, &devMode);
		mymonitor[1].offsetx=devMode.dmPosition.x;
		mymonitor[1].offsety=devMode.dmPosition.y;
		mymonitor[1].Width=devMode.dmPelsWidth;
		mymonitor[1].Height=devMode.dmPelsHeight;
		mymonitor[1].Depth=devMode.dmBitsPerPel;
	}
	if (nr_monitors>2) {
		GetThirdDevice();
		devMode.dmSize = sizeof(DEVMODE);
		EnumDisplaySettings(mymonitor[2].device, ENUM_CURRENT_SETTINGS, &devMode);
		mymonitor[2].offsetx=devMode.dmPosition.x;
		mymonitor[2].offsety=devMode.dmPosition.y;
		mymonitor[2].Width=devMode.dmPelsWidth;
		mymonitor[2].Height=devMode.dmPelsHeight;
		mymonitor[2].Depth=devMode.dmBitsPerPel;
	}

	mymonitor[3].offsetx=GetSystemMetrics(SM_XVIRTUALSCREEN);
    mymonitor[3].offsety=GetSystemMetrics(SM_YVIRTUALSCREEN);
    mymonitor[3].Width=GetSystemMetrics(SM_CXVIRTUALSCREEN);
    mymonitor[3].Height=GetSystemMetrics(SM_CYVIRTUALSCREEN);
	mymonitor[3].Depth=mymonitor[0].Depth;//depth primary monitor is used
}



int
vncDesktop::GetNrMonitors()
{
	int i;
    int j=0;
    DISPLAY_DEVICE dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);
    for (i=0; EnumDisplayDevicesA(NULL, i, &dd, 0); i++) {
		if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
			j++;
	}
	return j;
}

void
vncDesktop::GetPrimaryDevice()
{
	int i;
    int j=0;
    DISPLAY_DEVICE dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);
    for (i=0; EnumDisplayDevicesA(NULL, i, &dd, 0); i++) {
		if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP && dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE 
				&& !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {
			strcpy(mymonitor[0].device,(char *)dd.DeviceName);
			break;
		}
	}
}

void
vncDesktop::GetSecondaryDevice()
{
	int i;
    int j=0;
    DISPLAY_DEVICE dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);
    for (i=0; EnumDisplayDevicesA(NULL, i, &dd, 0); i++) {
		if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP && !(dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
				&& !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {
			strcpy(mymonitor[1].device,(char *)dd.DeviceName);
			break;
			}
	}
}

void
vncDesktop::GetThirdDevice()
{
	int i;
    int j=0;
    DISPLAY_DEVICE dd;
    ZeroMemory(&dd, sizeof(dd));
    dd.cb = sizeof(dd);
    for (i=0; EnumDisplayDevicesA(NULL, i, &dd, 0); i++) {
		if (dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) {
			j++;
			if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) && !(dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)) {
				if (j == 3) {
					strcpy(mymonitor[2].device,(char *)dd.DeviceName);
					break;
				}
			}
		}
	}
}