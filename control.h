#include <Windows.h>


	class comPort
	{
	public:
		int TimeOuts;                 //��ʱ 500ms
		LPCSTR comportNum;
		HANDLE H_comport;
		char* sendbuff;
		char* recvbuff;

		comPort(LPCSTR comm,int b)
		{
			comportNum = comm;
			H_comport = OpenCom(comportNum,b);
			sendbuff = "";
			recvbuff = "";
		}

		~comPort()
		{
			CloseCom(H_comport);
		}
		


//////////////////////////////////////////////////////////////////////////
// ��  ��: �ر� COM Port;//
// ����ֵ��= 0:  �رճɹ�//
//         = ����ֵ: �ر�ʧ��//
//////////////////////////////////////////////////////////////////////////
	int CloseCom(HANDLE ComID)
	{
		if(ComID > 0) 
		if(CloseHandle(ComID))
			return 0;

		return -1;
	}

//////////////////////////////////////////////////////////////////////////
// ��  ��: ��ʼ�� COM Port//
// ��  ��: ComPort  --  ���ں�//
//         BaudRate --  ������//
// ����ֵ��>0 :  �򿪳ɹ������ش��ڵ��豸���//
//         = ����ֵ:  ��ʧ��//
//////////////////////////////////////////////////////////////////////////
//HANDLE OpenCom(const char *ComPort,int BaudRate)
	HANDLE OpenCom(LPCSTR comm,int BaudRate)
	{
		_DCB FDCB;

		HANDLE ComID = NULL;

		ComID = CreateFileA(comm,
		GENERIC_READ | GENERIC_WRITE,			// �ɶ� ��д
		NULL,									 // ��ռ��ʽ
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

		if(ComID == INVALID_HANDLE_VALUE)
			return 0;

		if(!GetCommState(ComID,&FDCB))
			return 0;

		TimeOuts = 500;
		FDCB.BaudRate = BaudRate;
		FDCB.ByteSize = 8;            // 8 Bits ����
		FDCB.StopBits = ONESTOPBIT;   // һλֹͣλ
		FDCB.Parity = 0;                //ȱʡ��У��

		if(!SetCommState(ComID,&FDCB)) 
		return 0;

		if(!SetupComm(ComID,500,500)) 
			return 0;

		return ComID; 
	}

//////////////////////////////////////////////////////////////////////////
// ��  ��: ���ó�ʱ//
// ��  ��: TimeOut --  ��ʱʱ�䣨��λ�����룩//
//////////////////////////////////////////////////////////////////////////
	int SetTimeOuts(int TimeOut)
	{
		TimeOuts = TimeOut;
		return TimeOut;
	}

//////////////////////////////////////////////////////////////////////////
// ��  ��: ����У�鷽ʽ//
// ��  ��: Parity --  У�鷽ʽ//
//         =0  ��У��(no)//
//         =1  ��У��(odd)//
//         =2  żУ��(even)//
//         =3  ���У��(mark)//
//         =4  �ո�У��(space)//
// ����ֵ��=0     :  �ɹ�//
//         =����ֵ:  ʧ��//
//////////////////////////////////////////////////////////////////////////
	int SetParity(HANDLE ComID,int Parity)
	{
		DCB FDCB;
		if(!GetCommState(ComID,&FDCB)) 
			return -1;

		FDCB.Parity = Parity;   //У�鷽ʽ

		if(!SetCommState(ComID,&FDCB))
			return -1;

		return 0;
	}

//////////////////////////////////////////////////////////////////////////
// ��  ��: �Ӵ��ڶ������ֽ���//
// ��  ��: ��//
// ����ֵ��> 0:  ��ȡ���ݴ�С//
//         = -1: ʧ��//
//////////////////////////////////////////////////////////////////////////
	int ReadFromCom(DWORD ReadSize)
	{
		DWORD TempSize;
		DWORD EndTime,ErrorFlag;
		COMSTAT cs;
		BOOL bResult;

		EndTime = GetTickCount() + TimeOuts;
		while(GetTickCount() <= EndTime)
		{
			bResult = ClearCommError(H_comport,&ErrorFlag,&cs);
			if(!bResult) 
				return 0;

			if(cs.cbInQue < ReadSize)//�����������С��ReadSize
			{
				bResult = ReadFile(H_comport,recvbuff,cs.cbInQue,&TempSize,NULL) ;
				if(!bResult) 
					return 0;
			}
			else
			{
				bResult = ReadFile(H_comport,recvbuff,ReadSize,&TempSize,NULL);
				if(!bResult)
					return 0;
			}

			if(TempSize > 0)
				return TempSize;
		}

		return -1;
	}

//////////////////////////////////////////////////////////////////////////
// ��  ��: �򴮿ڷ����ֽ���.//
// ��  ��: ��//
// ����ֵ��= 0:  �ɹ�//
//         = -1: ʧ��//
//////////////////////////////////////////////////////////////////////////
	BOOL SendToCom(DWORD SendSize)
	{
		DWORD Status;
		BOOL bResult;

		bResult = PurgeComm(H_comport,PURGE_TXCLEAR | PURGE_RXCLEAR); // ������ն��кͷ��Ͷ���
		if(!bResult) 
			return 0;

		bResult = WriteFile(H_comport,sendbuff,SendSize,&Status,0);
		if(!bResult) 
			return 0;

		return -1;
	}
};