#include "stdafx.h"
#include "classParser.h"

int main()
{
	wprintf(L"main�� ����\n");

	CFileBuffer fileOpening(L"kkk.txt");
	if (fileOpening.FindArea(L"APPLE") == false)
	{
		wprintf(L"�з� ���� ����\n");
	}
	else
	{
		wprintf(L"�з� ���� ����\n");

		int iValue1;
		int iValue2;
		WCHAR cValue3[256] = { 0, };
		int iValue4;
		int iValue5;
		float iValue6;
		/////////////////////////////////////////////////////////////////
//		int iValue7;
		/////////////////////////////////////////////////////////////////

		fileOpening.GetValue(L"Version", &iValue1);
		fileOpening.GetValue(L"ServerID", &iValue2);

		WCHAR* temp = L"ServerBindIP";
		int tempLen = wcslen(temp);
		if (fileOpening.GetValue(temp, cValue3, &tempLen) == false)
		{
			wprintf(L"���� �б� ����!!\n");
			exit(1);
		}

		fileOpening.GetValue(L"ServerBindPort", &iValue4);
		fileOpening.GetValue(L"WorkerThread", &iValue5);
		fileOpening.GetValue(L"MaxUser", &iValue6);

		/////////////////////////////////////////////////////////////////
//		if (fileOpening.GetValue("MapCount", &iValue7) == false)
//		{
//			printf("��ī��Ʈ ����!!\n");
//			exit(1);
//		}
		/////////////////////////////////////////////////////////////////
		wprintf(L"Version         : %d\n", iValue1);
		wprintf(L"ServerID        : %d\n", iValue2);

		wprintf(L"ServerBindIP    : %s\n", cValue3);
		wprintf(L"���ڸ� %d ��ŭ �о����ϴ�!!\n", tempLen);

		wprintf(L"ServerBindPort  : %d\n", iValue4);
		wprintf(L"WorkerThread    : %d\n", iValue5);
		wprintf(L"MaxUser         : %.4f\n", iValue6);
		//		/////////////////////////////////////////////////////////////////
		//		printf("MapCount         : %d\n", iValue7);
		//		/////////////////////////////////////////////////////////////////
	}

	wprintf(L"\nmain�� ��\n");

	return 0;
}