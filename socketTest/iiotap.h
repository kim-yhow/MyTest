#ifndef IIOTAP
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <WinSock2.h>

#define ENCODE_SIZE 8
#define STR_SIZE 50
#define SIZE   50
#define DATA_SIZE   10
#define STATUS_NUM   20
#define USHORT_SIZE 2
#define DEVICE_NUM  8


#define IEEE_FLOAT_SIZE  4
#define IEEE_DOUBLE_SIZE 8

#define MSG_SERIAL 1

//������Ϣ��
#define COMMOMRESP		   0x80		//ͨ����Ӧ��Ϣ���ڲ�����һ����Ӧ��
#define REGISTERREQ		   0x45     //ע����Ϣ���������ƶ�ע��
#define REGISTERRESP	   0x85	
#define UNREGISTERREQ	   0x41		//ע����Ϣ���������ƶ�ע��
#define PINGREQ		       0x46		//���ӱ�����Ϣ
#define PINGRESP		   0x86
#define REPORTDATANOTIF				   0x05		//�ϱ��豸����
#define REPORTDEVOCESTATUSNOTIF		   0x06		//�ϱ��豸״̬������״̬��������ɾ����
//�ƶ˷������Ϣ
#define	GETDEVICELISTREQ	  0x50
#define GETDECIVELISTRESP     0X90
#define SENDDECICECOMMANDREQ  0X55
#define SENDDEVICECOMMANDRESP 0X95

//�������ͱ����
#define BOOLEAN_IDX		0
#define UBYTE_IDX		1
#define BYTE_IDX		2
#define USHORT_IDX	    3
#define SHORT_IDX		4
#define UINT_IDX		5
#define INT_IDX			6
#define ULONG_IDX		7
#define LONG_IDX		8
#define STRING_IDX		9
#define FLOAT_IDX		10
#define DOUBLE_IDX		11
#define DATE_IDX		12
#define DATETIME_IDX	13
#define UBYTEF_IDX		14
#define BYTEF_IDX		15
#define USHORTF_IDX		16
#define SHORTF_IDX		17
#define UINTF_IDX		18
#define INTF_IDX		19
#define ULONGF_IDX		20
#define LONGF_IDX		21
#define VINT_IDX		22
#define BYTEARRAY		23

//���ݳ���
#define BOOLEAN_SIZE 1
#define UBYTE_SIZE 1
#define BYTE_SIZE 1
#define USHORT_SIZE 2
#define SHORT_SIZE 2
#define UINT_SIZE 4
#define INT_SIZE 4
#define ULONG_SIZE 8
#define LONG_SIZE 8
#define FLOAT_SIZE 4
#define DOUBLE_SIZE 8
#define DATE_SIZE 4
#define DATETIME_SIZE 8
#define UBYTEF_SIZE 1
#define BYTEF_SIZE 1
#define USHORTF_SIZE 2
#define SHORTF_SIZE 2
#define UINTF_SIZE 4
#define INTF_SIZE 4
#define ULONGF_SIZE 8
#define LONGF_SIZE 8

//��������
#define SIMPLE_SEND_DATA			0x05
//��������
#define BARCH_SEND_DATA				0x06



//���������д�packet����
typedef struct {
	//��С
	uint8_t  size;
	//��������
	uint8_t *data;
}iiot_packet;
//�ɱ����α���
typedef struct{
	//����
	uint8_t encode[ENCODE_SIZE];				
	//������ֵ
	uint32_t value;	
	//���볤��
	uint16_t encodesize;
} vint;
//string�Ľṹ�����
typedef struct{
	//����
	uint8_t ecodecont[STR_SIZE];					
	//�ַ������ȣ���vint��ʾ
	vint    strlen_vint;
	//��Ϣ�����С
	uint16_t msgEncodsize;
} str_msg;

typedef struct{
	uint8_t ecodecont[USHORT_SIZE];
	uint8_t ushortEncodesize;
	uint16_t ushort_value;
} ushort;

