#include "iiotap.h"
#define PACKET_SIZE 1000

#pragma comment(lib, "ws2_32.lib")  //加载 ws2_32.dll

#define USER_MSG_SOCKET_REC           (WM_USER + 0x01)

#define PACKET_SIZE	1000


Iiotap_Head head;
Iiotap_reg_payLoad reg_body;
iiot_packet packet;
Iiotap_DeviceListResp_payLoad Device_body;
Iiotap_ReportDeviceStatusNotif_payLoad RepDevStat;
Iiotap_ReportDataNotif_payLoad ReData;
Iiotap_Batch_ReportDataNotif_payLoad ReBatchData;
uint8_t data[PACKET_SIZE];

/*
uint8_t Scratch[] = {0x00,
0x49,0x49,0x4f ,0x54 ,0x41 ,0x50,
0x01,
0x74, 0x65, 0x73, 0x74, 0x6c, 0x6f, 0x74, 0x61, 0x70,
0x74, 0x65, 0x73, 0x74, 0x31, 0x32, 0x033};           
 */

int main()
{
	/*
	ieee_float_union union_value;
	union_value.ieee_float_value = 23.4f;
	ieee_float value;
	getIEEE_floatencode(&union_value, &value);

	uint8_t t[2] = { 0xf1,0x01 };
	uint16_t idx = 0;
	printf("%02x",getVintValue(t, &idx));

	*/
	uint16_t i = 0;
	char szBuffer[MAXBYTE] = {0};
	WSADATA wsaData;
	SOCKET sock;
	SOCKADDR_IN sockAddr;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	//创建套接字
	sock=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	memset(&sockAddr,0,sizeof(sockAddr));


	//定义结构体
/*
	sockAddr.sin_family=PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("192.168.1.198");
	sockAddr.sin_port=htons(8223);				   
*/

	sockAddr.sin_family=PF_INET;
	sockAddr.sin_addr.s_addr = inet_addr("120.27.27.151");
	sockAddr.sin_port=htons(17777);		

	connect(sock,(SOCKADDR*)&sockAddr,sizeof(SOCKADDR));
	packet.data=data;
	
	memset(packet.data,0,sizeof(packet.data));
	memset(&head, 0, sizeof(head));
	//发送注册消息
	SendRegisterReq(&packet, &head,&reg_body);
	SendtoServer(sock,(char *)packet.data,packet.size);


/*
	//打印包	
//	getpacketchar(&packet);

	//发送保持消息
	memset(packet.data, 0, sizeof(packet.data));
	memset(&head, 0, sizeof(head));
	SendKeepLink(&packet, &head);
	SendtoServer(sock, (char *)packet.data, packet.size);
	*/
	/*
	
	//相应查询
	memset(packet.data,0,sizeof(packet.data));
	memset(&head,0,sizeof(head));
	memset(&Device_body,0,sizeof(Device_body));
	GetDeviceListResp(&packet,&head,&Device_body);
	//发送数据
	send(sock, (char *)packet.data, packet.size,0);
	
	//发送上报设备状态
	memset(packet.data, 0, sizeof(packet.data));
	memset(&head, 0, sizeof(head));
	memset(&RepDevStat, 0, sizeof(RepDevStat));
	SendReportDeviceStatus(&packet, &head, &RepDevStat);
	send(sock, (char *)packet.data, packet.size,0);
	/*

	//上报设备数据
	memset(packet.data, 0, sizeof(packet.data));
	memset(&head, 0, sizeof(head));
	memset(&ReData, 0, sizeof(ReData));
	SendReportData(&packet, &head, &ReData);
	SendtoServer(sock, (char *)packet.data, packet.size);
	
	
	//批量上报数据
	memset(packet.data, 0, sizeof(packet.data));
	memset(&head, 0, sizeof(head));
	memset(&ReBatchData, 0, sizeof(ReBatchData));
	SendReportBatchData(&packet, &head, &ReBatchData);
	SendtoServer(sock, (char *)packet.data, packet.size);
		

	getpacketchar(&packet);	*/
    //关闭套接字
    closesocket(sock);
    //终止使用 DLL
    WSACleanup();
   system("pause");
    return 0;
}



	