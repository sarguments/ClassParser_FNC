#pragma once

class CFileBuffer
{
public:
	CFileBuffer(WCHAR* param); // ���ϸ����� ������ ȣ��

	CFileBuffer() = default; // ����Ʈ ������
	~CFileBuffer(); // ���߿� ���� ������

	bool FindArea(WCHAR* szName);
	bool GetValue(WCHAR* szName, int* ipValue);
	bool GetValue(WCHAR* szName, WCHAR* ipValue, int* szSize);
	bool GetValue(WCHAR* szName, float* ipValue);

private:
	bool GetStringWord(WCHAR** chppBuffer, int* ipLength) const;
	bool GetFloatWord(WCHAR** chppBuffer, int* ipLength);
	bool GetNextWord(WCHAR** chppBuffer, int * ipLength);
	bool SkipNoneCommand();
	bool RemoveComment();

	WCHAR* FileBuffer;
	int BufferSize;

	WCHAR* m_pBufferAreaStart;
	WCHAR* m_pBufferArearEnd;
	WCHAR* m_pBufferFocusPos;

	bool AreaMode;
};