typedef struct {
	uint8_t int_flag;			//����
	uint32_t int_value;
	uint8_t  intEncode[INT_SIZE];

	uint16_t EncodeSize;
} intEn;

typedef struct{
	uint8_t  Length;			//�ײ�������λΪ0
	uint8_t  T_Code;			//type����
	vint   PayLength_vint;			//payLoad����
	ushort  Msg_Serial;			//��Ϣid��ţ�ushort����
	vint  DeviceAddress_vint;		//�豸��ַ������1-4�ֽ�
} Iiotap_Head;

typedef struct{
	uint8_t registerFLag;					//��־λ
	str_msg protocolName;
	uint8_t protocolVersion;				//�̶�λ 1
	str_msg username;						//�û���
	str_msg  password;						//����	
    str_msg gatewayVendor;					//���س���
	str_msg gatewayModel;					//�����ͺ�
	str_msg gatewaySerial;					//�������к�
	str_msg gatewayHardwareVersion;		//����Ӳ���汾	
	str_msg gatewaySoftwareVersion;		//��������汾

	uint32_t payloadlength;				//��Ϣ�ܳ���
	
} Iiotap_reg_payLoad;
//
typedef struct{
	vint deviceNumber;
	uint8_t status[STATUS_NUM];
	vint deviceAddr[STATUS_NUM*8];

	uint16_t statusSize;
	uint32_t payloadlength;				//��Ϣ�ܳ���
} Iiotap_DeviceListResp_payLoad;
//�ϱ��豸״̬��payLoad
typedef struct {
	vint deviceNumber;
	vint deviceAddress[DEVICE_NUM];
	uint8_t status[STATUS_NUM];


	uint32_t payloadlength;				//��Ϣ�ܳ���
} Iiotap_ReportDeviceStatusNotif_payLoad;

typedef struct {
	ushort year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t milisecond;

	uint8_t encode[ENCODE_SIZE];
	uint16_t encodesize;
}datetime;

typedef struct{
	str_msg tag;
	uint8_t type;					//��ʾ��������
	uint8_t valueEncode[SIZE];		//���ɶ�����֮��Ĵ���	
	uint16_t encodeSize;			//��ʾencode�Ĵ�С

	uint16_t DataSize;
} DATA;

typedef struct {
	uint8_t taskCode;
	uint8_t reportFlag;
	datetime timeStamp;

	vint dataNumber;
	DATA data[DATA_SIZE];

	uint32_t payloadlength;				//��Ϣ�ܳ���
}Iiotap_ReportDataNotif_payLoad;

typedef union {
	
	float ieee_float_value;

	uint8_t encode[IEEE_FLOAT_SIZE];

} ieee_float_union;

typedef struct {
	//����
	uint8_t encode[IEEE_FLOAT_SIZE];
	//�����С
	uint8_t ieee_floatEncodesize;
	//��ֵ
	uint32_t ieee_float_value;

} ieee_float;

typedef union {
	double ieee_double_value;
	uint8_t encode[IEEE_DOUBLE_SIZE];
} ieee_double_union;

typedef struct {
	//����
	uint8_t encode[IEEE_DOUBLE_SIZE];
	//�����С
	uint8_t ieee_doubleEncodesize;
	//��ֵ
	uint64_t ieee_double_value;
} ieee_double;

typedef struct {
	vint deviceAddress;
	ieee_float timeoffset;

	vint dataNumber;
	DATA data[DATA_SIZE];

	uint32_t DevEncodeSize;
}Device;

typedef struct {
		uint8_t taskCode;
		uint8_t reportFlag;
		datetime timeStamp;
		vint deviceNumber;

		Device device[DEVICE_NUM];

		uint32_t payloadlength;				//��Ϣ�ܳ���
}Iiotap_Batch_ReportDataNotif_payLoad;

