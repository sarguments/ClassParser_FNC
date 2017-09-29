#include "stdafx.h"
#include "classParser.h"


/////////////////////// ���� ///////////////////////

// Ŭ������ ����� ���� �ҷ����� �޼��� ȣ���ؼ� ���� ���۷�
CFileBuffer::CFileBuffer(WCHAR* param) // ������
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
	BufferSize = fileSize; // ���� ������
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

bool CFileBuffer::FindArea(WCHAR* szName) // �񱳹���, �����Ұ�
{
	/*

	������ ��ġ
	m_iBufferAreaStart
	m_iBufferArearEnd
	m_iBufferFocusPos

	���� ������, ������ ���ϰ� �ű⼭�� �˻�

	bool AreaMode ( ��ŵ�޼��� �߰�ȣ�� ã�� )
	�������ϱ� ��� ������ �����ϰ� ã�´�

	*/

	WCHAR chWord[256];

	m_pBufferFocusPos = FileBuffer;

	for (int i = 0; i < BufferSize; i++) // : �� ��ġ���� ������ �̵�
	{
		if (*m_pBufferFocusPos == L':')
		{
			int iTemp = 0;
			m_pBufferFocusPos++;
			m_pBufferAreaStart = m_pBufferFocusPos; // area ���� ������ ����

			if (GetNextWord(&m_pBufferFocusPos, &iTemp) == true) //true
			{
				WCHAR * chpBuff = m_pBufferFocusPos;
				int iLength = iTemp;

				// Word ���ۿ� ã�� �ܾ �����Ѵ�.
				memset(chWord, 0, 256);
				memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

				// ���ڷ� �Է¹��� �ܾ�� ������ �˻��Ѵ�.
				if (0 == wcscmp(szName, chWord))
				{
					AreaMode = true;
					m_pBufferFocusPos += iLength + 2; //

					//�´ٸ� �ٷ� �ڿ� { �� ã��
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
			// �ܾ� �������� �޼���
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

				chpBuffer++; // ���̸� ���� �뵵
				tempLength++;
			}
			*chppBuffer = m_pBufferFocusPos; // �̵��� cpy������
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

				chpBuffer++; // ���̸� ���� �뵵
				tempLength++;
			}
			*chppBuffer = m_pBufferFocusPos; // �̵��� cpy������
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

bool CFileBuffer::GetValue(WCHAR* szName, WCHAR* ipValue, int* szSize) // �񱳹���, �����Ұ�, ���� ������
{
	WCHAR* chpBuff;
	WCHAR chWord[256];
	int iLength;
	// ã���� �ϴ� �ܾ ���ö����� ��� ã�� ���̹Ƿ� while������ �˻�

	while (GetNextWord(&chpBuff, &iLength) == true) // true
	{
		// Word ���ۿ� ã�� �ܾ �����Ѵ�.
		memset(chWord, 0, 256);
		memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

		// ���ڷ� �Է¹��� �ܾ�� ������ �˻��Ѵ�.
		if (0 == wcscmp(szName, chWord))
		{
			m_pBufferFocusPos += iLength;
			//�´ٸ� �ٷ� �ڿ� = �� ã��
			if (GetNextWord(&chpBuff, &iLength) == true) // true
			{
				memset(chWord, 0, 256);
				memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

				if (0 == wcscmp(L"=", chWord))
				{
					m_pBufferFocusPos += iLength;

					if (*(m_pBufferFocusPos + 1) == L'\"') // ���ڿ� �޾ƿ���
					{
						m_pBufferFocusPos += 2;

						if (iLength > *szSize)
						{
							return false; // ���� ���� ���� �ʰ�!
						}

						GetStringWord(&chpBuff, &iLength);

						memcpy_s(ipValue, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));
						m_pBufferFocusPos += iLength;
						*szSize = iLength; // ���ڷ� ���� ���� ������ �����Ϳ� ���� ���ڿ� ���� ����

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
	// ã���� �ϴ� �ܾ ���ö����� ��� ã�� ���̹Ƿ� while������ �˻�

	while (GetNextWord(&chpBuff, &iLength) == true) // true
	{
		if (iLength < 0)
		{
			wprintf(L"���� ����\n");
			return false;
		}

		// Word ���ۿ� ã�� �ܾ �����Ѵ�.
		memset(chWord, 0, 256);
		memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

		// ���ڷ� �Է¹��� �ܾ�� ������ �˻��Ѵ�.
		if (0 == wcscmp(szName, chWord))
		{
			m_pBufferFocusPos += iLength;
			//�´ٸ� �ٷ� �ڿ� = �� ã��
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
	// ã���� �ϴ� �ܾ ���ö����� ��� ã�� ���̹Ƿ� while������ �˻�

	while (GetNextWord(&chpBuff, &iLength) == true) // true
	{
		if (iLength < 0)
		{
			printf("���� ����\n");
			return false;
		}

		// Word ���ۿ� ã�� �ܾ �����Ѵ�.
		memset(chWord, 0, 256);
		memcpy_s(chWord, iLength * sizeof(WCHAR), chpBuff, iLength * sizeof(WCHAR));

		// ���ڷ� �Է¹��� �ܾ�� ������ �˻��Ѵ�.
		if (0 == wcscmp(szName, chWord))
		{
			m_pBufferFocusPos += iLength;
			//�´ٸ� �ٷ� �ڿ� = �� ã��
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

// ���� ã�� �޼���

// �ڸ�Ʈ�� �ƴϸ� ���ۿ��� Ư�� ���� �������� �޼��� ,�ڸ�Ʈ�� �ѱ��
	// �ϴ� �ܾ� �ϳ��� �����´�
		// ���� ������ �����ϰ� ���� ���� ������ �� ���� ������ ������Ű�鼭 ���� �����ؼ� �Ѱ��ش�.
// ������ �ܾ Ư�� �ܾ��̸� ���� �������� �ƴϸ� �����ܾ� �����´�.