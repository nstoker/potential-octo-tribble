// DetectHWDevices.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>

int main(int argc, char *argv[], char *envp[])
{
	HDEVINFO hDevInfo;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i;
	PCWSTR devInst = L"USB\\VID_0547&PID_1004\\0123456789ABCDEF";
	printf("Starting up\n");
	// Interested in this device class guid {4d36e978-e325-11ce-bfc1-08002be10318}
	// Hardware Ids USB\VID_0547&PID_1004&REV_0002
	//              USB\VID_0547&PID_1004

	// Create a HDEVINFO with all present devices.
	hDevInfo = SetupDiGetClassDevs(NULL,
		devInst, // Enumerator
		0,
		DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (hDevInfo == INVALID_HANDLE_VALUE){
		// Insert error handling here.
		printf("No handle obtained from SetupDiGetClassDevs \n");
		return 1;
	}

	// Enumerate through all devices in Set.
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i,
		&DeviceInfoData); i++)
	{
		DWORD DataT;
		HLOCAL buffer = NULL;
		DWORD buffersize = 0;
		printf("index %i\t", i);

		// Attempt to get DeviceInterfaces using http://msdn.microsoft.com/en-gb/library/windows/hardware/ff551015(v=vs.85).aspx

		//
		// Call function with null to begin with,
		// then use the returned buffer size (doubled)
		// to Alloc the buffer. Keep calling until
		// success or an unknown failure.
		//
		//  Double the returned buffersize to correct
		//  for underlying legacy CM functions that
		//  return an incorrect buffersize value on
		//  DBCS/MBCS systems.
		//
		while (!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&DeviceInfoData,
			SPDRP_DEVICEDESC,
			&DataT,
			(PBYTE)buffer,
			buffersize,
			&buffersize))
		{
			if (GetLastError() ==
				ERROR_INSUFFICIENT_BUFFER)
			{
				// Change the buffer size.
				if (buffer) LocalFree(buffer);
				// Double the size to avoid problems on
				// W2k MBCS systems per KB 888609.
				buffer = LocalAlloc(LPTR, buffersize * 2);
			}
			else
			{
				// Insert error handling here.
				printf("\nLast error: ", GetLastError());
				break;
			}
		}



		printf("Result:[%s]\n", buffer);

		if (buffer) LocalFree(buffer);
	}


	if (GetLastError() != NO_ERROR &&
		GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		// Insert error handling here.
		return 1;
	}


	printf("\nCleaning up\n");
	//  Cleanup
	SetupDiDestroyDeviceInfoList(hDevInfo);

	return 0;
}

/* WAS

// Enumerate through all devices in Set.





//  Cleanup
SetupDiDestroyDeviceInfoList(hDevInfo);

return 0;
}

*/