#include "stdafx.h"
#include "classParser.h"

int main()
{
	wprintf(L"main의 시작\n");

	CFileBuffer fileOpening(L"kkk.txt");
	if (fileOpening.FindArea(L"APPLE") == false)
	{
		wprintf(L"분류 설정 실패\n");
	}
	else
	{
		wprintf(L"분류 설정 성공\n");

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
			wprintf(L"문자 읽기 실패!!\n");
			exit(1);
		}

		fileOpening.GetValue(L"ServerBindPort", &iValue4);
		fileOpening.GetValue(L"WorkerThread", &iValue5);
		fileOpening.GetValue(L"MaxUser", &iValue6);

		/////////////////////////////////////////////////////////////////
//		if (fileOpening.GetValue("MapCount", &iValue7) == false)
//		{
//			printf("맵카운트 실패!!\n");
//			exit(1);
//		}
		/////////////////////////////////////////////////////////////////
		wprintf(L"Version         : %d\n", iValue1);
		wprintf(L"ServerID        : %d\n", iValue2);

		wprintf(L"ServerBindIP    : %s\n", cValue3);
		wprintf(L"문자를 %d 만큼 읽었습니다!!\n", tempLen);

		wprintf(L"ServerBindPort  : %d\n", iValue4);
		wprintf(L"WorkerThread    : %d\n", iValue5);
		wprintf(L"MaxUser         : %.4f\n", iValue6);
		//		/////////////////////////////////////////////////////////////////
		//		printf("MapCount         : %d\n", iValue7);
		//		/////////////////////////////////////////////////////////////////
	}

	wprintf(L"\nmain의 끝\n");

	return 0;
}