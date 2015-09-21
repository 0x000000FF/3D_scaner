#include <Windows.h>


	class comPort
	{
	public:
		int TimeOuts;                 //超时 500ms
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
// 功  能: 关闭 COM Port;//
// 返回值：= 0:  关闭成功//
//         = 其它值: 关闭失败//
//////////////////////////////////////////////////////////////////////////
	int CloseCom(HANDLE ComID)
	{
		if(ComID > 0) 
		if(CloseHandle(ComID))
			return 0;

		return -1;
	}

//////////////////////////////////////////////////////////////////////////
// 功  能: 初始化 COM Port//
// 参  数: ComPort  --  串口号//
//         BaudRate --  波特率//
// 返回值：>0 :  打开成功，返回串口的设备句柄//
//         = 其它值:  打开失败//
//////////////////////////////////////////////////////////////////////////
//HANDLE OpenCom(const char *ComPort,int BaudRate)
	HANDLE OpenCom(LPCSTR comm,int BaudRate)
	{
		_DCB FDCB;

		HANDLE ComID = NULL;

		ComID = CreateFileA(comm,
		GENERIC_READ | GENERIC_WRITE,			// 可读 可写
		NULL,									 // 独占方式
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
		FDCB.ByteSize = 8;            // 8 Bits 数据
		FDCB.StopBits = ONESTOPBIT;   // 一位停止位
		FDCB.Parity = 0;                //缺省无校验

		if(!SetCommState(ComID,&FDCB)) 
		return 0;

		if(!SetupComm(ComID,500,500)) 
			return 0;

		return ComID; 
	}

//////////////////////////////////////////////////////////////////////////
// 功  能: 设置超时//
// 参  数: TimeOut --  超时时间（单位：毫秒）//
//////////////////////////////////////////////////////////////////////////
	int SetTimeOuts(int TimeOut)
	{
		TimeOuts = TimeOut;
		return TimeOut;
	}

//////////////////////////////////////////////////////////////////////////
// 功  能: 设置校验方式//
// 参  数: Parity --  校验方式//
//         =0  无校验(no)//
//         =1  奇校验(odd)//
//         =2  偶校验(even)//
//         =3  标记校验(mark)//
//         =4  空格校验(space)//
// 返回值：=0     :  成功//
//         =其它值:  失败//
//////////////////////////////////////////////////////////////////////////
	int SetParity(HANDLE ComID,int Parity)
	{
		DCB FDCB;
		if(!GetCommState(ComID,&FDCB)) 
			return -1;

		FDCB.Parity = Parity;   //校验方式

		if(!SetCommState(ComID,&FDCB))
			return -1;

		return 0;
	}

//////////////////////////////////////////////////////////////////////////
// 功  能: 从串口读所有字节流//
// 参  数: 无//
// 返回值：> 0:  读取数据大小//
//         = -1: 失败//
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

			if(cs.cbInQue < ReadSize)//如果串口数据小于ReadSize
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
// 功  能: 向串口发送字节流.//
// 参  数: 无//
// 返回值：= 0:  成功//
//         = -1: 失败//
//////////////////////////////////////////////////////////////////////////
	BOOL SendToCom(DWORD SendSize)
	{
		DWORD Status;
		BOOL bResult;

		bResult = PurgeComm(H_comport,PURGE_TXCLEAR | PURGE_RXCLEAR); // 清除接收队列和发送队列
		if(!bResult) 
			return 0;

		bResult = WriteFile(H_comport,sendbuff,SendSize,&Status,0);
		if(!bResult) 
			return 0;

		return -1;
	}
};