//ע����Ϣ
void SendRegisterReq(iiot_packet * packet, Iiotap_Head* header, Iiotap_reg_payLoad* reg_body);
//��Ӧ��ѯ��Ϣ
void GetDeviceListResp(iiot_packet * packet,Iiotap_Head* header,Iiotap_DeviceListResp_payLoad *DevResp);
//�ϱ�״̬��Ϣ
void SendReportDeviceStatus(iiot_packet * packet, Iiotap_Head* header, Iiotap_ReportDeviceStatusNotif_payLoad * RepDevStat);
//�ϱ�������Ϣ
void SendReportData(iiot_packet * packet, Iiotap_Head* header, Iiotap_ReportDataNotif_payLoad *ReportData );
//�����ϱ�������Ϣ
void SendReportBatchData(iiot_packet * packet, Iiotap_Head* header, Iiotap_Batch_ReportDataNotif_payLoad *BatchRepData);
//�ϱ�����������Ϣ
void SendKeepLink(iiot_packet * packet, Iiotap_Head* header);
//��Ӧ�豸����
void  GetDeviceComReq(uint8_t *buf, char* data, int *Size);


//header
int assemble_header(Iiotap_Head* header,uint8_t type, uint32_t paylength,uint16_t msg_id,uint32_t DeviceAddress);

//body
void assemble_reg_payLoad(Iiotap_reg_payLoad * reg,char *username,char *password);
void assemble_DeviceListResp_payLoad(Iiotap_DeviceListResp_payLoad *DeviceResp, uint16_t deviceNum, uint8_t *status, uint32_t * deviceAddr);
void assemble_ReportDeviceStatusNotif_payLoad(Iiotap_ReportDeviceStatusNotif_payLoad * RepDevStat, uint16_t deviceNum, uint8_t *status, uint32_t * deviceAddr);
void assemble_ReportDataNotif_payLoad(Iiotap_ReportDataNotif_payLoad * ReportData,uint8_t reportFlag,uint16_t dataNum);
void assemble_Batch_ReportDataNotif_payLoad(Iiotap_Batch_ReportDataNotif_payLoad *BatchRepData, uint8_t reportFlag,uint32_t DevNum);

//ƴ��
void joint_header(iiot_packet * packet,Iiotap_Head* header);
void joint_reg_body(iiot_packet * packet,Iiotap_reg_payLoad* reg);
void joint_DeviceListResp_body(iiot_packet * packet, Iiotap_DeviceListResp_payLoad * DeviceListResp);
void joint_ReportDeviceStatusNotif_payLoad(iiot_packet * packet, Iiotap_ReportDeviceStatusNotif_payLoad * RepDevStat);
void joint_ReportDataNotif_body(iiot_packet * packet, Iiotap_ReportDataNotif_payLoad * ReportData);
void joint_Batch_ReportDataNotif_payLoad(iiot_packet *packet, Iiotap_Batch_ReportDataNotif_payLoad *BatchRepData);

//���뺯��
uint16_t getVintencode(vint * value);
uint16_t getStrencode(str_msg* msg,char * temp);
void getUshortencode(ushort * value);
void getintEncode(intEn *value);
uint16_t getTimeEncode(datetime* t, uint16_t y, uint8_t m, uint8_t d, uint8_t h, uint8_t min, uint8_t s, uint8_t ms);
uint8_t getIEEE_floatencode(ieee_float_union* union_value, ieee_float * value);
uint8_t getIEEE_doubleencode(ieee_double_union * union_value, ieee_double * value);
void convert_to_utc(datetime* t);
//vint�Ľ��뺯��
uint32_t getVintValue(uint8_t * vtmp, uint16_t* idx);

//װ������
uint16_t SetDataInReport(DATA *data, char * tag, uint8_t type, uint8_t *value,uint16_t EncodeSize);
uint16_t SetDevInfo(Device*dev,uint32_t devAddress,float timeoffset);

//���
void getpacketchar(iiot_packet *packet);
void dump_mem(uint8_t* mem, uint16_t length);
//���͵�������
void SendtoServer(SOCKET sock, char* data, int Size);
#endif



