#include "stdafx.h"
#include "classParser.h"


/////////////////////// 로직 ///////////////////////

// 클래스를 만들고 파일 불러오는 메서드 호출해서 파일 버퍼로
CFileBuffer::CFileBuffer(WCHAR* param) // 생성자
{
	_wsetlocale(LC_ALL, L"korean");

	FILE* fp;
	errno_t err = _wfopen_s(&fp, param, L"rb, ccs=UNICODE");
	if (err != 0)
	{
		wprintf(L"File Open Error\n");
	}

	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	BufferSize = fileSize; // 버퍼 사이즈
	fseek(fp, 0, SEEK_SET);

	FileBuffer = (WCHAR*)malloc(fileSize + 2);
	fread_s(FileBuffer, fileSize, fileSize, 1, fp);
	FileBuffer[fileSize + 1] = 0;
	FileBuffer[fileSize] = 0;

	m_pBufferArearEnd = FileBuffer + BufferSize;

	fclose(fp);
}

CFileBuffer::~CFileBuffer()
{
	free(FileBuffer);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFileBuffer::FindArea(WCHAR* szName) // 비교문자, 저장할곳
{
	/*

	버퍼의 위치
	m_iBufferAreaStart
	m_iBufferArearEnd
	m_iBufferFocusPos

	범위 시작점, 끝점을 정하고 거기서만 검색

	bool AreaMode ( 스킵메서드 중괄호만 찾게 )
	구역정하기 모드 설정시 무시하고 찾는다

	*/

	WCHAR chWord[256];

	m_pBufferFocusPos = FileBuffer;

	for (int i = 0; i < BufferSize; i++) // : 의 위치까지 포인터 이동
	{
		if (*m_pBufferFocusPos == L':')
		{
			int iTemp = 0;
			m_pBufferFocusPos++;
			m_pBufferAreaStart = m_pBufferFocusPos; // area 시작 포인터 설정

			if (GetNextWord(&m_pBufferFocusPos, &iTemp) == true) //true
			{
				WCHAR * chpBuff = m_pBufferFocusPos;
				int iLength = iTemp;

				// Word 버퍼에 찾은 단어를 저장한다.
				memset(chWord, 0, 256);
				memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

				// 인자로 입력받은 단어와 같은지 검사한다.
				if (0 == wcscmp(szName, chWord))
				{
					AreaMode = true;
					m_pBufferFocusPos += iLength + 2; //

					//맞다면 바로 뒤에 { 을 찾자
					if (GetNextWord(&chpBuff, &iLength) == true) //true
					{
						memset(chWord, 0, 256);
						memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

						if (0 == wcscmp(L"{", chWord))
						{
							m_pBufferFocusPos += 3;
							m_pBufferAreaStart = m_pBufferFocusPos;

							while (*m_pBufferFocusPos != L'}')
							{
								m_pBufferFocusPos++;
							}

							m_pBufferArearEnd = (m_pBufferFocusPos - 2);
							m_pBufferFocusPos = m_pBufferAreaStart;
							AreaMode = false;

							return true; //true
						}
					}
					return false;
				}
				m_pBufferFocusPos += iLength;
			}
			// 단어 가져오는 메서드
		}
		m_pBufferFocusPos++;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFileBuffer::GetNextWord(WCHAR** chppBuffer, int* ipLength)
{
	int tempLength = 0;

	while (true)
	{
		WCHAR * chpBuffer = m_pBufferFocusPos;

		if (chpBuffer == m_pBufferArearEnd)
		{
			return false;
		}

		if (*chpBuffer != L'.' && *chpBuffer != L'"' &&
			*chpBuffer != 0x20 && *chpBuffer != 0x08 &&
			*chpBuffer != 0x09 && *chpBuffer != 0x0a &&
			*chpBuffer != 0x0d)
		{
			while (true)
			{
				if (*chpBuffer == L'.' || *chpBuffer == L'"' ||
					*chpBuffer == 0x20 || *chpBuffer == 0x08 ||
					*chpBuffer == 0x09 || *chpBuffer == 0x0a ||
					*chpBuffer == 0x0d)
				{
					break;
				}

				chpBuffer++; // 길이만 세는 용도
				tempLength++;
			}
			*chppBuffer = m_pBufferFocusPos; // 이동한 cpy포인터
			*ipLength = tempLength;

			return true; //true
		}

		if (*chpBuffer == 0)
		{
			return false;
		}

		if (*chpBuffer == L'/' && *(chpBuffer + 1) == L'*')
		{
			m_pBufferFocusPos += 3;
			if (RemoveComment() == false)
			{
				return false;
			}
		}
		else
		{
			if (SkipNoneCommand() == false)
			{
				return false;
			}
		}
	}
}

bool CFileBuffer::SkipNoneCommand()
{
	WCHAR* chpBuffer = m_pBufferFocusPos;

	while (true)
	{
		if (chpBuffer == m_pBufferArearEnd)
		{
			return false;
		}

		if (*chpBuffer == L'.' || *chpBuffer == L'"' ||
			*chpBuffer == 0x20 || *chpBuffer == 0x08 ||
			*chpBuffer == 0x09 || *chpBuffer == 0x0a ||
			*chpBuffer == 0x0d)
		{
			chpBuffer++;
			continue;
		}

		if (*chpBuffer == 0)
		{
			return false;
		}

		if (*chpBuffer == L'/' && *(chpBuffer + 1) == L'/')
		{
			chpBuffer += 2;
			continue;
		}

		m_pBufferFocusPos = chpBuffer;
		break;
	}

	return true; //true
}

bool CFileBuffer::RemoveComment()
{
	WCHAR* chpBuffer = m_pBufferFocusPos;

	while (true)
	{
		if (chpBuffer == m_pBufferArearEnd)
		{
			return false;
		}

		if (*chpBuffer == L'*' && *(chpBuffer + 1) == L'/')
		{
			m_pBufferFocusPos = chpBuffer;
			return true; //true
		}

		if (*chpBuffer == 0)
		{
			return false;
		}

		chpBuffer++;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFileBuffer::GetStringWord(WCHAR** chppBuffer, int* ipLength) const
{
	WCHAR* chpBuffer = m_pBufferFocusPos;
	int tempLength = 0;

	while (true)
	{
		if (chpBuffer == m_pBufferArearEnd)
		{
			return false;
		}

		if (*chpBuffer == L'\"')
		{
			*chppBuffer = m_pBufferFocusPos;
			*ipLength = tempLength;

			return true; //true
		}

		if (*chpBuffer == 0)
		{
			return false;
		}

		chpBuffer++;
		tempLength++;
	}
}

bool CFileBuffer::GetFloatWord(WCHAR ** chppBuffer, int * ipLength)
{
	int tempLength = 0;

	while (true)
	{
		WCHAR * chpBuffer = m_pBufferFocusPos;

		if (chpBuffer == m_pBufferArearEnd)
		{
			return false;
		}

		if (*chpBuffer != L'"' &&
			*chpBuffer != 0x20 && *chpBuffer != 0x08 &&
			*chpBuffer != 0x09 && *chpBuffer != 0x0a &&
			*chpBuffer != 0x0d)
		{
			while (true)
			{
				if (*chpBuffer == L'"' ||
					*chpBuffer == 0x20 || *chpBuffer == 0x08 ||
					*chpBuffer == 0x09 || *chpBuffer == 0x0a ||
					*chpBuffer == 0x0d)
				{
					break;
				}

				chpBuffer++; // 길이만 세는 용도
				tempLength++;
			}
			*chppBuffer = m_pBufferFocusPos; // 이동한 cpy포인터
			*ipLength = tempLength;

			return true; //true
		}

		if (*chpBuffer == 0)
		{
			return false;
		}

		if (*chpBuffer == L'/' && *(chpBuffer + 1) == L'*')
		{
			m_pBufferFocusPos += 3;
			if (RemoveComment() == false)
			{
				return false;
			}
		}
		else
		{
			if (SkipNoneCommand() == false)
			{
				return false;
			}
		}
	}
}

bool CFileBuffer::GetValue(WCHAR* szName, WCHAR* ipValue, int* szSize) // 비교문자, 저장할곳, 문자 사이즈
{
	WCHAR* chpBuff;
	WCHAR chWord[256];
	int iLength;
	// 찾고자 하는 단어가 나올때까지 계속 찾을 것이므로 while문으로 검사

	while (GetNextWord(&chpBuff, &iLength) == true) // true
	{
		// Word 버퍼에 찾은 단어를 저장한다.
		memset(chWord, 0, 256);
		memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

		// 인자로 입력받은 단어와 같은지 검사한다.
		if (0 == wcscmp(szName, chWord))
		{
			m_pBufferFocusPos += iLength;
			//맞다면 바로 뒤에 = 을 찾자
			if (GetNextWord(&chpBuff, &iLength) == true) // true
			{
				memset(chWord, 0, 256);
				memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

				if (0 == wcscmp(L"=", chWord))
				{
					m_pBufferFocusPos += iLength;

					if (*(m_pBufferFocusPos + 1) == L'\"') // 문자열 받아오기
					{
						m_pBufferFocusPos += 2;

						if (iLength > *szSize)
						{
							return false; // 문자 버퍼 길이 초과!
						}

						GetStringWord(&chpBuff, &iLength);

						memcpy_s(ipValue, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));
						m_pBufferFocusPos += iLength;
						*szSize = iLength; // 인자로 받은 문자 사이즈 포인터에 읽은 문자열 길이 저장

						return true; //true
					}
					return false;
				}
			}
			return false;
		}
		m_pBufferFocusPos += iLength;
	}
	return false;
}

bool CFileBuffer::GetValue(WCHAR * szName, float * ipValue)
{
	WCHAR* chpBuff;
	WCHAR chWord[256];
	int iLength;
	// 찾고자 하는 단어가 나올때까지 계속 찾을 것이므로 while문으로 검사

	while (GetNextWord(&chpBuff, &iLength) == true) // true
	{
		if (iLength < 0)
		{
			wprintf(L"길이 에러\n");
			return false;
		}

		// Word 버퍼에 찾은 단어를 저장한다.
		memset(chWord, 0, 256);
		memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

		// 인자로 입력받은 단어와 같은지 검사한다.
		if (0 == wcscmp(szName, chWord))
		{
			m_pBufferFocusPos += iLength;
			//맞다면 바로 뒤에 = 을 찾자
			if (GetNextWord(&chpBuff, &iLength) == true) // true
			{
				memset(chWord, 0, 256);
				memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

				if (0 == wcscmp(L"=", chWord))
				{
					m_pBufferFocusPos += iLength;

					if (GetFloatWord(&chpBuff, &iLength) == true) // true
					{
						memset(chWord, 0, 256);
						memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));
						*ipValue = (float)_wtof(chWord); // float??
						m_pBufferFocusPos += iLength;

						return true; //true
					}
					return false;
				}
			}
			return false;
		}
		m_pBufferFocusPos += iLength;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CFileBuffer::GetValue(WCHAR* szName, int* ipValue)
{
	WCHAR* chpBuff;
	WCHAR chWord[256];
	int iLength;
	// 찾고자 하는 단어가 나올때까지 계속 찾을 것이므로 while문으로 검사

	while (GetNextWord(&chpBuff, &iLength) == true) // true
	{
		if (iLength < 0)
		{
			printf("길이 에러\n");
			return false;
		}

		// Word 버퍼에 찾은 단어를 저장한다.
		memset(chWord, 0, 256);
		memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

		// 인자로 입력받은 단어와 같은지 검사한다.
		if (0 == wcscmp(szName, chWord))
		{
			m_pBufferFocusPos += iLength;
			//맞다면 바로 뒤에 = 을 찾자
			if (GetNextWord(&chpBuff, &iLength) == true) // true
			{
				memset(chWord, 0, 256);
				memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

				if (0 == wcscmp(L"=", chWord))
				{
					m_pBufferFocusPos += iLength;

					if (GetNextWord(&chpBuff, &iLength) == true) // true
					{
						memset(chWord, 0, 256);
						memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));
						*ipValue = _wtoi(chWord);
						m_pBufferFocusPos += iLength;

						return true; //true
					}
					return false;
				}
			}
			return false;
		}
		m_pBufferFocusPos += iLength;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 구역 찾기 메서드

// 코멘트가 아니면 버퍼에서 특정 값을 가져오는 메서드 ,코멘트면 넘긴다
	// 일단 단어 하나를 가져온다
		// 원본 포인터 저장하고 구분 문자 등장할 때 까지 포인터 증가시키면서 길이 측정해서 넘겨준다.
// 가져온 단어가 특정 단어이면 값을 가져오고 아니면 다음단어 가져온다.