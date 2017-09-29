#pragma once

class CFileBuffer
{
public:
	CFileBuffer(WCHAR* param); // 파일명으로 생성자 호출

	CFileBuffer() = default; // 디폴트 생성자
	~CFileBuffer(); // 나중에 파일 지우자

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